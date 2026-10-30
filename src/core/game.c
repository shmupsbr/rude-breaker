/**
 * Game state machine. Platform-agnostic; talks to hardware only via hal.h.
 */
#include "game.h"
#include "diskimg.h"
#include "player.h"
#include "enemy.h"
#include "collision.h"
#include "stage.h"
#include "stage_data.h"
#include "assets.h"
#include "exepath.h"
#include "vm.h"
#include "gfx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static RbEnemyPool  g_enemies;
static RbBulletPool g_bullets;
static RbAssets     g_assets;
static RbVm        *g_vm;
static int          g_smoke_stop;
static int          g_ship_based;
static int16_t      g_ship_base_x;
static int          g_saw_move;
static int          g_saw_clear;
static int          g_saw_over;
static int          g_data_missing;
/* Host options. Default is the modern palette, training off and rank
 * on. */
static int          g_modern = 1;
static int          g_training;
static int          g_rank_on = 1;
/* 30th AE intro card before the Compile logo: about 3 s at the
 * game's 30.67 Hz logic rate, 12-frame fades in and out. */
#define CARD_FRAMES 92
#define CARD_FADE   12
static int          g_card;
static int          g_card_hold;
/* Old settings files predate the pr9p defaults (modern palette, ship
 * select off). They are moved over once. */
static int          g_cfg_v9p;
static int          g_menu_latched;
static uint16_t     g_menu_prev = 0xFFFFu;
static int          g_suspend;
static int          g_suspend_closed;
static int          g_page;
static int          g_cursor;
static int          g_capture;
static int          g_handoff;
static int          g_bgm_no;
static int          g_se_no;
static int          g_pl_kind;
static int          g_pl;
static int          g_joy_was;
static int          g_joy_lockout;
/* Set by a VM iteration that flips the page. Frame skip leaves it
 * clear so the picture already on screen stays up. */
static int          g_show_frame = 1;

static void cfg_path(const RbGame *g, char *out, size_t n)
{
    struct stat st;
    char dir[512];
    char exe[1024];
    char *slash;

    /* Bindings, palette, and training sit beside the executable. */
    if (rb_exe_dir(exe, sizeof exe) == 0) {
        snprintf(out, n, "%s/rudebreaker.cfg", exe);
        return;
    }
    if (!g || !g->asset_dir[0]) {
        snprintf(out, n, "%s", "rudebreaker.cfg");
        return;
    }
    snprintf(dir, sizeof dir, "%s", g->asset_dir);
    if (stat(dir, &st) != 0 || !S_ISDIR(st.st_mode)) {
        slash = strrchr(dir, '/');
        if (slash && slash != dir) {
            *slash = 0;
        } else {
            snprintf(dir, sizeof dir, "%s", ".");
        }
    }
    snprintf(out, n, "%s/rudebreaker.cfg", dir);
}

static void load_cfg(const RbGame *g)
{
    char path[600];
    FILE *fp;
    char line[80];

    g_modern = 1;
    g_training = 0;
    g_rank_on = 1;
    g_cfg_v9p = 0;
    sys_bind_restore();
    cfg_path(g, path, sizeof path);
    fp = fopen(path, "r");
    if (!fp) {
        g_cfg_v9p = -1;
        return;
    }
    while (fgets(line, sizeof line, fp)) {
        if (strncmp(line, "palette=modern", 14) == 0) {
            g_modern = 1;
        } else if (strncmp(line, "palette=original", 16) == 0) {
            g_modern = 0;
        } else if (strncmp(line, "training=on", 11) == 0) {
            g_training = 1;
        } else if (strncmp(line, "training=off", 12) == 0) {
            g_training = 0;
        } else if (strncmp(line, "rank=on", 7) == 0) {
            g_rank_on = 1;
        } else if (strncmp(line, "rank=off", 8) == 0) {
            g_rank_on = 0;
        } else if (strncmp(line, "defaults=9q", 11) == 0) {
            g_cfg_v9p = 1;
        } else if (strncmp(line, "defaults=9p", 11) == 0) {
            if (g_cfg_v9p != 1) {
                g_cfg_v9p = 2;
            }
        } else {
            sys_bind_read_line(line);
        }
    }
    fclose(fp);
}

/* ---- in-match state: speed button and rank ----------------------------
 *
 * While a match is on screen the host owns two values the menu also
 * writes: the speed bits of [bp+0x72] (the speed button changes them)
 * and the C9 copy enemy shots take at spawn (rank raises it; C9 itself
 * stays on the chosen difficulty). The menu values are kept in
 * g_live_menu_speed / g_live_c9 and are what RBRANK.DAT stores; live_park
 * puts them back for the export and live_unpark restores the match. */
static int      g_live;            /* 1 while the stage HUD is up */
static int      g_live_parked;
static int      g_live_menu_speed; /* 0 slow, 1 normal, 2 fast */
static int      g_live_speed;
static int      g_live_dir = 1;
static uint8_t  g_live_c9;         /* the difficulty's own C9 */
static uint8_t  g_live_c9_now;     /* rank-adjusted C9 for shot spawns */
static uint16_t g_live_prev_btn;
static uint32_t g_live_prev_score;
static int      g_live_prev_lives;
/* Rank, 8.8 fixed point, 0..RANK_MAX (Zanac's 0x9F scale). */
#define RANK_MAX  (0x9Fu << 8)
#define RANK_HALF (0x50u << 8)
static uint32_t g_rank;

static uint16_t flag_word(void);
static void set_flag_word(uint16_t f);
static int speed_index(void);
static void set_speed(int i);

static void live_park(void)
{
    if (!g_vm || !g_live || g_live_parked) {
        return;
    }
    set_speed(g_live_menu_speed);
    vm_set_global_u8(g_vm, 0xC9, g_live_c9);
    g_live_parked = 1;
}

static void live_unpark(void)
{
    if (!g_vm || !g_live_parked) {
        return;
    }
    set_speed(g_live_speed);
    vm_set_global_u8(g_vm, 0xC9, g_live_c9);
    g_live_parked = 0;
}

static uint8_t  g_rank_c9;
static uint16_t g_rank_b2;
static uint16_t g_rank_stock;

/* Ranking stays beside the data folder. The settings file is the one
 * next to the executable. */
static void rank_path(const RbGame *g, char *out, size_t n)
{
    struct stat st;
    char dir[512];
    char *slash;

    if (!g || !g->asset_dir[0]) {
        snprintf(out, n, "%s", "RBRANK.DAT");
        return;
    }
    snprintf(dir, sizeof dir, "%s", g->asset_dir);
    if (stat(dir, &st) != 0 || !S_ISDIR(st.st_mode)) {
        slash = strrchr(dir, '/');
        if (slash && slash != dir) {
            *slash = 0;
        } else {
            snprintf(dir, sizeof dir, "%s", ".");
        }
    }
    snprintf(out, n, "%s/RBRANK.DAT", dir);
}

static void rank_snapshot(void)
{
    if (!g_vm) {
        return;
    }
    g_rank_c9 = vm_global_u8(g_vm, 0xC9);
    g_rank_b2 = vm_global_u16(g_vm, 0xB2);
    g_rank_stock = vm_global_u16(g_vm, 0x62);
}

static int rank_header_changed(void)
{
    if (!g_vm) {
        return 0;
    }
    return vm_global_u8(g_vm, 0xC9) != g_rank_c9 ||
           vm_global_u16(g_vm, 0xB2) != g_rank_b2 ||
           vm_global_u16(g_vm, 0x62) != g_rank_stock;
}

/* Identical bytes stay on disk. A training run never gets here. */
static void rank_flush(const RbGame *g)
{
    char path[600];
    uint8_t fresh[230];
    uint8_t old[230];
    FILE *fp;
    size_t n = 0;

    if (!g || !g_vm || g_training) {
        return;
    }
    live_park();
    vm_rank_export(g_vm, fresh);
    rank_path(g, path, sizeof path);
    fp = fopen(path, "rb");
    if (fp) {
        n = fread(old, 1, sizeof old, fp);
        if (n == sizeof old && fgetc(fp) != EOF) {
            n = 0;
        }
        fclose(fp);
        if (n == sizeof old && memcmp(old, fresh, sizeof fresh) == 0) {
            rank_snapshot();
            live_unpark();
            return;
        }
    }
    fp = fopen(path, "wb");
    if (!fp) {
        live_unpark();
        return;
    }
    if (fwrite(fresh, 1, sizeof fresh, fp) == sizeof fresh) {
        rank_snapshot();
    }
    fclose(fp);
    live_unpark();
}

static void rank_load(RbGame *g)
{
    char path[600];
    uint8_t buf[230];
    FILE *fp;
    size_t n = 0;
    int exact = 0;

    rank_path(g, path, sizeof path);
    fp = fopen(path, "rb");
    if (fp) {
        n = fread(buf, 1, sizeof buf, fp);
        exact = (n == sizeof buf && fgetc(fp) == EOF);
        fclose(fp);
    }
    if (exact) {
        vm_rank_load(g_vm, buf, n);
    } else {
        vm_rank_load(g_vm, NULL, 0);
    }
    rank_snapshot();
    if (!exact) {
        rank_flush(g);
    }
}

static void rank_sync(const RbGame *g)
{
    int commit = g_vm && vm_take_rank_commit(g_vm);

    if (!g_vm || g_training) {
        return;
    }
    if (commit) {
        rank_flush(g);
        return;
    }
    live_park();
    commit = rank_header_changed();
    live_unpark();
    if (commit) {
        rank_flush(g);
    }
}

static void save_cfg(const RbGame *g)
{
    char path[600];
    FILE *fp;

    cfg_path(g, path, sizeof path);
    fp = fopen(path, "w");
    if (!fp) {
        return;
    }
    fprintf(fp, "palette=%s\ntraining=%s\nrank=%s\ndefaults=9q\n",
            g_modern ? "modern" : "original",
            g_training ? "on" : "off",
            g_rank_on ? "on" : "off");
    sys_bind_write(fp);
    fclose(fp);
}

static void apply_host_options(void)
{
    if (!g_vm) {
        return;
    }
    vm_set_training(g_vm, g_training);
    vm_set_grade(g_vm, g_modern);
}

static int btn_pressed(const RbGame *g, uint16_t mask);

static int script_settings(void)
{
    /* SETTINGS, the two JIS characters behind that English label. */
    return g_vm && vm_text_has_jis(g_vm, 0x405Fu, 0x446Au);
}

static int menu_visible(void)
{
    return g_menu_latched && !g_suspend && g_handoff == 0;
}

enum {
    PAGE_ROOT = 0,
    PAGE_GAME,
    PAGE_CTRL,
    PAGE_KEYS,
    PAGE_SOUND
};

static uint16_t flag_word(void)
{
    return vm_global_u16(g_vm, 0x72);
}

static void set_flag_word(uint16_t f)
{
    vm_set_global_u8(g_vm, 0x72, (uint8_t)f);
    vm_set_global_u8(g_vm, 0x73, (uint8_t)(f >> 8));
}

static int sound_on(void)
{
    return g_vm && (vm_global_u8(g_vm, 0xB9) & 0x04u) == 0;
}

static void menu_beep(void)
{
    if (sound_on()) {
        sys_play_sound(0x1D, 180);
    }
}

static void menu_ok(void)
{
    if (sound_on()) {
        sys_play_sound(0x1C, 180);
    }
}

/* Difficulty pairs the original left/right handler writes.
 * C9 / B2: 0/0, 4/1, 7/2, 10/3, 28/3. 7/2 is the loader default. */
static const uint8_t k_c9[5] = {0, 4, 7, 10, 28};
static const uint8_t k_b2[5] = {0, 1, 2, 3, 3};
static const char *const k_diff[5] = {
    "VERY EASY", "EASY", "NORMAL", "HARD", "VERY HARD"
};

static int diff_index(void)
{
    uint8_t c9 = vm_global_u8(g_vm, 0xC9);
    int i;

    for (i = 0; i < 5; i++) {
        if (k_c9[i] == c9) {
            return i;
        }
    }
    return 2;
}

static void set_diff(int i)
{
    if (i < 0) {
        i = 0;
    }
    if (i > 4) {
        i = 4;
    }
    vm_set_global_u8(g_vm, 0xC9, k_c9[i]);
    vm_set_global_u8(g_vm, 0xB2, k_b2[i]);
    vm_set_global_u8(g_vm, 0xB3, 0);
}

/* Bit 0x10 is slow, bit 0x20 is fast, neither is normal. */
static int speed_index(void)
{
    uint16_t f = flag_word();

    if (f & 0x0010u) {
        return 0;
    }
    if (f & 0x0020u) {
        return 2;
    }
    return 1;
}

static void set_speed(int i)
{
    uint16_t f = (uint16_t)(flag_word() & (uint16_t)~0x0030u);

    if (i <= 0) {
        f = (uint16_t)(f | 0x0010u);
    } else if (i >= 2) {
        f = (uint16_t)(f | 0x0020u);
    }
    set_flag_word(f);
}

/* RETURN FIRE. Bit 0x0008 set is the OFF label. Clear is ON. */
static int suicide_on(void)
{
    return (flag_word() & 0x0008u) == 0;
}

/* SHIP SELECT. Bit 0x0080 set skips the scene (the OFF label). */
static int ship_on(void)
{
    return (flag_word() & 0x0080u) == 0;
}

static int joy_flag(void)
{
    return (flag_word() & 0x0400u) != 0;
}

static void set_joy(int on)
{
    uint16_t f = flag_word();

    if (on) {
        if (!sys_joy_present() && (vm_global_u8(g_vm, 0xB9) & 0x08u) == 0) {
            return;
        }
        set_flag_word((uint16_t)(f | 0x0400u));
        g_joy_lockout = 0;
    } else {
        set_flag_word((uint16_t)(f & (uint16_t)~0x0400u));
        g_joy_lockout = 1;
    }
}

static void set_sound(int on)
{
    uint8_t b = vm_global_u8(g_vm, 0xB9);

    if (on) {
        vm_set_global_u8(g_vm, 0xB9, (uint8_t)(b & (uint8_t)~0x04u));
        sys_play_sound(0x1D, 180);
        sys_play_music(0x0D);
    } else {
        vm_set_global_u8(g_vm, 0xB9, (uint8_t)(b | 0x04u));
        sys_audio_pause(0);
        sys_stop_music();
        g_pl = 0;
    }
}

static void player_stop(void)
{
    sys_audio_pause(0);
    sys_stop_music();
    g_pl = 0;
}

static void player_play(void)
{
    if (!sound_on()) {
        return;
    }
    sys_audio_pause(0);
    if (g_pl_kind) {
        sys_play_sound((uint16_t)g_se_no, 180);
    } else {
        sys_play_music((uint16_t)g_bgm_no);
    }
    g_pl = 1;
}

static void player_pause(void)
{
    if (g_pl == 1) {
        sys_audio_pause(1);
        g_pl = 2;
    } else if (g_pl == 2) {
        sys_audio_pause(0);
        g_pl = 1;
    }
}

static void joy_auto(void)
{
    int present;

    if (!g_vm) {
        return;
    }
    present = sys_joy_present();
    if (present && !g_joy_was && !g_joy_lockout) {
        set_flag_word((uint16_t)(flag_word() | 0x0400u));
    }
    g_joy_was = present;
}

static int page_count(void)
{
    switch (g_page) {
    case PAGE_GAME:  return 9;
    case PAGE_CTRL:  return 3;
    case PAGE_KEYS:  return 11;
    case PAGE_SOUND: return 7;
    default:         return 6;
    }
}

static void enter_page(int page)
{
    if (g_page == PAGE_SOUND && page != PAGE_SOUND) {
        player_stop();
    }
    g_page = page;
    g_cursor = 0;
    g_capture = 0;
}

static void handoff(int e0)
{
    if (g_page == PAGE_SOUND) {
        player_stop();
    }
    g_handoff = e0;
}

static void change_value(RbGame *g, int dir)
{
    int changed = 0;

    (void)g;
    if (g_page == PAGE_GAME) {
        if (g_cursor == 0) {
            set_diff(diff_index() + dir);
            changed = 1;
        } else if (g_cursor == 1) {
            set_flag_word((uint16_t)(flag_word() ^ 0x0008u));
            changed = 1;
        } else if (g_cursor == 2) {
            set_flag_word((uint16_t)(flag_word() ^ 0x0080u));
            changed = 1;
        } else if (g_cursor == 3) {
            set_speed(speed_index() + dir);
            changed = 1;
        } else if (g_cursor == 4) {
            g_modern = !g_modern;
            changed = 1;
        } else if (g_cursor == 5) {
            g_training = !g_training;
            changed = 1;
        } else if (g_cursor == 6) {
            /* Host only, saved to rudebreaker.ini, not RBRANK.DAT. */
            sys_pace_set_pref_exact(!sys_pace_pref_exact());
            changed = 1;
        } else if (g_cursor == 7) {
            /* Host only, saved to the settings file. */
            g_rank_on = !g_rank_on;
            changed = 1;
        }
    } else if (g_page == PAGE_CTRL && g_cursor == 0) {
        int next = !joy_flag();

        if (dir < 0) {
            next = 0;
        } else if (dir > 0) {
            next = 1;
        }
        if (next != joy_flag()) {
            set_joy(next);
            changed = joy_flag() == next;
        }
    } else if (g_page == PAGE_SOUND) {
        if (g_cursor == 0) {
            set_sound(!sound_on());
            return;
        }
        if (g_cursor == 1) {
            g_bgm_no += dir;
            if (g_bgm_no < 0) {
                g_bgm_no = 0;
            }
            if (g_bgm_no > 47) {
                g_bgm_no = 47;
            }
            g_pl_kind = 0;
            changed = 1;
        } else if (g_cursor == 2) {
            g_se_no += dir;
            if (g_se_no < 0) {
                g_se_no = 0;
            }
            if (g_se_no > 47) {
                g_se_no = 47;
            }
            g_pl_kind = 1;
            changed = 1;
        }
    }
    if (changed) {
        apply_host_options();
        save_cfg(g);
        menu_beep();
    }
}

static void activate_row(RbGame *g)
{
    if (g_page == PAGE_ROOT) {
        if (g_cursor == 0) {
            enter_page(PAGE_GAME);
        } else if (g_cursor == 1) {
            enter_page(PAGE_CTRL);
        } else if (g_cursor == 2) {
            enter_page(PAGE_SOUND);
        } else if (g_cursor == 3) {
            handoff(3);
        } else if (g_cursor == 4) {
            handoff(4);
        } else {
            handoff(2);
        }
        menu_ok();
        return;
    }
    if (g_page == PAGE_GAME && g_cursor == 8) {
        enter_page(PAGE_ROOT);
        menu_ok();
        return;
    }
    if (g_page == PAGE_CTRL) {
        if (g_cursor == 1) {
            enter_page(PAGE_KEYS);
            menu_ok();
        } else if (g_cursor == 2) {
            enter_page(PAGE_ROOT);
            menu_ok();
        }
        return;
    }
    if (g_page == PAGE_KEYS) {
        if (g_cursor == 9) {
            sys_bind_restore();
            save_cfg(g);
            menu_ok();
        } else if (g_cursor == 10) {
            enter_page(PAGE_CTRL);
            menu_ok();
        } else if (g_cursor >= 0 && g_cursor < RB_ACT_COUNT) {
            g_capture = 1;
            sys_bind_begin();
            menu_ok();
        }
        return;
    }
    if (g_page == PAGE_SOUND) {
        if (g_cursor == 1) {
            g_pl_kind = 0;
            player_play();
        } else if (g_cursor == 2) {
            g_pl_kind = 1;
            player_play();
        } else if (g_cursor == 3) {
            g_pl_kind = 0;
            player_play();
        } else if (g_cursor == 4) {
            player_pause();
        } else if (g_cursor == 5) {
            player_stop();
        } else if (g_cursor == 6) {
            enter_page(PAGE_ROOT);
        }
        menu_ok();
    }
}

static void strip_menu_buttons(RbGame *g)
{
    g->input.buttons = (uint16_t)(g->input.buttons &
                                  (uint16_t)~(RB_BTN_UP | RB_BTN_DOWN |
                                               RB_BTN_LEFT | RB_BTN_RIGHT |
                                               RB_BTN_A | RB_BTN_B |
                                               RB_BTN_START));
    g->input.pad_buttons = 0;
}

/* Esc / pad B: one page up. On the root page it is the BACK row. */
static void go_back(void)
{
    switch (g_page) {
    case PAGE_KEYS:
        enter_page(PAGE_CTRL);
        g_cursor = 1;
        break;
    case PAGE_GAME:
        enter_page(PAGE_ROOT);
        break;
    case PAGE_CTRL:
        enter_page(PAGE_ROOT);
        g_cursor = 1;
        break;
    case PAGE_SOUND:
        enter_page(PAGE_ROOT);
        g_cursor = 2;
        break;
    default:
        handoff(2);
        break;
    }
    menu_ok();
}

static void host_menu_input(RbGame *g, uint16_t *swallowed)
{
    int up;
    int down;
    int left;
    int right;
    int fire;
    int back;
    int n;

    *swallowed = 0;
    joy_auto();
    if (!g_vm) {
        return;
    }
    if (g_handoff) {
        int e0 = g_handoff;

        g_handoff = 0;
        vm_set_global_u8(g_vm, 0xE0, (uint8_t)e0);
        g->input.buttons = (uint16_t)((g->input.buttons &
                                       (uint16_t)~(RB_BTN_UP | RB_BTN_DOWN |
                                                    RB_BTN_LEFT | RB_BTN_RIGHT |
                                                    RB_BTN_B)) |
                                      RB_BTN_A);
        g_menu_latched = 0;
        g_suspend = 1;
        g_suspend_closed = 0;
        g_capture = 0;
        return;
    }
    if (g_suspend) {
        if (!script_settings()) {
            g_suspend_closed = 1;
        }
        if (g_suspend_closed && script_settings()) {
            g_suspend = 0;
            g_suspend_closed = 0;
            g_menu_latched = 1;
            g_page = PAGE_ROOT;
            g_menu_prev = 0xFFFFu;
            g_cursor = 0;
        } else {
            return;
        }
    }
    if (!g_menu_latched) {
        if (!script_settings()) {
            return;
        }
        g_menu_latched = 1;
        g_page = PAGE_ROOT;
        g_cursor = 0;
        g_menu_prev = 0xFFFFu;
    }
    if (g_capture) {
        int kind = 0;
        int a = 0;
        int b = 0;
        int got = sys_bind_result(&kind, &a, &b);

        if (got > 0) {
            sys_bind_set(g_cursor, kind, a, b);
            save_cfg(g);
            g_capture = 0;
            menu_beep();
        } else if (got < 0) {
            g_capture = 0;
        }
        g_menu_prev = 0x3FFFu;
        strip_menu_buttons(g);
        return;
    }
    /* Menus read the keyboard and the pad together. The pad bits live
     * in pad_buttons (the VM only merges them while JOYSTICK is on), so
     * the menu keeps its own edge state over both. Pad: stick, d-pad or
     * hat move; A (or START) selects; B or BACK goes up one page.
     * Keyboard: arrows move; Z/X/Enter select; Esc goes up one page. */
    {
        uint16_t kb = g->input.buttons;
        uint16_t pd = g->input.pad_buttons;
        uint16_t nav = (uint16_t)((kb | pd) &
                                  (RB_BTN_UP | RB_BTN_DOWN |
                                   RB_BTN_LEFT | RB_BTN_RIGHT));
        uint16_t now;
        uint16_t edge;

        now = nav;
        if ((kb & (RB_BTN_A | RB_BTN_B)) || (pd & (RB_BTN_A | RB_BTN_START))) {
            now = (uint16_t)(now | 0x1000u);
        }
        if ((kb & RB_BTN_START) || (pd & (RB_BTN_B | RB_BTN_QUIT))) {
            now = (uint16_t)(now | 0x2000u);
        }
        edge = (uint16_t)(now & (uint16_t)~g_menu_prev);
        g_menu_prev = now;
        up = (edge & RB_BTN_UP) != 0;
        down = (edge & RB_BTN_DOWN) != 0;
        left = (edge & RB_BTN_LEFT) != 0;
        right = (edge & RB_BTN_RIGHT) != 0;
        fire = (edge & 0x1000u) != 0;
        back = (edge & 0x2000u) != 0;
    }
    n = page_count();
    if (up) {
        g_cursor--;
        if (g_cursor < 0) {
            g_cursor = n - 1;
        }
        menu_beep();
    } else if (down) {
        g_cursor++;
        if (g_cursor >= n) {
            g_cursor = 0;
        }
        menu_beep();
    } else if (left) {
        change_value(g, -1);
    } else if (right) {
        change_value(g, 1);
    } else if (fire) {
        activate_row(g);
    } else if (back) {
        go_back();
    }
    strip_menu_buttons(g);
}

static void host_menu_commit(RbGame *g, uint16_t swallowed)
{
    (void)g;
    (void)swallowed;
}

static void paint_option_row(int col, int value_col, int row,
                             const char *label, const char *value,
                             int value_width, int selected, int hot)
{
    uint8_t label_attr = selected ? 0x41u : 0xE1u;
    uint8_t value_attr = hot ? 0xC1u : 0xA1u;
    int blink = vm_global_u8(g_vm, 0xC8) != 0;

    if (selected && blink) {
        value_attr = 0x41u;
    }
    vm_text_write(g_vm, col, row, label, 0, label_attr);
    if (value) {
        vm_text_write(g_vm, value_col, row, value, value_width, value_attr);
    }
}

static const char *speed_name(void)
{
    switch (speed_index()) {
    case 0:  return "LOW";
    case 2:  return "MAX";
    default: return "MID";
    }
}

static const char *joy_value(void)
{
    if (!joy_flag()) {
        return sys_joy_present() ? "UNUSED" : "NONE";
    }
    if (sys_joy_name()[0]) {
        return sys_joy_name();
    }
    return "IN USE";
}

/* Text columns are 8 px; each menu character is a 16 px cell (two
 * columns). The longest label, RESTORE DEFAULTS, ends at column 35, so
 * values start at column 40 (x = 320) and get 18 characters. */
#define MENU_LABEL_COL 4
#define MENU_VALUE_COL 40
#define MENU_VALUE_W   18

static void paint_line(int index, int row, const char *label, const char *value,
                       int hot)
{
    paint_option_row(MENU_LABEL_COL, MENU_VALUE_COL, row, label, value,
                     value ? MENU_VALUE_W : 0, index == g_cursor, hot);
}

/* Rows are two text lines apart (one blank line between items). */
static int menu_row(int first, int index)
{
    return first + index * 2;
}

/* Sound Test spectrum, drawn in pixels after present. Like the
 * Distopika player's first layout: white bars, instant attack, about
 * 0.3 s release, plus a peak cap that holds and then falls. */
#define VU_N      16
#define VU_X0     64
#define VU_X1     576
#define VU_TOP    296
#define VU_BOTTOM 360
static float    g_vu_lv[VU_N];
static float    g_vu_pk[VU_N];
static uint32_t g_vu_pk_t[VU_N];
static uint32_t g_vu_last;

static void vu_update(void)
{
    float in[VU_N];
    uint32_t now = sys_ticks_ms();
    float dt = g_vu_last ? (float)(now - g_vu_last) / 1000.0f : 0.033f;
    float decay;
    int i;
    int got;

    if (dt <= 0.0f) {
        dt = 0.001f;
    }
    if (dt > 0.1f) {
        dt = 0.1f;
    }
    g_vu_last = now;
    memset(in, 0, sizeof in);
    got = sys_audio_bands(in, VU_N);
    /* Show whatever is audible (menu music included), not only a track
     * started from this page. Pause already reads as silence. */
    if (got <= 0) {
        memset(in, 0, sizeof in);
    }
    /* 1 - e^(-dt / 0.3), without libm in the core. */
    decay = dt / (0.3f + dt);
    for (i = 0; i < VU_N; i++) {
        float t = in[i];

        if (t >= g_vu_lv[i]) {
            g_vu_lv[i] = t;
        } else {
            g_vu_lv[i] += (t - g_vu_lv[i]) * decay;
        }
        if (g_vu_lv[i] < 0.002f) {
            g_vu_lv[i] = 0.0f;
        }
        if (g_vu_lv[i] >= g_vu_pk[i]) {
            g_vu_pk[i] = g_vu_lv[i];
            g_vu_pk_t[i] = now;
        } else if (now - g_vu_pk_t[i] > 450u) {
            g_vu_pk[i] -= dt * 0.9f;
            if (g_vu_pk[i] < g_vu_lv[i]) {
                g_vu_pk[i] = g_vu_lv[i];
            }
        }
    }
}

static void vu_fill(uint32_t *fb, int x0, int y0, int x1, int y1, uint32_t c,
                    int alpha)
{
    int x;
    int y;

    for (y = y0; y < y1; y++) {
        if (y < 0 || y >= RB_NATIVE_H) {
            continue;
        }
        for (x = x0; x < x1; x++) {
            uint32_t *d;
            uint32_t r;
            uint32_t g;
            uint32_t b;

            if (x < 0 || x >= RB_NATIVE_W) {
                continue;
            }
            d = &fb[(size_t)y * RB_NATIVE_W + (size_t)x];
            r = (((*d >> 24) & 0xFFu) * (uint32_t)(256 - alpha) +
                 ((c >> 24) & 0xFFu) * (uint32_t)alpha) >> 8;
            g = (((*d >> 16) & 0xFFu) * (uint32_t)(256 - alpha) +
                 ((c >> 16) & 0xFFu) * (uint32_t)alpha) >> 8;
            b = (((*d >> 8) & 0xFFu) * (uint32_t)(256 - alpha) +
                 ((c >> 8) & 0xFFu) * (uint32_t)alpha) >> 8;
            *d = (r << 24) | (g << 16) | (b << 8) | 0xFFu;
        }
    }
}

static void vu_draw(uint32_t *fb)
{
    int span = (VU_X1 - VU_X0) / VU_N;
    int gap = span / 4;
    int h = VU_BOTTOM - VU_TOP;
    int i;

    if (!fb) {
        return;
    }
    /* Faint well and baseline so a silent meter still reads as a meter. */
    vu_fill(fb, VU_X0 - 4, VU_TOP - 4, VU_X1 + 4 - gap, VU_BOTTOM + 3,
            0x101828FFu, 150);
    vu_fill(fb, VU_X0, VU_BOTTOM + 1, VU_X1 - gap, VU_BOTTOM + 2,
            0x6080A0FFu, 200);
    for (i = 0; i < VU_N; i++) {
        int x = VU_X0 + i * span;
        int bh = (int)(g_vu_lv[i] * (float)h + 0.5f);
        int ph = (int)(g_vu_pk[i] * (float)h + 0.5f);

        if (g_vu_lv[i] > 0.0f && bh < 2) {
            bh = 2;
        }
        if (bh > 0) {
            vu_fill(fb, x, VU_BOTTOM - bh, x + span - gap, VU_BOTTOM,
                    0xFFFFFFFFu, 235);
        }
        if (ph > 0 && g_pl != 0) {
            int py = VU_BOTTOM - ph - 3;

            vu_fill(fb, x, py, x + span - gap, py + 2, 0x60E0FFFFu, 255);
        }
    }
}

static int sound_page_visible(void)
{
    return menu_visible() && g_vm && g_page == PAGE_SOUND;
}

static void host_menu_paint(void)
{
    int row;
    int i;
    char num[8];
    char track[24];
    const char *title = "CONFIG";
    static const char *root_name[6] = {
        "GAMEPLAY", "CONTROLS", "SOUND TEST", "RANKING", "PASSWORD", "BACK"
    };
    static const char *key_name[11] = {
        "UP", "DOWN", "LEFT", "RIGHT", "FIRE", "MODE", "SHIFT", "PAUSE",
        "SPEED", "RESTORE DEFAULTS", "BACK"
    };

    if (!menu_visible() || !g_vm) {
        gfx_set_panel(0, 0, 0, 0, 0);
        return;
    }
    /* Darken the title art behind the menu so the rows do not sit on
     * the logo or on the (c) COMPILE 1996 line. */
    gfx_set_panel(1, 16, 8, 624, 392);
    for (row = 0; row < 25; row++) {
        vm_text_write(g_vm, 0, row, "", 40, 0);
    }
    if (g_page == PAGE_GAME) {
        title = "GAMEPLAY";
    } else if (g_page == PAGE_CTRL) {
        title = "CONTROLS";
    } else if (g_page == PAGE_KEYS) {
        title = "DEFINE KEYS";
    } else if (g_page == PAGE_SOUND) {
        title = "SOUND TEST";
    }
    vm_text_write(g_vm, MENU_LABEL_COL, 1, title, 0, 0xE1u);
    if (g_page == PAGE_ROOT) {
        for (i = 0; i < 6; i++) {
            paint_line(i, menu_row(4, i), root_name[i], NULL, 0);
        }
    } else if (g_page == PAGE_GAME) {
        paint_line(0, menu_row(4, 0), "DIFFICULTY", k_diff[diff_index()],
                   diff_index() != 2);
        paint_line(1, menu_row(4, 1), "SUICIDE BULLETS",
                   suicide_on() ? "ON" : "OFF", suicide_on());
        paint_line(2, menu_row(4, 2), "SHIP SELECT", ship_on() ? "ON" : "OFF",
                   ship_on());
        paint_line(3, menu_row(4, 3), "MOVE SPEED", speed_name(),
                   speed_index() != 1);
        paint_line(4, menu_row(4, 4), "PALETTE",
                   g_modern ? "MODERN" : "ORIGINAL", g_modern);
        paint_line(5, menu_row(4, 5), "TRAINING", g_training ? "ON" : "OFF",
                   g_training);
        paint_line(6, menu_row(4, 6), "FRAME PACING",
                   sys_pace_pref_exact() ? "EXACT" : "SMOOTH",
                   sys_pace_pref_exact());
        paint_line(7, menu_row(4, 7), "RANK", g_rank_on ? "ON" : "OFF",
                   g_rank_on);
        paint_line(8, menu_row(4, 8), "BACK", NULL, 0);
    } else if (g_page == PAGE_CTRL) {
        paint_line(0, menu_row(4, 0), "JOYSTICK", joy_value(), joy_flag());
        paint_line(1, menu_row(4, 1), "DEFINE KEYS", NULL, 0);
        paint_line(2, menu_row(4, 2), "BACK", NULL, 0);
        if (sys_joy_present()) {
            vm_text_write(g_vm, MENU_LABEL_COL, 12, "PAD FOUND", 0, 0xA1u);
            vm_text_write(g_vm, MENU_VALUE_COL, 12, sys_joy_name(),
                          MENU_VALUE_W, 0xA1u);
        } else {
            vm_text_write(g_vm, MENU_LABEL_COL, 12, "PLUG IN A PAD ANY TIME",
                          0, 0xA1u);
        }
    } else if (g_page == PAGE_KEYS) {
        for (i = 0; i < RB_ACT_COUNT; i++) {
            const char *value = sys_bind_label(i);

            if (g_capture && g_cursor == i) {
                value = "PRESS KEY OR PAD";
            }
            paint_line(i, menu_row(2, i), key_name[i], value, 0);
        }
        paint_line(9, menu_row(2, 9), key_name[9], NULL, 0);
        paint_line(10, menu_row(2, 10), key_name[10], NULL, 0);
    } else if (g_page == PAGE_SOUND) {
        const char *state = "STOP";

        snprintf(num, sizeof num, "%02d", g_bgm_no);
        paint_line(0, menu_row(3, 0), "SOUND", sound_on() ? "ON" : "OFF",
                   sound_on());
        paint_line(1, menu_row(3, 1), "BGM", num, g_pl_kind == 0 && g_pl == 1);
        snprintf(num, sizeof num, "%02d", g_se_no);
        paint_line(2, menu_row(3, 2), "SE", num, g_pl_kind == 1 && g_pl == 1);
        paint_line(3, menu_row(3, 3), "PLAY", NULL, 0);
        paint_line(4, menu_row(3, 4), "PAUSE", NULL, 0);
        paint_line(5, menu_row(3, 5), "STOP", NULL, 0);
        paint_line(6, menu_row(3, 6), "BACK", NULL, 0);
        if (g_pl == 1) {
            state = "PLAY";
        } else if (g_pl == 2) {
            state = "PAUSE";
        }
        snprintf(track, sizeof track, "%s %02d  %s",
                 g_pl_kind ? "SE" : "BGM",
                 g_pl_kind ? g_se_no : g_bgm_no, state);
        vm_text_write(g_vm, MENU_LABEL_COL, 17, "TRACK", 0, 0xE1u);
        vm_text_write(g_vm, MENU_VALUE_COL, 17, track, MENU_VALUE_W,
                      g_pl == 1 ? 0xC1u : 0xA1u);
    }
    if (g_capture) {
        vm_text_write(g_vm, MENU_LABEL_COL, 23, "ESC OR PAD BACK CANCELS", 0,
                      0xA1u);
    } else {
        vm_text_write(g_vm, MENU_LABEL_COL, 23,
                      "FIRE OK  ESC BACK  LEFT RIGHT SET", 0, 0xA1u);
    }
}

static int env_on(const char *name)
{
    const char *v = getenv(name);

    return v && v[0] && strcmp(v, "0") != 0;
}

static int env_is(const char *name, const char *want)
{
    const char *v = getenv(name);

    return v && want && strcmp(v, want) == 0;
}

static const char *state_name(RbGameState st)
{
    switch (st) {
    case RB_STATE_BOOT:    return "boot";
    case RB_STATE_TITLE:   return "title";
    case RB_STATE_PLAY:    return "play";
    case RB_STATE_RANKING: return "ranking";
    case RB_STATE_QUIT:    return "quit";
    default:               return "other";
    }
}

static void smoke_trace(const RbGame *g, const char *why)
{
    if (!env_on("RUDEBR_TRACE")) {
        return;
    }
    if (g_vm && (g->state == RB_STATE_PLAY || g->state == RB_STATE_RANKING)) {
        RbVmInfo info;

        vm_info(g_vm, &info);
        fprintf(stderr,
                "trace: %s state=%s stage=%u ship=%d,%d chunk=%u rec=%u hdr=%u c5=%u frame=%u score=%u\n",
                why, state_name(g->state), (unsigned)g->stage,
                (int)info.ship_x, (int)info.ship_y,
                (unsigned)info.chunk, (unsigned)info.load_record,
                (unsigned)info.load_header,
                (unsigned)vm_global_u8(g_vm, 0xC5),
                (unsigned)g->frame, (unsigned)info.score);
        return;
    }
    fprintf(stderr, "trace: %s state=%s frame=%u\n",
            why, state_name(g->state), (unsigned)g->frame);
}

static const char *got(const void *p)
{
    return p ? "found" : "MISSING";
}

static void report_missing(const RbDisk *disk)
{
    char body[1600];
    const char *where = (disk && disk->label[0]) ? disk->label : "(unknown)";

    snprintf(body, sizeof(body),
             "%s:\n%s\n\n"
             "Copy these files from your own Rude Breaker disk into that folder,\n"
             "or drop the disk image there (or pass the image as the program argument).\n"
             "A PC-98 image works: .hdm, .fdi, .d88, .hdi, .nhd, or a zip of one of those.\n"
             "The program reads the files out of the FAT filesystem, including a RUDEBR folder.\n\n"
             "  SGM1.DAT     required   %s\n"
             "  FPLAY.COM    required   %s\n"
             "  SONG.DAT     required   %s\n"
             "  RBRANK.DAT   optional   %s\n\n"
             "SGC.COM, READJS.COM, and RUDEBR.BAT are on the disk and are not read.\n",
             (disk && disk->saw_image) ? "Opened disk image" : "Folder",
             where,
             got(disk ? disk->sgm1 : NULL),
             got(disk ? disk->fplay : NULL),
             got(disk ? disk->song : NULL),
             (disk && disk->rank) ? "found" : "absent");
    sys_message("Rude Breaker 30th AE — disk files needed", body);
}

static void watch_smoke(RbGame *g)
{
    const char *frames = getenv("RUDEBR_FRAMES");

    if (g_vm && g->state == RB_STATE_PLAY) {
        RbVmInfo info;

        vm_info(g_vm, &info);
        if (!g_ship_based && info.chunk >= 2 &&
            (info.ship_x != 0 || info.ship_y != 0)) {
            g_ship_based = 1;
            g_ship_base_x = info.ship_x;
        } else if (g_ship_based && !g_saw_move &&
                   info.ship_x >= (int16_t)(g_ship_base_x + 8)) {
            g_saw_move = 1;
            fprintf(stderr, "smoke: stage1 x=%d from %d frame=%u\n",
                    (int)info.ship_x, (int)g_ship_base_x, (unsigned)g->frame);
            if (env_is("RUDEBR_STOP", "stage1")) {
                g_smoke_stop = 1;
                g->state = RB_STATE_QUIT;
            }
        }
        if (!g_saw_clear && vm_global_u8(g_vm, 0xC5) >= 2 && info.chunk >= 3) {
            g_saw_clear = 1;
            fprintf(stderr, "smoke: stage-clear c5=%u chunk=%u frame=%u\n",
                    (unsigned)vm_global_u8(g_vm, 0xC5), (unsigned)info.chunk,
                    (unsigned)g->frame);
            if (env_is("RUDEBR_STOP", "clear")) {
                g_smoke_stop = 1;
                g->state = RB_STATE_QUIT;
            }
        }
        if (!g_saw_over && g->frame > 30 && info.chunk == 1 &&
            info.load_record == 1 && info.load_header == 2) {
            g_saw_over = 1;
            fprintf(stderr, "smoke: game-over frame=%u score=%u\n",
                    (unsigned)g->frame, (unsigned)info.score);
            if (env_is("RUDEBR_STOP", "over")) {
                g_smoke_stop = 1;
                g->state = RB_STATE_QUIT;
            }
        }
    }
    if (env_is("RUDEBR_STOP", "title") && g->frame >= 1 &&
        g->state == RB_STATE_TITLE) {
        g_smoke_stop = 1;
        g->state = RB_STATE_QUIT;
    }
    if (frames && frames[0] && g->state != RB_STATE_QUIT) {
        unsigned long lim = strtoul(frames, NULL, 10);

        if (lim > 0 && g->frame >= lim) {
            fprintf(stderr, "smoke: frame limit %lu\n", lim);
            g->state = RB_STATE_QUIT;
        }
    }
}

static int btn_pressed(const RbGame *g, uint16_t mask)
{
    return (g->input.buttons & mask) && !(g->input_prev.buttons & mask);
}

static void begin_stage(RbGame *g, uint16_t stage, int fresh_ship)
{
    int16_t lives = g->player.lives;

    if (g_vm) {
        /* The stage chunk's own script spawns the ship, the shot
         * controller, and the director. A later opcode 0x0D loads the
         * next chunk; this reset is only the title-to-play entry. */
        vm_reset_play(g_vm, stage);
        vm_set_hiscore(g_vm, g->hiscore);
        g->stage = stage;
        g->scroll_y = 0;
        g->scroll_speed = 0;
        player_reset(&g->player);
        if (!fresh_ship) {
            g->player.lives = lives;
        }
        return;
    }

    enemy_pool_init(&g_enemies);
    bullet_pool_init(&g_bullets);
    stage_begin(stage);
    g->stage = stage;
    g->scroll_y = stage_scroll_y();
    g->scroll_speed = stage_scroll_speed();
    player_reset(&g->player);
    if (!fresh_ship) {
        g->player.lives = lives;
    }
}

void game_init(RbGame *g, const RbDisk *disk)
{
    int scaffold = env_on("RUDEBR_SCAFFOLD");
    int have = disk && disk->ok;

    memset(g, 0, sizeof(*g));
    g->state = RB_STATE_BOOT;
    g->hiscore = 0;
    g->stage = 1;
    g->scroll_y = 0;
    g->scroll_speed = 0;
    if (disk && disk->label[0]) {
        snprintf(g->asset_dir, sizeof(g->asset_dir), "%s", disk->label);
    }

    player_reset(&g->player);
    enemy_pool_init(&g_enemies);
    bullet_pool_init(&g_bullets);

    if (!have) {
        if (scaffold) {
            fprintf(stderr, "rudebreaker: scaffold mode (RUDEBR_SCAFFOLD)\n");
            g->state = RB_STATE_TITLE;
            return;
        }
        g_data_missing = 1;
        g->state = RB_STATE_QUIT;
        report_missing(disk);
        return;
    }

    if (sys_audio_bind_mem(disk->fplay, disk->fplay_n, disk->song, disk->song_n,
                           disk->data_dir) == 0 &&
        assets_load_mem(&g_assets, disk->sgm1, disk->sgm1_n,
                        disk->song, disk->song_n,
                        disk->rank, disk->rank_n) == 0) {
        fprintf(stderr, "assets: loaded SGM1 (%u chunks, %zu bytes) from %s\n",
                (unsigned)g_assets.sgm1.count, g_assets.sgm1_size, g->asset_dir);
        fprintf(stderr, "rudebreaker: SGM1.DAT, FPLAY.COM, and SONG.DAT found\n");
        if (vm_create(&g_vm, &g_assets) != 0) {
            fprintf(stderr, "vm: create failed\n");
            g_vm = NULL;
            g_data_missing = 1;
            g->state = RB_STATE_QUIT;
            return;
        }
    } else {
        g_data_missing = 1;
        g->state = RB_STATE_QUIT;
        report_missing(disk);
        return;
    }

    vm_reset_boot(g_vm);
    rank_load(g);
    if (getenv("RUDEBR_STAGE")) {
        vm_reset_play(g_vm, (uint16_t)atoi(getenv("RUDEBR_STAGE")));
    }
    {
        RbVmInfo info;

        vm_info(g_vm, &info);
        g->hiscore = info.hiscore;
    }
    load_cfg(g);
    if (g_cfg_v9p != 1) {
        /* One-time move to the pr9p defaults: modern palette and ship
         * select OFF (weapons change in play through the capsules). */
        if (g_cfg_v9p == 0) {
            g_modern = 1;
        }
        if (g_cfg_v9p != 2) {
            set_flag_word((uint16_t)(flag_word() | 0x0080u));
        }
        /* pr9p update: MOVE SPEED defaults to MID, once. */
        set_speed(1);
        save_cfg(g);
        rank_flush(g);
    }
    apply_host_options();
    if (!getenv("RUDEBR_STAGE") && !env_on("RUDEBR_NOCARD")) {
        g_card = CARD_FRAMES;
    }
    g->state = RB_STATE_TITLE;
    if (env_on("RUDEBR_TRACE") || getenv("RUDEBR_STOP")) {
        fprintf(stderr, "smoke: title\n");
    }
    smoke_trace(g, "boot");
}

void game_shutdown(RbGame *g)
{
    rank_flush(g);
}

int game_data_missing(void)
{
    return g_data_missing;
}

int game_smoke_done(void)
{
    return g_smoke_stop;
}

int game_wants_quit(const RbGame *g)
{
    return g->state == RB_STATE_QUIT || (g->input.buttons & RB_BTN_QUIT);
}

unsigned game_vsyncs(const RbGame *g)
{
    (void)g;
    if (g_vm) {
        return vm_vsync_count(g_vm);
    }
    return 3;
}

int game_take_stage_gap(void)
{
    return g_vm && vm_take_stage_gap(g_vm);
}

/* One VM frame. Chunk 1 header 0 is the logo, title, and menu. A stage
 * chunk (2..6) is play. Header 0 loaded again returns to the title
 * without resetting the script that just loaded it. */
static void step_script(RbGame *g)
{
    RbInput in = g->input;
    RbVmInfo info;

    /* RUDEBR_HOLD latches fire from frame 1. The title prompt looks for
     * a later edge, so drop fire for one frame every 30 while the boot
     * chunk is still up. Real keyboards already produce that edge. */
    if (env_on("RUDEBR_HOLD")) {
        vm_info(g_vm, &info);
        if (info.chunk < 2 && (g->frame % 30u) == 0u) {
            in.buttons = (uint16_t)(in.buttons & (uint16_t)~RB_BTN_A);
        }
    }
    if (env_on("RUDEBR_SUSTAIN")) {
        vm_sustain_ship(g_vm);
    }
    vm_set_input(g_vm, &in);
    vm_frame(g_vm);
    vm_info(g_vm, &info);
    g->score = info.score;
    if (info.hiscore > g->hiscore) {
        g->hiscore = info.hiscore;
    }
    g->scroll_y = info.scroll_y;
    g->scroll_speed = info.speed;
    g->player.lives = info.lives;
    g->player.pos.x = info.ship_x;
    g->player.pos.y = info.ship_y;
    if (info.chunk >= 2 && info.chunk <= 6) {
        g->stage = (uint16_t)(info.chunk - 1u);
        if (g->state == RB_STATE_TITLE || g->state == RB_STATE_RANKING) {
            g->state = RB_STATE_PLAY;
        }
    }
    if (vm_quit_requested(g_vm)) {
        g->state = RB_STATE_QUIT;
    } else if (vm_boot_requested(g_vm)) {
        vm_ack_boot(g_vm);
        if (g->state != RB_STATE_QUIT) {
            g->state = RB_STATE_TITLE;
        }
    }
}

/* RUDEBR_SCRIPT=120:DOWN,140:A,200-208:RIGHT
 * One frame, or a range, of extra buttons. Tokens match RUDEBR_HOLD
 * and are joined with '+'. Headless menu shots use this. */
static uint16_t script_token(const char *s, size_t n)
{
    char buf[16];
    size_t i;

    if (n == 0 || n >= sizeof buf) {
        return 0;
    }
    for (i = 0; i < n; i++) {
        char c = s[i];

        if (c >= 'a' && c <= 'z') {
            c = (char)(c - 'a' + 'A');
        }
        buf[i] = c;
    }
    buf[n] = 0;
    if (strcmp(buf, "A") == 0 || strcmp(buf, "Z") == 0) {
        return RB_BTN_A;
    }
    if (strcmp(buf, "B") == 0 || strcmp(buf, "X") == 0) {
        return RB_BTN_B;
    }
    if (strcmp(buf, "SPEED") == 0) {
        return RB_BTN_SPEED;
    }
    if (strcmp(buf, "C") == 0) {
        return RB_BTN_C;
    }
    if (strcmp(buf, "UP") == 0) {
        return RB_BTN_UP;
    }
    if (strcmp(buf, "DOWN") == 0) {
        return RB_BTN_DOWN;
    }
    if (strcmp(buf, "LEFT") == 0) {
        return RB_BTN_LEFT;
    }
    if (strcmp(buf, "RIGHT") == 0) {
        return RB_BTN_RIGHT;
    }
    if (strcmp(buf, "START") == 0) {
        return RB_BTN_START;
    }
    if (strcmp(buf, "1") == 0) {
        return RB_BTN_1;
    }
    if (strcmp(buf, "2") == 0) {
        return RB_BTN_2;
    }
    if (strcmp(buf, "3") == 0) {
        return RB_BTN_3;
    }
    if (strcmp(buf, "4") == 0) {
        return RB_BTN_4;
    }
    return 0;
}

static uint16_t script_buttons_env(const char *env, uint32_t frame)
{
    const char *s = getenv(env);
    uint16_t bits = 0;

    if (!s || !s[0]) {
        return 0;
    }
    while (*s) {
        char *end = NULL;
        unsigned long a;
        unsigned long b;
        const char *tok;

        while (*s == ' ' || *s == ',') {
            s++;
        }
        if (!*s) {
            break;
        }
        a = strtoul(s, &end, 10);
        if (end == s) {
            break;
        }
        s = end;
        b = a;
        if (*s == '-') {
            s++;
            b = strtoul(s, &end, 10);
            s = end;
        }
        if (*s != ':') {
            break;
        }
        s++;
        tok = s;
        while (*s && *s != ',') {
            s++;
        }
        if (frame >= a && frame <= b) {
            const char *p = tok;

            while (p < s) {
                const char *q = p;

                while (q < s && *q != '+') {
                    q++;
                }
                bits = (uint16_t)(bits | script_token(p, (size_t)(q - p)));
                p = q < s ? q + 1 : q;
            }
        }
    }
    return bits;
}

/* Rank, after the Zanac port's ALC ("automatic level control"):
 * one byte on a 0..0x9F scale that climbs while the player does well
 * and is halved on a death. Zanac's difficulty rules carry over: the
 * easy settings never pass half rank, the hard settings start at half
 * rank, normal starts at zero with no cap. Rude Breaker's own
 * difficulty writes byte C9, which every enemy shot copies when it
 * spawns (op 0x08 into shot field 0x33). Rank adds up to RANK_SPAN on
 * top of the chosen difficulty's C9, never past VERY HARD's 28, and
 * only for that shot copy (vm_set_shot_c9). C9 itself always holds the
 * chosen difficulty, since the bonus tables compare it with 4/7/10/28. */
#define RANK_SPAN      7
#define RANK_PER_FRAME 4u    /* +1 rank about every 64 frames alive */
#define RANK_PER_SCORE 64u   /* +1/4 rank per frame the score moves */

static int live_match(void)
{
    return vm_text_cell(g_vm, 18, 1) == 0x4256u &&
           vm_text_cell(g_vm, 50, 1) == 0x3756u &&
           vm_text_cell(g_vm, 52, 1) == 0x3856u;
}

static int live_diff(void)
{
    int i;

    for (i = 0; i < 5; i++) {
        if (k_c9[i] == g_live_c9) {
            return i;
        }
    }
    return 2;
}

static void live_reset_match(void)
{
    g_live_speed = g_live_menu_speed;
    g_live_dir = g_live_menu_speed >= 2 ? -1 : 1;
    g_rank = live_diff() >= 3 ? RANK_HALF : 0u;
}

static void live_update(RbGame *g)
{
    RbVmInfo info;
    uint16_t flags;
    uint16_t btn;
    int lives;
    int on;

    if (!g_vm) {
        return;
    }
    /* Smoke test: hold the debug ending flag (NEKOCO key 4). */
    if (env_on("RUDEBR_ENDING")) {
        vm_set_global_u8(g_vm, 0xCC, 1);
    }
    on = live_match() && !menu_visible();
    vm_info(g_vm, &info);
    lives = vm_global_u8(g_vm, 0xD2);
    if (on && !g_live) {
        g_live_menu_speed = speed_index();
        g_live_c9 = vm_global_u8(g_vm, 0xC9);
        g_live_c9_now = g_live_c9;
        if (info.score == 0u) {
            live_reset_match();
        }
        g_live = 1;
        g_live_prev_score = info.score;
        g_live_prev_lives = lives;
        g_live_prev_btn = 0xFFFFu;
    } else if (!on && g_live) {
        set_speed(g_live_menu_speed);
        vm_set_global_u8(g_vm, 0xC9, g_live_c9);
        vm_set_shot_c9(g_vm, 0);
        g_live = 0;
    }
    if (!g_live) {
        gfx_set_hud_speed(0);
        return;
    }
    flags = flag_word();
    btn = g->input.buttons;
    if (flags & 0x0400u) {
        btn = (uint16_t)(btn | g->input.pad_buttons);
    }
    btn = (uint16_t)(btn & RB_BTN_SPEED);
    if ((flags & 0x8000u) == 0u && btn && !(g_live_prev_btn & RB_BTN_SPEED)) {
        /* LOW MID MAX MID LOW ... */
        int next = g_live_speed + g_live_dir;

        if (next > 2) {
            g_live_dir = -1;
            next = 1;
        } else if (next < 0) {
            g_live_dir = 1;
            next = 1;
        }
        g_live_speed = next;
    }
    g_live_prev_btn = btn;
    set_speed(g_live_speed);

    if ((flags & 0x8000u) == 0u) {
        uint32_t cap = live_diff() <= 1 ? RANK_HALF : RANK_MAX;

        if (lives < g_live_prev_lives) {
            g_rank >>= 1;
        } else {
            g_rank += RANK_PER_FRAME;
            if (info.score != g_live_prev_score) {
                g_rank += RANK_PER_SCORE;
            }
        }
        if (g_rank > cap) {
            g_rank = cap;
        }
        g_live_prev_score = info.score;
        g_live_prev_lives = lives;
    }
    g_live_c9_now = g_live_c9;
    if (g_rank_on && !g_training) {
        unsigned add = (unsigned)((g_rank >> 8) * RANK_SPAN + 0x4Fu) / 0x9Fu;
        unsigned c9 = g_live_c9 + add;

        if (c9 > k_c9[4]) {
            c9 = k_c9[4];
        }
        if (c9 < g_live_c9) {
            c9 = g_live_c9;
        }
        g_live_c9_now = (uint8_t)c9;
    }
    /* C9 stays on the chosen difficulty; only shot spawns see rank. */
    vm_set_global_u8(g_vm, 0xC9, g_live_c9);
    vm_set_shot_c9(g_vm, g_live_c9_now != g_live_c9 ? g_live_c9_now : 0u);
    gfx_set_hud_speed(g_live_speed + 1);
    if (env_on("RUDEBR_RANKLOG") && (g->frame % 60u) == 0u) {
        fprintf(stderr, "rank f=%u r=%u.%02u shot_c9=%u c9=%u hits=%u spd=%d lives=%d\n",
                (unsigned)g->frame, (unsigned)(g_rank >> 8),
                (unsigned)((g_rank & 0xFFu) * 100u / 256u),
                (unsigned)g_live_c9_now, (unsigned)vm_global_u8(g_vm, 0xC9),
                vm_shot_c9_hits(g_vm),
                g_live_speed, lives);
    }
}

void game_update(RbGame *g)
{
    uint16_t user_buttons;
    uint16_t swallowed = 0;

    g->frame++;
    g->input.buttons = (uint16_t)(g->input.buttons |
                                  script_buttons_env("RUDEBR_SCRIPT", g->frame));
    /* Same syntax, pressed on the gamepad side (pad_buttons). */
    g->input.pad_buttons = (uint16_t)(g->input.pad_buttons |
                                      script_buttons_env("RUDEBR_PADSCRIPT",
                                                         g->frame));
    user_buttons = g->input.buttons;
    host_menu_input(g, &swallowed);
    apply_host_options();

    switch (g->state) {
    case RB_STATE_BOOT:
        g->state = RB_STATE_TITLE;
        break;

    case RB_STATE_TITLE:
        if (g_vm && g_card > 0) {
            /* 30th AE card; trigger or Start skips it. The VM waits
             * until the button is let go, so the same press does not
             * also skip the Compile logo. */
            if (btn_pressed(g, RB_BTN_START) || btn_pressed(g, RB_BTN_A)) {
                g_card = 0;
                g_card_hold = 1;
            } else {
                g_card--;
            }
            g_show_frame = 1;
        } else if (g_vm && g_card_hold) {
            if (!(g->input.buttons & (RB_BTN_START | RB_BTN_A))) {
                g_card_hold = 0;
            }
            g_show_frame = 1;
        } else if (g_vm) {
            step_script(g);
        } else if (btn_pressed(g, RB_BTN_START) || btn_pressed(g, RB_BTN_A)) {
            g->score = 0;
            begin_stage(g, 1, 1);
            g->state = RB_STATE_PLAY;
            sys_play_music(0);
        }
        break;

    case RB_STATE_PLAY:
        if (g_vm) {
            /* One forward scan. A child spawned this frame runs only if
             * its slot is still ahead of the parent. RUDEBR_SUSTAIN is a
             * smoke-test switch; the frame loop itself never calls it.
             * Lives and the escape bit stay inside the script. */
            step_script(g);
        } else {
            /* Scaffold path used only when SGM1 failed to load. */
            player_update(&g->player, &g->input);
            stage_note_despawn(enemy_pool_update(&g_enemies));
            stage_tick(&g_enemies);
            g->scroll_y = stage_scroll_y();
            g->scroll_speed = stage_scroll_speed();
            bullet_pool_update(&g_bullets);
            if (btn_pressed(g, RB_BTN_A)) {
                RbBullet shot;

                memset(&shot, 0, sizeof(shot));
                shot.pos.x = (int16_t)(g->player.pos.x + 8);
                shot.pos.y = (int16_t)(g->player.pos.y - 8);
                shot.vel.y = -8;
                shot.hitbox.x = shot.pos.x;
                shot.hitbox.y = shot.pos.y;
                shot.hitbox.w = 4;
                shot.hitbox.h = 8;
                shot.damage = 1;
                shot.life = 90;
                bullet_spawn(&g_bullets, &shot);
                sys_play_sound(1, 200);
            }
            collision_player_vs_enemies(&g->player, &g_enemies);
            collision_bullets_vs_enemies(&g_bullets, &g_enemies);
            g->score += enemy_collect_score(&g_enemies);
            if (g->player.lives <= 0) {
                sys_stop_music();
                if (g->score > g->hiscore) {
                    g->hiscore = g->score;
                }
                g->state = RB_STATE_RANKING;
            } else if (stage_finished()) {
                if (g->stage < RB_STAGE_COUNT) {
                    begin_stage(g, (uint16_t)(g->stage + 1u), 0);
                } else {
                    sys_stop_music();
                    if (g->score > g->hiscore) {
                        g->hiscore = g->score;
                    }
                    g->state = RB_STATE_RANKING;
                }
            }
        }
        /* Scaffold only. With the VM running, Escape is the PC-98 bit
         * the script already read this frame. F10 is the quit bit. */
        if (!g_vm && g->state == RB_STATE_PLAY && btn_pressed(g, RB_BTN_START)) {
            sys_stop_music();
            g->state = RB_STATE_TITLE;
        }
        break;

    case RB_STATE_RANKING:
        if (btn_pressed(g, RB_BTN_START) || btn_pressed(g, RB_BTN_A)) {
            g->state = RB_STATE_TITLE;
        }
        break;

    case RB_STATE_QUIT:
    default:
        break;
    }

    g->input.buttons = user_buttons;
    host_menu_commit(g, swallowed);
    live_update(g);
    rank_sync(g);
    if (g_vm) {
        if (vm_frame_shown(g_vm)) {
            g_show_frame = 1;
        }
        sys_pace_set_mode(vm_vsync_count(g_vm),
                          (vm_global_u16(g_vm, 0x72) & 0x4000u) != 0);
    } else {
        g_show_frame = 1;
        sys_pace_set_mode(2, 0);
    }
    g->input_prev = g->input;
    watch_smoke(g);
    if (env_on("RUDEBR_TRACE") && (g->frame == 1 || (g->frame % 120u) == 0)) {
        smoke_trace(g, "frame");
    }
}

void game_render(const RbGame *g)
{
    char line[80];

    if (!g_show_frame) {
        return;
    }
    g_show_frame = 0;
    /* The original SETTINGS screen is up but the host menu has not
     * latched yet (it latches on the next update), or a handoff is
     * still leaving it. Keep the previous picture so the original
     * options page never reaches the screen. */
    if (g_vm && ((script_settings() && !menu_visible()) ||
                 g_handoff != 0 || (g_suspend && !g_suspend_closed))) {
        return;
    }
    host_menu_paint();
    sys_clear(8, 8, 24);

    switch (g->state) {
    case RB_STATE_TITLE:
        if (g_vm) {
            uint32_t *frame = sys_frame_pixels();

            if (frame && (g_card > 0 || g_card_hold)) {
                int done = CARD_FRAMES - g_card;
                int level = 16;

                if (g_card_hold) {
                    level = 0;
                } else if (done < CARD_FADE) {
                    level = done * 16 / CARD_FADE;
                } else if (g_card < CARD_FADE) {
                    level = g_card * 16 / CARD_FADE;
                }
                gfx_intro_card(frame, RB_NATIVE_W, RB_NATIVE_H, level);
                break;
            }
            if (frame) {
                vm_present(g_vm, frame, RB_NATIVE_W, RB_NATIVE_H);
                if (sound_page_visible()) {
                    vu_update();
                    vu_draw(frame);
                }
            }
            break;
        }
        sys_draw_text(200, 140, "RUDE BREAKER", 0xFFFFFFFFu);
        sys_draw_text(180, 180, "PC-98 clean-room C99 port", 0xFFA0A0A0u);
        sys_draw_text(200, 240, "PRESS Z / SPACE", 0xFFFFFF00u);
        sys_draw_text(160, 280, "No assets (scaffold mode)", 0xFFFF8080u);
        break;

    case RB_STATE_PLAY:
        if (g_vm) {
            RbVmInfo info;
            uint32_t *frame = sys_frame_pixels();

            if (frame && vm_gfx_active(g_vm)) {
                vm_present(g_vm, frame, RB_NATIVE_W, RB_NATIVE_H);
            } else {
                RbVmSprite spr[256];
                int n;
                int i;

                n = vm_collect_sprites(g_vm, spr, 256);
                for (i = 0; i < n; i++) {
                    RbSprite draw;

                    memset(&draw, 0, sizeof(draw));
                    draw.x = spr[i].x;
                    draw.y = spr[i].y;
                    draw.w = (uint16_t)(spr[i].w > 0 ? spr[i].w : 1);
                    draw.h = (uint16_t)(spr[i].h > 0 ? spr[i].h : 1);
                    draw.pixels = NULL;
                    draw.solid_rgba = spr[i].rgba;
                    sys_draw_sprite(&draw);
                }
            }
            vm_info(g_vm, &info);
            if (!(frame && vm_gfx_active(g_vm))) {
                snprintf(line, sizeof(line), "SCORE %u  HI %u  LIVES %d",
                         (unsigned)g->score, (unsigned)g->hiscore, (int)info.lives);
                sys_draw_text(8, 8, line, 0xFFFFFFFFu);
                snprintf(line, sizeof(line), "STAGE %u  CELL %u  SPD %d",
                         (unsigned)g->stage, (unsigned)info.cell, (int)info.speed);
                sys_draw_text(8, 24, line, 0xFFC0C0C0u);
            }
        } else {
            player_render(&g->player);
            enemy_pool_render(&g_enemies);
            bullet_pool_render(&g_bullets);
            snprintf(line, sizeof(line), "SCORE %u  HI %u  LIVES %d",
                     (unsigned)g->score, (unsigned)g->hiscore, (int)g->player.lives);
            sys_draw_text(8, 8, line, 0xFFFFFFFFu);
            snprintf(line, sizeof(line), "STAGE %u  CELL %u  SPD %d",
                     (unsigned)g->stage, (unsigned)stage_cell(), (int)g->scroll_speed);
            sys_draw_text(8, 24, line, 0xFFC0C0C0u);
        }
        break;

    case RB_STATE_RANKING:
        sys_draw_text(240, 160, "GAME OVER", 0xFFFF4040u);
        snprintf(line, sizeof(line), "SCORE %u", (unsigned)g->score);
        sys_draw_text(240, 200, line, 0xFFFFFFFFu);
        sys_draw_text(200, 260, "PRESS Z / SPACE", 0xFFFFFF00u);
        break;

    default:
        break;
    }
    if (g_training && g->state == RB_STATE_PLAY) {
        sys_draw_text(8, 8, "TRAINING", 0x40FF40FFu);
    }
    if (menu_visible() || getenv("RUDEBR_SHOT_FRAME")) {
        const char *shot = getenv("RUDEBR_SHOT");
        static int saved;
        uint32_t *px;
        FILE *fp;

        /* RUDEBR_SHOT_FRAME waits until that logic frame. The settings
         * page draws its lower rows only after the cursor has visited
         * them, so a headless shot can land on the finished list. */
        if (shot && shot[0] && !saved) {
            const char *at = getenv("RUDEBR_SHOT_FRAME");
            unsigned long want = at && at[0] ? strtoul(at, NULL, 10) : 0;

            if (want > 0 && g->frame < want) {
                shot = NULL;
            }
        }
        if (shot && shot[0] && !saved) {
            px = sys_frame_pixels();
            if (px) {
                fp = fopen(shot, "wb");
                if (fp) {
                    fwrite(px, 4, (size_t)RB_NATIVE_W * (size_t)RB_NATIVE_H, fp);
                    fclose(fp);
                    fprintf(stderr, "shot %s\n", shot);
                    saved = 1;
                }
            }
        }
    }
}
