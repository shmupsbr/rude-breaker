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
#include "vm.h"

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
/* Host options. Default is the original palette and training off, so
 * the scripted checks keep the same picture and the same deaths.
 * On the settings screen they are cursor rows E0 20 and 21. */
static int          g_modern;
static int          g_training;
static int          g_menu_pending;
/* Set by a VM iteration that flips the page. Frame skip leaves it
 * clear so the picture already on screen stays up. */
static int          g_show_frame = 1;

static void cfg_path(const RbGame *g, char *out, size_t n)
{
    struct stat st;
    char dir[512];
    char *slash;

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

    g_modern = 0;
    g_training = 0;
    cfg_path(g, path, sizeof path);
    fp = fopen(path, "r");
    if (!fp) {
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
        }
    }
    fclose(fp);
}

static uint8_t  g_rank_c9;
static uint16_t g_rank_b2;
static uint16_t g_rank_stock;

static void rank_path(const RbGame *g, char *out, size_t n)
{
    char cfg[600];
    char *slash;

    cfg_path(g, cfg, sizeof cfg);
    slash = strrchr(cfg, '/');
    if (!slash) {
        snprintf(out, n, "%s", "RBRANK.DAT");
        return;
    }
    *slash = 0;
    snprintf(out, n, "%s/RBRANK.DAT", cfg);
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
            return;
        }
    }
    fp = fopen(path, "wb");
    if (!fp) {
        return;
    }
    if (fwrite(fresh, 1, sizeof fresh, fp) == sizeof fresh) {
        rank_snapshot();
    }
    fclose(fp);
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
    if (commit || rank_header_changed()) {
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
    fprintf(fp, "palette=%s\ntraining=%s\n",
            g_modern ? "modern" : "original",
            g_training ? "on" : "off");
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

static int settings_open(void)
{
    /* SETTINGS, the two JIS characters behind that English label. */
    return g_vm && vm_text_has_jis(g_vm, 0x405Fu, 0x446Au);
}

/* Cursor index to the text row it redraws. E0 2 is BACK on row 5.
 * INVINCIBLE (17) skips the blank row 20. */
static int row_for_e0(int e0)
{
    if (e0 >= 2 && e0 <= 16) {
        return e0 + 3;
    }
    if (e0 >= 17 && e0 <= 19) {
        return e0 + 4;
    }
    return -1;
}

static int btn_pressed(const RbGame *g, uint16_t mask);

static void menu_beep(void)
{
    sys_play_sound(0x1D, 180);
}

static void swallow_btn(RbGame *g, uint16_t btn, uint16_t game_bit, uint16_t *sw)
{
    if (g->input.buttons & btn) {
        g->input.buttons = (uint16_t)(g->input.buttons & (uint16_t)~btn);
        *sw = (uint16_t)(*sw | game_bit);
    }
}

/* UP/DOWN move the settings cursor. LEFT/RIGHT change the value.
 * PALETTE and TRAINING share the blank row between FRAME SKIP and
 * INVINCIBLE. TRAINING is the left-hand value. PALETTE keeps the
 * label and value columns the other rows use. Row 24 stays empty. */
static void host_menu_input(RbGame *g, uint16_t *swallowed)
{
    int up = btn_pressed(g, RB_BTN_UP);
    int down = btn_pressed(g, RB_BTN_DOWN);
    int left = btn_pressed(g, RB_BTN_LEFT);
    int right = btn_pressed(g, RB_BTN_RIGHT);
    uint8_t e0;
    int changed = 0;

    *swallowed = 0;
    if (!settings_open()) {
        g_menu_pending = 0;
        return;
    }
    e0 = vm_global_u8(g_vm, 0xE0);
    if (e0 == 20 || e0 == 21) {
        swallow_btn(g, RB_BTN_UP, 0x0001u, swallowed);
        swallow_btn(g, RB_BTN_LEFT, 0x0002u, swallowed);
        swallow_btn(g, RB_BTN_RIGHT, 0x0004u, swallowed);
        swallow_btn(g, RB_BTN_DOWN, 0x0008u, swallowed);
        swallow_btn(g, RB_BTN_A, 0x0010u, swallowed);
        swallow_btn(g, RB_BTN_B, 0x0020u, swallowed);
        if (e0 == 21) {
            if (up) {
                g_menu_pending = 16;
                menu_beep();
            } else if (down) {
                vm_set_global_u8(g_vm, 0xE0, 20);
                menu_beep();
            } else if (left || right) {
                g_training = !g_training;
                changed = 1;
                menu_beep();
            }
        } else if (up) {
            vm_set_global_u8(g_vm, 0xE0, 21);
            menu_beep();
        } else if (down) {
            g_menu_pending = 17;
            menu_beep();
        } else if (left || right) {
            g_modern = !g_modern;
            changed = 1;
            menu_beep();
        }
        if (changed) {
            apply_host_options();
            save_cfg(g);
        }
        return;
    }
    if (e0 == 16 && down) {
        swallow_btn(g, RB_BTN_DOWN, 0x0008u, swallowed);
        g_menu_pending = 21;
        menu_beep();
    } else if (e0 == 17 && up) {
        swallow_btn(g, RB_BTN_UP, 0x0001u, swallowed);
        g_menu_pending = 20;
        menu_beep();
    }
}

static void host_menu_commit(RbGame *g, uint16_t swallowed)
{
    if (g_vm && g_menu_pending && settings_open()) {
        int row = row_for_e0((int)vm_global_u8(g_vm, 0xE0));

        if (row >= 0) {
            vm_text_settle_row(g_vm, row);
        }
        vm_set_global_u8(g_vm, 0xE0, (uint8_t)g_menu_pending);
    }
    g_menu_pending = 0;
    if (g_vm && swallowed) {
        vm_pretend_held(g_vm, swallowed);
    }
    (void)g;
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
    vm_text_write(g_vm, value_col, row, value, value_width, value_attr);
}

static void host_menu_paint(void)
{
    uint8_t e0;

    if (!settings_open()) {
        return;
    }
    e0 = vm_global_u8(g_vm, 0xE0);
    /* Row 20 is the blank line the original leaves between FRAME SKIP
     * and INVINCIBLE. Both host rows share it. Row 24 is wiped so a
     * previous build's TRAINING line cannot sit on the bottom edge. */
    vm_text_write(g_vm, 0, 20, "", 40, 0);
    vm_text_write(g_vm, 0, 24, "", 40, 0);
    paint_option_row(0, 18, 20, "TRAINING",
                     g_training ? "ON" : "OFF", 3,
                     e0 == 21, g_training);
    paint_option_row(26, 50, 20, "PALETTE",
                     g_modern ? "MODERN" : "ORIGINAL", 8,
                     e0 == 20, g_modern);
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
             "SGC.COM, READJS.COM, and RUDEBR.BAT are on the disk and are not read.\n"
             "ym2608_adpcm_rom.bin is optional (8192 bytes, CRC32 23c9e0d8).\n",
             (disk && disk->saw_image) ? "Opened disk image" : "Folder",
             where,
             got(disk ? disk->sgm1 : NULL),
             got(disk ? disk->fplay : NULL),
             got(disk ? disk->song : NULL),
             (disk && disk->rank) ? "found" : "absent");
    sys_message("Rude Breaker — disk files needed", body);
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
                           disk->rhythm_dir) == 0 &&
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
    {
        RbVmInfo info;

        vm_info(g_vm, &info);
        g->hiscore = info.hiscore;
    }
    load_cfg(g);
    apply_host_options();
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

static uint16_t script_buttons(uint32_t frame)
{
    const char *s = getenv("RUDEBR_SCRIPT");
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

void game_update(RbGame *g)
{
    uint16_t user_buttons;
    uint16_t swallowed = 0;

    g->frame++;
    g->input.buttons = (uint16_t)(g->input.buttons | script_buttons(g->frame));
    user_buttons = g->input.buttons;
    host_menu_input(g, &swallowed);
    apply_host_options();

    switch (g->state) {
    case RB_STATE_BOOT:
        g->state = RB_STATE_TITLE;
        break;

    case RB_STATE_TITLE:
        if (g_vm) {
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
    host_menu_paint();
    sys_clear(8, 8, 24);

    switch (g->state) {
    case RB_STATE_TITLE:
        if (g_vm) {
            uint32_t *frame = sys_frame_pixels();

            if (frame) {
                vm_present(g_vm, frame, RB_NATIVE_W, RB_NATIVE_H);
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
    if (settings_open()) {
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
