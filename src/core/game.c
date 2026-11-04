/**
 * Game state machine. Platform-agnostic; talks to hardware only via hal.h.
 */
#include "game.h"
#include "version.h"
#include "diskimg.h"
#include "player.h"
#include "enemy.h"
#include "collision.h"
#include "stage.h"
#include "stage_data.h"
#include "assets.h"
#include "exepath.h"
#include "vm.h"
#include "cfilter.h"
#include "crt.h"
#include "demo.h"
#include "gfx.h"
#include "eq.h"
#include "panels.h"
#include "kraken.h"

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
/* RANK MODE: 0 CLASSIC, 1 ADVANCED (hidden 0..1000 rank). */
static int          g_rank_mode = 1;
/* Side panels for 16:9 windows (host option, saved). */
static int          g_panels_on = 1;
/* ORIENTATION: output turned 0/90/180/270 degrees clockwise (0..3). */
static int          g_orient = 0;
static int          g_bullet_c;     /* 0 ORIGINAL, 1 PINK, 2 LILAC, 3 BLUE, 4 GREEN */
static const char *const k_bullet_name[5] = {"ORIGINAL", "PINK", "LILAC", "BLUE", "GREEN"};
static int          g_cfg_9r;      /* settings file has the pr9r defaults */
/* HITBOX: 0 OFF, then the bullet colours (PINK..GREEN) plus RED and
 * WHITE. Never the colour enemy bullets use (ORIGINAL is the red). */
#define HIT_N 6
static int          g_hitbox;
static const char *const k_hit_name[HIT_N] = {
    "OFF", "PINK", "LILAC", "BLUE", "GREEN", "RED"
};
static const uint32_t k_hit_rgba[HIT_N] = {
    0, 0xFF60D0FFu, 0xD8A0FFFFu, 0x50C8FFFFu, 0x78FFC8FFu, 0xFF3030FFu
};
static const char *const k_hit_cfg[HIT_N] = {
    "off", "pink", "lilac", "blue", "green", "red"
};
static int          g_fullscreen;   /* FULLSCREEN option / F11 */
static int          g_scaling = 1;  /* SCALING: 0 SMOOTH, 1 SHARP, 2 INTEGER, 3 AUTO */
static int          g_aspect43 = 1; /* ASPECT: 1 = 4:3, 0 = 1:1 */
static int          g_ship_aggr;    /* SHIP SPEED: 0 STANDARD, 1 AGGRESSIVE (default) */
static int          g_cfilter;      /* COLOR FILTER, CF_* */
static unsigned     g_crt;          /* CRT effects, 1 << CRT_* */
/* Attract mode. g_demo is the stage being shown (0 = none). While it
 * runs, nothing is saved: no cfg, no RBRANK.DAT, no hi-score. */
static int          g_demo;
static unsigned     g_demo_f;
static int          g_demo_next = 1;
static uint32_t     g_idle;
static int          g_demo_lives = -1;
static int          g_demo_keep[3];
static uint32_t     g_demo_hi;
static int          g_attract_on = 1;  /* ATTRACT MODE option, saved */
/* I-FRAMES (GAMEPLAY, saved): CLASSIC by default, NEW adds: rings from the ship
 * while the invulnerability timer runs. Display only. */
static int          g_aura_on;
/* MASTER VOLUME, host output level in percent (0..100, saved). The
 * game's own music and effect levels are not touched. */
static int          g_mvol = 100;
/* BALANCE (-10 left .. 10 right), OUTPUT PROFILE (EQP_*) and the CUSTOM
 * EQ curve in dB, host output shaping, saved. */
static int          g_bal;
static int          g_eqp;
static int8_t       g_eqc[EQ_BANDS];

static void apply_eq(void)
{
    sys_audio_eq(g_eqp, g_eqc, g_bal);
}

static void eq_reset_all(void)
{
    g_bal = 0;
    g_eqp = EQP_FLAT;
    memset(g_eqc, 0, sizeof g_eqc);
    apply_eq();
}
/* JOYSTICK turned OFF by hand: a pad plugged in later (or found at the
 * next launch) is not switched on by itself. Saved as joystick=off. */
static int          g_joy_off_saved;
/* preset=original in the cfg: apply the ORIGINAL PRESET once at boot. */
static int          g_cfg_preset;
static unsigned     g_rank_seen;       /* frames the boot ranking has been up */
static uint32_t     g_demo_t0;         /* demo start, wall clock ms */
#define DEMO_MS 30000u
/* Pause menu over a paused match: 0 none, 1 RESUME/CONFIG/EXIT list,
 * 2 the config pages opened from it. The text page under it is kept
 * and put back on RESUME. */
static int          g_pmenu;
static int          g_pcur;
static uint16_t     g_pprev;
static uint16_t     g_pad_prev;
static uint8_t      g_ptext[RB_VM_TEXT_SIZE];
static int          g_sndx = 1;         /* SOUND CHANNELS: 0 ORIGINAL, 1 EXTRA */
/* MUSIC CHIP / SFX CHIP: 0 ORIGINAL, 1 SID, 2 GAME BOY, 3 PSG, 4 PC ENGINE,
 * 5 VIRTUAL BOY, 6 ZX SPECTRUM, 7 MSX2. */
#define CHIP_N 9
static int          g_mchip;
static int          g_schip;
static const char *const k_chip_name[CHIP_N] = {"ORIGINAL", "SID", "GAME BOY", "PSG", "PC ENGINE", "VIRTUAL BOY", "ZX SPECTRUM", "MSX2", "MEGA DRIVE"};
static const char *const k_chip_key[CHIP_N] = {"original", "sid", "gameboy", "psg", "pcengine", "virtualboy", "zx", "msx2", "megadrive"};

/* ALTERNATE VERSIONS (CONFIG page; cfg key alternate_versions, old
 * key parallel_universe read and dropped): one machine = filter + music chip + sfx chip.
 * Not stored on its own: the row shows the preset the three settings
 * match, or CUSTOM. */
#define PU_N 9
static const char *const k_pu_name[PU_N] = {"ORIGINAL PC-98", "COMMODORE 64", "GAME BOY",
                                           "GAME GEAR", "PC ENGINE", "VIRTUAL BOY", "ZX SPECTRUM 128K", "MSX2", "MEGA DRIVE"};
static const int k_pu_set[PU_N][2] = {{0, 0}, {3, 1}, {1, 2}, {4, 3}, {5, 4}, {2, 5}, {6, 6}, {7, 7}, {8, 8}};

static int pu_index(void)
{
    int k;

    for (k = 0; k < PU_N; k++) {
        if (g_cfilter == k_pu_set[k][0] && g_mchip == k_pu_set[k][1] && g_schip == k_pu_set[k][1]) {
            return k;
        }
    }
    return -1;
}

static int chip_from_key(const char *s)
{
    int i;
    size_t n;

    for (i = 0; i < CHIP_N; i++) {
        n = strlen(k_chip_key[i]);
        if (strncmp(s, k_chip_key[i], n) == 0 && (s[n] == 0 || s[n] == '\n' || s[n] == '\r')) {
            return i;
        }
    }
    return 0;
}
static int          g_cfg_9zf;      /* settings file has the pr9zf defaults */
static int          g_cfg_101;      /* settings file has the 0.10.1 defaults */
static int          g_cfg_101b;     /* ... and I-FRAMES NEW - BLUE as default */
/* 1CC on record (saved) and whether this run still counts. */
static int          g_onecc;
/* Lives lost this run (NO DEATH box); cleared only by a new game. */
static int          g_run_deaths;
/* The run is always two loops: stage 5 of loop 1 returns to stage 1,
 * and stage 5 of loop 2 goes to the ending (or to Kraken). */
static int          g_loop = 1;     /* current loop, 1 = first pass */
static uint8_t      g_live_b2;      /* the difficulty's own B2 */
static int          g_run_ok;
static int          g_last_clear;
/* Old settings files predate the pr9p defaults (modern palette, ship
 * select off). They are moved over once. */
static int          g_cfg_v9p;
static int          g_menu_latched;
static uint16_t     g_menu_prev = 0xFFFFu;
static int          g_suspend;
static int          g_suspend_closed;
static int          g_page;
static int          g_cursor;
static int          g_confirm;   /* RESTORE DEFAULTS asked once */
static int          g_capture;
static int          g_handoff;
static int          g_bgm_no;
static int          g_se_no;
static int          g_pl_kind;
static int          g_pl;
static int          g_vu_style;   /* 0 LEVEL, 1 SPECTRUM */
static int          g_joy_was;
static int          g_joy_lockout;
static void original_preset(RbGame *g);
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
    g_rank_mode = 1;  /* new installs: ADVANCED */
    g_panels_on = 1;
    g_orient = 0;
    g_bullet_c = 1;
    g_cfg_9r = 0;
    g_onecc = 0;
    g_sndx = 1;
    g_mchip = 0;
    g_schip = 0;
    g_scaling = 1;
    g_aspect43 = 1;
    g_ship_aggr = 1;
    g_cfilter = CF_ORIGINAL;
    g_crt = 0;
    g_cfg_9zf = 0;
    g_cfg_101 = 0;
    g_vu_style = 0;
    g_hitbox = 0;
    g_fullscreen = 0;
    g_cfg_v9p = 0;
    g_aura_on = 3;      /* I-FRAMES NEW - BLUE */
    g_cfg_101b = 0;
    g_mvol = 100;
    g_bal = 0;
    g_eqp = EQP_FLAT;
    memset(g_eqc, 0, sizeof g_eqc);
    g_joy_off_saved = 0;
    g_cfg_preset = 0;
    sys_bind_restore();
    cfg_path(g, path, sizeof path);
    fp = fopen(path, "r");
    if (!fp) {
        g_cfg_v9p = -1;
        return;
    }
    /* A save from before rank_mode= was written plays CLASSIC, as it did. */
    g_rank_mode = 0;
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
        } else if (strncmp(line, "rank_mode=advanced", 18) == 0) {
            g_rank_mode = 1;
        } else if (strncmp(line, "rank_mode=classic", 17) == 0) {
            g_rank_mode = 0;
        } else if (strncmp(line, "panels=tate", 11) == 0) {
            /* Old TATE setting: panels on, turned 90 degrees. */
            g_panels_on = 1;
            g_orient = 1;
        } else if (strncmp(line, "orientation=", 12) == 0) {
            int a = atoi(line + 12);

            g_orient = a == 90 ? 1 : a == 180 ? 2 : a == 270 ? 3 : 0;
        } else if (strncmp(line, "panels=on", 9) == 0) {
            g_panels_on = 1;
        } else if (strncmp(line, "panels=off", 10) == 0) {
            g_panels_on = 0;
        } else if (strncmp(line, "bullets=", 8) == 0) {
            int k;

            for (k = 0; k < 5; k++) {
                const char *n = k_bullet_name[k];
                size_t m = strlen(n);
                char want[16];
                size_t q;

                for (q = 0; q < m && q < sizeof want - 1; q++) {
                    want[q] = n[q] == ' ' ? '-' : (char)(n[q] - 'A' + 'a');
                }
                want[q] = 0;
                if (strncmp(line + 8, want, q) == 0 && (line[8 + q] == '\n' || line[8 + q] == '\r' || !line[8 + q])) {
                    g_bullet_c = k;
                }
            }
        } else if (strncmp(line, "hitbox=", 7) == 0) {
            int k;

            for (k = 0; k < HIT_N; k++) {
                size_t m = strlen(k_hit_cfg[k]);

                if (strncmp(line + 7, k_hit_cfg[k], m) == 0 &&
                    (line[7 + m] == '\n' || line[7 + m] == '\r' || !line[7 + m])) {
                    g_hitbox = k;
                }
            }
            if (strncmp(line + 7, "white", 5) == 0) {
                g_hitbox = 1;   /* WHITE was removed: PINK */
            }
        } else if (strncmp(line, "fullscreen=on", 13) == 0) {
            g_fullscreen = 1;
        } else if (strncmp(line, "fullscreen=off", 14) == 0) {
            g_fullscreen = 0;
        } else if (strncmp(line, "scaling=smooth", 14) == 0) {
            g_scaling = 0;
        } else if (strncmp(line, "scaling=sharp", 13) == 0) {
            g_scaling = 1;
        } else if (strncmp(line, "scaling=integer", 15) == 0) {
            g_scaling = 2;
        } else if (strncmp(line, "scaling=auto", 12) == 0) {
            g_scaling = 3;
        } else if (strncmp(line, "crt_", 4) == 0) {
            int k;

            for (k = 0; k < CRT_COUNT; k++) {
                size_t m = strlen(crt_key(k));

                if (strncmp(line, crt_key(k), m) == 0 && line[m] == '=') {
                    if (strncmp(line + m + 1, "on", 2) == 0) {
                        g_crt |= 1u << k;
                    } else {
                        g_crt &= ~(1u << k);
                    }
                }
            }
        } else if (strncmp(line, "parallel_universe=", 18) == 0 ||
                   strncmp(line, "alternate_versions=", 19) == 0) {
            /* informational: the three settings carry the choice */
        } else if (strncmp(line, "color_filter=", 13) == 0) {
            /* not persisted: every launch starts as the ORIGINAL PC-98 */
        } else if (strncmp(line, "vu=spectrum", 11) == 0) {
            g_vu_style = 1;
        } else if (strncmp(line, "vu=level", 8) == 0) {
            g_vu_style = 0;
        } else if (strncmp(line, "ship_speed=aggressive", 21) == 0) {
            g_ship_aggr = 1;
        } else if (strncmp(line, "ship_speed=standard", 19) == 0) {
            g_ship_aggr = 0;
        } else if (strncmp(line, "aspect=8:5", 10) == 0 ||
                   strncmp(line, "aspect=1:1", 10) == 0) {
            /* 8:5 (16:10, square pixels); 1:1 is the older name. */
            g_aspect43 = 0;
        } else if (strncmp(line, "aspect=4:3", 10) == 0) {
            g_aspect43 = 1;
        } else if (strncmp(line, "sound_channels=extra", 20) == 0) {
            g_sndx = 1;
        } else if (strncmp(line, "sound_channels=original", 23) == 0) {
            g_sndx = 0;
        } else if (strncmp(line, "music_chip=", 11) == 0) {
            /* not persisted */
        } else if (strncmp(line, "sfx_chip=", 9) == 0) {
            /* not persisted */
        } else if (strncmp(line, "defaults101=1", 13) == 0) {
            g_cfg_101 = 1;
        } else if (strncmp(line, "defaults101b=1", 14) == 0) {
            g_cfg_101b = 1;
        } else if (strncmp(line, "defaults9zf=1", 13) == 0) {
            g_cfg_9zf = 1;
        } else if (strncmp(line, "iframes=fire", 12) == 0 ||
                   strncmp(line, "iframes=new", 11) == 0) {
            /* "new" is the 0.10.0 name of FIRE */
            g_aura_on = 1;
        } else if (strncmp(line, "iframes=lime", 12) == 0) {
            g_aura_on = 2;
        } else if (strncmp(line, "iframes=blue", 12) == 0) {
            g_aura_on = 3;
        } else if (strncmp(line, "iframes=classic", 15) == 0) {
            g_aura_on = 0;
        } else if (strncmp(line, "master_volume=", 14) == 0) {
            int v = atoi(line + 14);

            g_mvol = v < 0 ? 0 : v > 100 ? 100 : v;
        } else if (strncmp(line, "balance=", 8) == 0) {
            int v = atoi(line + 8);

            g_bal = v < -EQ_BAL_MAX ? -EQ_BAL_MAX : v > EQ_BAL_MAX ? EQ_BAL_MAX : v;
        } else if (strncmp(line, "output_profile=", 15) == 0) {
            char key[32];
            int k = 0;
            int pv;

            while (k < 31 && line[15 + k] && line[15 + k] != '\n' && line[15 + k] != '\r') {
                key[k] = line[15 + k];
                k++;
            }
            key[k] = 0;
            pv = eq_profile_from_key(key);
            g_eqp = pv < 0 ? EQP_FLAT : pv;
        } else if (strncmp(line, "custom_eq=", 10) == 0) {
            const char *q = line + 10;
            int b;

            for (b = 0; b < EQ_BANDS && *q; b++) {
                int v = atoi(q);

                g_eqc[b] = (int8_t)(v < -EQ_DB_MAX ? -EQ_DB_MAX : v > EQ_DB_MAX ? EQ_DB_MAX : v);
                while (*q && *q != ',') {
                    q++;
                }
                if (*q == ',') {
                    q++;
                }
            }
        } else if (strncmp(line, "joystick=off", 12) == 0) {
            g_joy_off_saved = 1;
        } else if (strncmp(line, "joystick=auto", 13) == 0) {
            g_joy_off_saved = 0;
        } else if (strncmp(line, "preset=original", 15) == 0) {
            g_cfg_preset = 1;
        } else if (strncmp(line, "attract=on", 10) == 0) {
            g_attract_on = 1;
        } else if (strncmp(line, "attract=off", 11) == 0) {
            g_attract_on = 0;
        } else if (strncmp(line, "loop=", 5) == 0) {
            /* Older settings files stored a LOOP option. The game now
             * always runs two loops, so the line is ignored. */
        } else if (strncmp(line, "onecc=yes", 9) == 0) {
            g_onecc = 1;
        } else if (strncmp(line, "defaults9r=1", 12) == 0) {
            g_cfg_9r = 1;
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
/* Rank rises at 1/3 of the base rate times the multiplier
 * 1 + 0.01*shot levels + 0.05*sub levels + 0.05*extends gained this
 * game. g_rank_acc keeps the remainder in 1/300 steps so nothing is
 * lost to rounding. */
static uint32_t g_rank_acc;
static int g_rk_shot, g_rk_sub, g_rk_ext;
static int g_rk_d9 = -1, g_rk_d6, g_rk_d5, g_rk_lives;
/* Rank taken away when the sub-weapon was lost; given back on regain. */
static uint32_t g_rk_subcut;
static int rank_mul_pct(void)
{
    return 100 + 1 * g_rk_shot + 5 * g_rk_sub + 5 * g_rk_ext;
}

static uint16_t flag_word(void);
static void set_flag_word(uint16_t f);
static int speed_index(void);
static void set_speed(int i);

static void title_prewarm(void);
static uint8_t base_c9(void);
static uint8_t base_b2(void);
static int loop_mul(void);
static void suicide_force(int on);

static void live_park(void)
{
    if (!g_vm || !g_live || g_live_parked) {
        return;
    }
    set_speed(g_live_menu_speed);
    vm_set_global_u8(g_vm, 0xC9, g_live_c9);
    if (g_loop > 1) {
        vm_set_global_u8(g_vm, 0xB2, g_live_b2);
    }
    suicide_force(0);
    g_live_parked = 1;
}

static void live_unpark(void)
{
    if (!g_vm || !g_live_parked) {
        return;
    }
    set_speed(g_live_speed);
    vm_set_global_u8(g_vm, 0xC9, base_c9());
    if (g_loop > 1) {
        vm_set_global_u8(g_vm, 0xB2, base_b2());
    }
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

    if (!g || !g_vm || g_training || g_demo) {
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

    if (!g_vm || g_training || g_demo) {
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

    if (g_demo) {
        return;
    }
    cfg_path(g, path, sizeof path);
    fp = fopen(path, "w");
    if (!fp) {
        return;
    }
    fprintf(fp, "bullets=%s\ndefaults9r=1\n", g_bullet_c == 1 ? "pink" : g_bullet_c == 2 ? "lilac"
                                 : g_bullet_c == 3 ? "blue" : g_bullet_c == 4 ? "green" : "original");
    fprintf(fp, "palette=%s\ntraining=%s\nrank=%s\npanels=%s\nonecc=%s\ndefaults=9q\n",
            g_modern ? "modern" : "original",
            g_training ? "on" : "off",
            g_rank_on ? "on" : "off",
            g_panels_on ? "on" : "off",
            g_onecc ? "yes" : "no");
    fprintf(fp, "sound_channels=%s\ndefaults9zf=1\n",
            g_sndx ? "extra" : "original");
    fprintf(fp, "rank_mode=%s\n", g_rank_mode ? "advanced" : "classic");
    fprintf(fp, "attract=%s\n", g_attract_on ? "on" : "off");
    fprintf(fp, "iframes=%s\n", g_aura_on == 1 ? "fire" : g_aura_on == 2 ? "lime" :
                                g_aura_on == 3 ? "blue" : "classic");
    fprintf(fp, "master_volume=%d\n", g_mvol);
    fprintf(fp, "balance=%d\noutput_profile=%s\ncustom_eq=", g_bal, eq_profile_key(g_eqp));
    {
        int b;

        for (b = 0; b < EQ_BANDS; b++) {
            fprintf(fp, "%s%d", b ? "," : "", g_eqc[b]);
        }
    }
    fprintf(fp, "\n");
    fprintf(fp, "joystick=%s\n", g_joy_lockout ? "off" : "auto");
    fprintf(fp, "ship_speed=%s\n", g_ship_aggr ? "aggressive" : "standard");
    fprintf(fp, "vu=%s\n", g_vu_style ? "spectrum" : "level");
    if (g_cfg_101) {
        fprintf(fp, "defaults101=1\n");
    }
    if (g_cfg_101b) {
        fprintf(fp, "defaults101b=1\n");
    }
    fprintf(fp, "scaling=%s\naspect=%s\n",
            g_scaling == 0 ? "smooth" : g_scaling == 1 ? "sharp" : g_scaling == 2 ? "integer" : "auto",
            g_aspect43 ? "4:3" : "8:5");
    fprintf(fp, "orientation=%d\n", g_orient * 90);
    {
        int k;

        for (k = 0; k < CRT_COUNT; k++) {
            fprintf(fp, "%s=%s\n", crt_key(k), (g_crt >> k) & 1u ? "on" : "off");
        }
    }
    fprintf(fp, "hitbox=%s\nfullscreen=%s\n", k_hit_cfg[g_hitbox],
            g_fullscreen ? "on" : "off");
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

/* The hitbox colour that would match the enemy bullets. ORIGINAL
 * bullets are the game's bright red, so RED is the one taken then. */
static int hit_blocked(int h)
{
    if (h >= 1 && h <= 4) {
        return h == g_bullet_c;
    }
    return h == 5 && g_bullet_c == 0;
}

static int hit_step(int h, int dir)
{
    int n;

    for (n = 0; n < HIT_N; n++) {
        h = (h + (dir < 0 ? HIT_N - 1 : 1)) % HIT_N;
        if (!hit_blocked(h)) {
            break;
        }
    }
    return h;
}

/* After the bullet colour changes: move the hitbox off a clash. */
static void hit_fix(void)
{
    if (g_hitbox && hit_blocked(g_hitbox)) {
        g_hitbox = hit_step(g_hitbox, 1);
        if (g_hitbox == 0) {
            g_hitbox = hit_step(0, 1);
        }
    }
}

static void fullscreen_apply(int on)
{
    sys_set_fullscreen(on);
    g_fullscreen = sys_fullscreen();
}

/* HITBOX overlay: the ship's own collision box (the words the hit test
 * reads), drawn as a 1 px outline with a 40 % fill so the sprite under
 * it stays visible. Display only; the hit test is not touched. */
static uint32_t hit_blend(uint32_t d, uint32_t c, int a256)
{
    uint32_t r = 0;
    int sh;

    for (sh = 8; sh <= 24; sh += 8) {
        int dc = (int)((d >> sh) & 0xFFu);
        int cc = (int)((c >> sh) & 0xFFu);

        r |= (uint32_t)(dc + ((cc - dc) * a256) / 256) << sh;
    }
    return r | (d & 0xFFu);
}

static void hitbox_draw(uint32_t *fb)
{
    int x0, y0, x1, y1;
    int x, y;
    uint32_t c;

    if (!fb || !g_vm || !g_hitbox || !vm_ship_box(g_vm, &x0, &y0, &x1, &y1)) {
        return;
    }
    c = k_hit_rgba[g_hitbox];
    for (y = y0; y < y1; y++) {
        for (x = x0; x < x1; x++) {
            uint32_t *d;

            if (x < 0 || y < 0 || x >= RB_NATIVE_W || y >= RB_NATIVE_H) {
                continue;
            }
            d = &fb[(size_t)y * RB_NATIVE_W + (size_t)x];
            /* Closed outline. The sides are 2 px wide: the playfield is
             * shown squeezed horizontally (4:3), which would drop a
             * 1 px column; top and bottom stay 1 px. */
            if (x <= x0 + 1 || x >= x1 - 2 || y == y0 || y == y1 - 1) {
                *d = c;                         /* outline */
            } else {
                *d = hit_blend(*d, c, 102);     /* 40 % fill */
            }
        }
    }
}

static int btn_pressed(const RbGame *g, uint16_t mask);

/* I-FRAMES NEW. While the invulnerability timer (global DF) runs, waves
 * in the game's own invulnerability reds run over the ship's surface,
 * from its centre outward, and stop at the sprite's edge: only opaque
 * sprite pixels of the ship, and of the options while they are joined
 * to it, are recoloured. The band width pulses on two harmonic cycles
 * (8 and 4 frames) and the bands thin out as the timer runs down. Pixel
 * art only: three palette colours, no blending. Drawn before COLOR
 * FILTER and CRT so those treat it like the rest of the picture. */
#define AURA_WX 80
static unsigned g_aura_rf;
static uint8_t g_aura_px[RB_NATIVE_W * RB_NATIVE_H];   /* pixels the aura painted this frame */
static int g_aura_any;
#define AURA_WY 64
static void aura_draw(uint32_t *fb, const RbGame *g)
{
    /* FIRE: the game's own invulnerability reds; LIME and BLUE use the
     * same 4-bit-per-gun steps as the PC-98 palette. dark, mid, crest */
    static const uint32_t pals[3][3] = {
        { 0x994444FFu, 0xE75C58FFu, 0xFFCC77FFu },   /* FIRE */
        { 0x338833FFu, 0x77DD44FFu, 0xDDFF99FFu },   /* LIME */
        { 0x334499FFu, 0x5588EEFFu, 0xBBDDFFFFu },   /* BLUE */
    };
    const uint32_t *pal;
    int el;
    static uint8_t m[(2 * AURA_WX) * (2 * AURA_WY)];
    static int top;
    static int prev;
    const uint8_t *kind;
    int df, x0, y0, x1, y1, cx, cy, x, y, fade, tri8, tri4, bw;
    unsigned t;
    const int W = 2 * AURA_WX, H = 2 * AURA_WY;

    if (g_aura_any) {
        memset(g_aura_px, 0, sizeof g_aura_px);
        g_aura_any = 0;
    }
    if (!fb || !g_vm || g_demo || g->state != RB_STATE_PLAY) {
        prev = 0;
        return;
    }
    df = vm_global_u8(g_vm, 0xDF);
    if (df > prev) {
        top = df;
    }
    prev = df;
    kind = gfx_own_mask();
    if (g_aura_on < 1 || g_aura_on > 3 || df <= 0 || top <= 0 || !kind ||
        !vm_ship_box(g_vm, &x0, &y0, &x1, &y1)) {
        return;
    }
    cx = (x0 + x1) / 2;
    cy = (y0 + y1) / 2;
    /* m == 2: a pixel the player's ship or one of its options drew and
     * that is still on top. Taken from the sprite draw itself, so an
     * enemy or bullet next to (or over) the ship is never painted. */
    for (y = 0; y < H; y++) {
        for (x = 0; x < W; x++) {
            int sx = cx - AURA_WX + x, sy = cy - AURA_WY + y;

            m[y * W + x] = (sx >= 80 && sx < 560 && sy >= 0 && sy < RB_NATIVE_H &&
                            (unsigned)(kind[(size_t)sy * RB_GFX_W + (size_t)sx] - 1) < 2u) ? 2 : 0;
        }
    }
    pal = pals[g_aura_on - 1];
    /* Timed from the i-frame start (DF counts down once a logic frame),
     * so the waves and the flash run in step with the timer itself. */
    el = top - df;
    if (el < 0) {
        el = 0;
    }
    t = (unsigned)el;
    if (++g_aura_rf % 6u == 0u) {
        /* 1 shown frame in 6: the whole ship as a solid silhouette in the
         * mode's crest colour. Counted on drawn frames, not on DF, which
         * can skip values between two pictures. */
        for (y = 0; y < H; y++) {
            for (x = 0; x < W; x++) {
                if (m[y * W + x] == 2) {
                    fb[(size_t)(cy - AURA_WY + y) * RB_NATIVE_W + (size_t)(cx - AURA_WX + x)] = pal[2];
                    g_aura_px[(size_t)(cy - AURA_WY + y) * RB_NATIVE_W + (size_t)(cx - AURA_WX + x)] = 1;
                    g_aura_any = 1;
                }
            }
        }
        return;
    }
    tri8 = (t & 4) ? 4 - (int)(t & 3) : (int)(t & 3);        /* 0..4 */
    tri4 = (t & 2) ? 2 - (int)(t & 1) : (int)(t & 1);        /* 0..2 */
    fade = df * 4 >= top ? 4 : (df * 16 + top - 1) / top;    /* 1..4 */
    bw = (2 + (tri8 + tri4) / 2) * fade / 4;                 /* 0..5 */
    if (bw < 1) {
        bw = 1;
    }
    for (y = 0; y < H; y++) {
        for (x = 0; x < W; x++) {
            int dx, dy, d, ph;

            if (m[y * W + x] != 2) {
                continue;
            }
            dx = (x - AURA_WX) * 5 / 6;    /* round on the 4:3 picture */
            dy = y - AURA_WY;
            d = 0;
            while ((d + 1) * (d + 1) <= dx * dx + dy * dy) {
                d++;
            }
            ph = (int)(((unsigned)(d + 1000) - t * 2u) % 16u);   /* outward */
            if (ph >= bw) {
                continue;
            }
            fb[(size_t)(cy - AURA_WY + y) * RB_NATIVE_W + (size_t)(cx - AURA_WX + x)] =
                pal[ph == 0 && fade >= 3 ? 2 : (ph * 2 < bw ? 1 : 0)];
            g_aura_px[(size_t)(cy - AURA_WY + y) * RB_NATIVE_W + (size_t)(cx - AURA_WX + x)] = 1;
            g_aura_any = 1;
        }
    }
}

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
    PAGE_SOUND,
    PAGE_GFX,
    PAGE_SNDOPT,
    PAGE_ALT,
    PAGE_EQ
};

/* GAMEPLAY / GRAPHICS / SOUND list option ids; the setting code below
 * works on the id, so cfg keys and behaviour do not depend on the page. */
enum {
    OPT_DIFF = 0, OPT_SUICIDE, OPT_SHIP, OPT_SPEED, OPT_PALETTE, OPT_TRAINING,
    OPT_PACING, OPT_RANK, OPT_PANELS, OPT_BULLET, OPT_HITBOX, OPT_LOOP,
    OPT_SNDX, OPT_FULLSCREEN, OPT_SCALING, OPT_ASPECT, OPT_CFILTER, OPT_ORIENT,
    OPT_CRT0, /* + CRT_COUNT effects */
    OPT_ATTRACT = 30,
    OPT_MCHIP = 31, OPT_SCHIP = 32, OPT_PRESET = 33, OPT_CUSTOM = 34,
    OPT_AURA = 35, OPT_MVOL = 36, OPT_BAL = 37, OPT_PROF = 38, OPT_EQ = 39,
    OPT_SNDTEST = 40, OPT_BACK = 41, OPT_PU0 = 50,
    OPT_EQB0 = 60, /* + EQ_BANDS */ OPT_EQRST = 70
};
static const int k_view_game[] = {OPT_DIFF, OPT_RANK, OPT_SUICIDE, OPT_SHIP, OPT_SPEED,
                                  OPT_TRAINING, OPT_ATTRACT, OPT_AURA, OPT_BACK};
static const int k_view_gfx[] = {OPT_FULLSCREEN, OPT_SCALING, OPT_ASPECT, OPT_ORIENT,
                                 OPT_PANELS, OPT_PALETTE, OPT_BULLET, OPT_HITBOX,
                                 OPT_CRT0 + 0, OPT_CRT0 + 1, OPT_CRT0 + 2,
                                 OPT_CRT0 + 3, OPT_CRT0 + 4, OPT_CRT0 + 5, OPT_PACING,
                                 OPT_BACK};
static const int k_view_snd[] = {OPT_SNDX, OPT_MVOL, OPT_BAL, OPT_PROF, OPT_EQ,
                                 OPT_SNDTEST, OPT_BACK};
/* From the pause menu the Sound Test is left out: it would stop the
 * stage music under the paused match. */
static const int k_view_snd_p[] = {OPT_SNDX, OPT_MVOL, OPT_BAL, OPT_PROF, OPT_EQ,
                                   OPT_BACK};
static const int k_view_eq[] = {OPT_EQB0 + 0, OPT_EQB0 + 1, OPT_EQB0 + 2, OPT_EQB0 + 3,
                                OPT_EQB0 + 4, OPT_EQB0 + 5, OPT_EQB0 + 6, OPT_EQB0 + 7,
                                OPT_EQB0 + 8, OPT_EQB0 + 9, OPT_BACK};
/* Row of an option on the SOUND page (where SOUND TEST and CUSTOM EQ
 * return to). */
static int snd_index(int opt);

/* Presets, CUSTOM, and (once CUSTOM is picked) the three single
 * settings. Nothing here is saved: every launch and every finished
 * play session goes back to the ORIGINAL PC-98. */
static int g_custom;
static int k_view_alt[PU_N + 5];

static void alt_reset(void)
{
    g_custom = 0;
    g_cfilter = CF_ORIGINAL;
    g_mchip = g_schip = 0;
    sys_audio_chips(0, 0);
}

static int alt_view(void)
{
    int n = 0, k;

    for (k = 0; k < PU_N; k++) {
        k_view_alt[n++] = OPT_PU0 + k;
    }
    k_view_alt[n++] = OPT_CUSTOM;
    if (g_custom) {
        k_view_alt[n++] = OPT_CFILTER;
        k_view_alt[n++] = OPT_MCHIP;
        k_view_alt[n++] = OPT_SCHIP;
    }
    k_view_alt[n++] = OPT_BACK;
    return n;
}

static const int *opt_view(int page, int *n)
{
    if (page == PAGE_ALT) {
        *n = alt_view();
        return k_view_alt;
    }
    if (page == PAGE_GAME) {
        *n = (int)(sizeof k_view_game / sizeof k_view_game[0]);
        return k_view_game;
    }
    if (page == PAGE_GFX) {
        *n = (int)(sizeof k_view_gfx / sizeof k_view_gfx[0]);
        return k_view_gfx;
    }
    if (page == PAGE_EQ) {
        *n = (int)(sizeof k_view_eq / sizeof k_view_eq[0]);
        return k_view_eq;
    }
    if (page == PAGE_SNDOPT && g_pmenu) {
        *n = (int)(sizeof k_view_snd_p / sizeof k_view_snd_p[0]);
        return k_view_snd_p;
    }
    if (page == PAGE_SNDOPT) {
        *n = (int)(sizeof k_view_snd / sizeof k_view_snd[0]);
        return k_view_snd;
    }
    *n = 0;
    return NULL;
}

static int opt_page(void);
static int opt_id(void);

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

/* SUICIDE BULLETS. Bit 0x0008 set makes the enemy death scripts fire
 * their return bullets (origins 04DB / 058B / 0640 spawn), so set is
 * ON. Earlier builds showed it inverted; the saved bit is unchanged,
 * only the label now matches what the game does. */
static int suicide_on(void)
{
    return (flag_word() & 0x0008u) != 0;
}

/* From loop 2 (not Training) SUICIDE BULLETS is forced ON. The player's
 * own setting is kept in g_sui_user and put back whenever the match is
 * parked (settings export) or left, so the option still decides loop 1
 * and the saved value never changes by itself. */
/* Loop 2+ boss HP: stage bosses +50 % per extra loop, the final boss
 * +100 % per extra loop (loop capped at LOOP_MUL_MAX, HP at 0xFFFF). */
static void boss_hp_update(RbGame *g)
{
    unsigned m = (unsigned)loop_mul();
    unsigned pb = 100u, pf = 100u;

    if (!g_vm) {
        return;
    }
    if (m > 1u && !g_training && !g_demo) {
        pb = 100u + 50u * (m - 1u);   /* stage bosses: +50% per extra loop */
        pf = 100u * m;                /* final boss: +100% per extra loop */
    }
    vm_scale_hp(g_vm, pb, pf, g->stage >= 5, NULL);
}

static int g_sui_forced;
static int g_sui_user;

static void suicide_force(int on)
{
    if (!g_vm) {
        return;
    }
    if (on && !g_sui_forced) {
        g_sui_user = suicide_on();
        g_sui_forced = 1;
        set_flag_word((uint16_t)(flag_word() | 0x0008u));
    } else if (!on && g_sui_forced) {
        g_sui_forced = 0;
        set_flag_word((uint16_t)((flag_word() & (uint16_t)~0x0008u) |
                                 (g_sui_user ? 0x0008u : 0u)));
    }
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
    case PAGE_GAME:
    case PAGE_GFX:
    case PAGE_ALT:
    case PAGE_SNDOPT:
    case PAGE_EQ: {
        int n;

        opt_view(g_page, &n);
        return n;
    }
    case PAGE_CTRL:  return 3;
    case PAGE_KEYS:  return 11;
    case PAGE_SOUND: return 10;
    default:         return g_pmenu ? 5 : 10;
    }
}

static int opt_page(void)
{
    return g_page == PAGE_GAME || g_page == PAGE_GFX || g_page == PAGE_SNDOPT ||
           g_page == PAGE_ALT || g_page == PAGE_EQ;
}

static int snd_index(int opt)
{
    int n, i;
    const int *v = opt_view(PAGE_SNDOPT, &n);

    for (i = 0; i < n; i++) {
        if (v[i] == opt) {
            return i;
        }
    }
    return 0;
}

static int opt_id(void)
{
    int n;
    const int *v = opt_view(g_page, &n);

    return v && g_cursor >= 0 && g_cursor < n ? v[g_cursor] : -1;
}

static void enter_page(int page)
{
    if (g_page == PAGE_SOUND && page != PAGE_SOUND) {
        player_stop();
    }
    g_page = page;
    g_cursor = 0;
    g_capture = 0;
    g_confirm = 0;
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
    if (opt_page()) {
        int opt = opt_id();

        if (opt == 0) {
            set_diff(diff_index() + dir);
            changed = 1;
        } else if (opt == 1) {
            if (g_sui_forced) {
                g_sui_user = !g_sui_user;   /* loop 2+: kept ON for now */
            } else {
                set_flag_word((uint16_t)(flag_word() ^ 0x0008u));
            }
            changed = 1;
        } else if (opt == 2) {
            set_flag_word((uint16_t)(flag_word() ^ 0x0080u));
            changed = 1;
        } else if (opt == 3) {
            g_ship_aggr = !g_ship_aggr;
            changed = 1;
        } else if (opt == 4) {
            g_modern = !g_modern;
            changed = 1;
        } else if (opt == 5) {
            g_training = !g_training;
            changed = 1;
        } else if (opt == 6) {
            /* Host only, saved to rudebreaker.ini, not RBRANK.DAT. */
            sys_pace_set_pref_exact(!sys_pace_pref_exact());
            changed = 1;
        } else if (opt == 7) {
            /* Host only, saved to the settings file. */
            /* OFF -> CLASSIC -> ADVANCED -> OFF. */
            if (!g_rank_on) {
                g_rank_on = 1;
                g_rank_mode = 0;
            } else if (!g_rank_mode) {
                g_rank_mode = 1;
            } else {
                g_rank_on = 0;
                g_rank_mode = 0;
            }
            changed = 1;
        } else if (opt == 8) {
            /* Host only, saved to the settings file. */
            g_panels_on = !g_panels_on;
            changed = 1;
        } else if (opt == 9) {
            /* Host only: recolours enemy bullets, saved to the settings file. */
            g_bullet_c = (g_bullet_c + (dir < 0 ? 4 : 1)) % 5;
            gfx_set_bullet_color(g_bullet_c);
            hit_fix();
            changed = 1;
        } else if (opt == 10) {
            /* Host only: never the bullet colour, saved. */
            g_hitbox = hit_step(g_hitbox, dir);
            changed = 1;
        } else if (opt == 12) {
            /* Host only: EXTRA plays effects on a second chip. Saved. */
            g_sndx = !g_sndx;
            sys_audio_extra(g_sndx);
            changed = 1;
        } else if (opt == 13) {
            /* Host only, saved to the settings file. F11 does the same. */
            fullscreen_apply(!g_fullscreen);
            changed = 1;
        } else if (opt == 14) {
            /* Host only: SMOOTH / SHARP / INTEGER, saved. */
            g_scaling = (g_scaling + (dir < 0 ? 3 : 1)) % 4;
            sys_set_scaling(g_scaling);
            changed = 1;
        } else if (opt == 15) {
            /* Host only: 4:3 screen (PC-98 monitor) or 8:5 (640x400, square pixels). */
            g_aspect43 = !g_aspect43;
            sys_set_aspect(g_aspect43);
            panels_set_aspect(g_aspect43);
            changed = 1;
        } else if (opt == 16) {
            /* Host only: recolours the output frame, saved. */
            g_cfilter = (g_cfilter + (dir < 0 ? CF_COUNT - 1 : 1)) % CF_COUNT;
            changed = 1;
        } else if (opt == 17) {
            /* Host only: 0 / 90 / 180 / 270 degrees clockwise, saved.
             * 90 and 270 use the vertical (TATE) panel layout. */
            g_orient = (g_orient + (dir < 0 ? 3 : 1)) % 4;
            sys_set_orientation(g_orient);
            changed = 1;
        } else if (opt >= OPT_PU0 && opt < OPT_PU0 + PU_N) {
            /* Host only: filter, music chip and sfx chip of one machine. */
            int k = opt - OPT_PU0;

            g_cfilter = k_pu_set[k][0];
            g_mchip = g_schip = k_pu_set[k][1];
            sys_audio_chips(g_mchip, g_schip);
            g_custom = 0;
            changed = 1;
        } else if (opt == OPT_CUSTOM) {
            /* Shows the three single settings, starting from the
             * current ones. */
            g_custom = !g_custom;
            changed = 1;
        } else if (opt == OPT_MCHIP || opt == OPT_SCHIP) {
            /* Host only: plays the music / effects through another
             * chip's voices, saved. */
            int *ch = opt == OPT_MCHIP ? &g_mchip : &g_schip;

            *ch = (*ch + (dir < 0 ? CHIP_N - 1 : 1)) % CHIP_N;
            sys_audio_chips(g_mchip, g_schip);
            changed = 1;
        } else if (opt == OPT_AURA) {
            /* Host only: display overlay, saved. */
            g_aura_on = (g_aura_on + (dir < 0 ? 3 : 1)) % 4;
            changed = 1;
        } else if (opt == OPT_MVOL) {
            /* Host only: output level in 10 % steps, saved. */
            g_mvol += dir < 0 ? -10 : 10;
            g_mvol = g_mvol < 0 ? 0 : g_mvol > 100 ? 100 : g_mvol;
            sys_audio_volume(g_mvol);
            changed = 1;
        } else if (opt == OPT_BAL) {
            /* Host only: software balance, saved. */
            g_bal += dir < 0 ? -1 : 1;
            g_bal = g_bal < -EQ_BAL_MAX ? -EQ_BAL_MAX : g_bal > EQ_BAL_MAX ? EQ_BAL_MAX : g_bal;
            apply_eq();
            changed = 1;
        } else if (opt == OPT_PROF) {
            /* Host only: output EQ curve, saved. */
            g_eqp = (g_eqp + (dir < 0 ? EQP_N - 1 : 1)) % EQP_N;
            apply_eq();
            changed = 1;
        } else if (opt >= OPT_EQB0 && opt < OPT_EQB0 + EQ_BANDS) {
            /* Editing a band picks the CUSTOM curve. */
            int b = opt - OPT_EQB0;
            int v = g_eqc[b] + (dir < 0 ? -1 : 1);

            g_eqc[b] = (int8_t)(v < -EQ_DB_MAX ? -EQ_DB_MAX : v > EQ_DB_MAX ? EQ_DB_MAX : v);
            g_eqp = EQP_CUSTOM;
            apply_eq();
            changed = 1;
        } else if (opt == OPT_ATTRACT) {
            /* Host only: demo play after the ranking, saved. */
            g_attract_on = !g_attract_on;
            changed = 1;
        } else if (opt >= OPT_CRT0 && opt < OPT_CRT0 + CRT_COUNT) {
            /* Host only: playfield CRT effects, each saved. */
            g_crt ^= 1u << (opt - OPT_CRT0);
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
        } else if (g_cursor == 6 || g_cursor == 7) {
            /* Same MUSIC / SFX CHIP as ALTERNATE VERSIONS (not saved,
             * back to ORIGINAL with the rest of that page); the track
             * playing now switches at once. */
            int *ch = g_cursor == 6 ? &g_mchip : &g_schip;

            *ch = (*ch + (dir < 0 ? CHIP_N - 1 : 1)) % CHIP_N;
            sys_audio_chips(g_mchip, g_schip);
            g_custom = 1;
            changed = 1;
        } else if (g_cursor == 8) {
            /* VU: LEVEL (LED bars) or SPECTRUM (analyser line), saved */
            g_vu_style = !g_vu_style;
            changed = 1;
        }
    }
    if (changed) {
        apply_host_options();
        save_cfg(g);
        menu_beep();
    }
}

/* Config opened from the pause menu: its root BACK returns to the
 * pause list, with the match's text page put back under it. */
static void pause_back_to_list(void)
{
    if (g_page == PAGE_SOUND) {
        player_stop();
    }
    g_menu_latched = 0;
    g_page = PAGE_ROOT;
    g_cursor = 0;
    g_capture = 0;
    g_pmenu = 1;
    g_pcur = 0;
    g_pprev = 0xFFFFu;
    vm_text_restore(g_vm, g_ptext);
}

/* RESTORE DEFAULTS (CONFIG): every option back to what a new install
 * has. High scores, the ranking and passwords are not touched; the
 * joystick ON/OFF stays as the connected pad needs it. */
static void restore_defaults(RbGame *g)
{
    g_modern = 1;
    g_training = 0;
    g_rank_on = 1;
    g_rank_mode = 1;
    g_panels_on = 1;
    g_orient = 0;
    g_bullet_c = 1;
    g_sndx = 1;
    g_mchip = 0;
    g_schip = 0;
    g_custom = 0;
    g_scaling = 1;
    g_aspect43 = 1;
    g_ship_aggr = 1;
    g_cfilter = CF_ORIGINAL;
    g_crt = 0;
    g_hitbox = 0;
    g_attract_on = 1;
    g_aura_on = 3;
    g_vu_style = 0;
    g_mvol = 100;
    sys_audio_volume(g_mvol);
    eq_reset_all();
    sys_bind_restore();
    sys_pace_set_pref_exact(0);
    if (g_vm) {
        set_diff(2);
        set_flag_word((uint16_t)((flag_word() & (uint16_t)~0x0008u) | 0x0080u));
        set_speed(1);
    }
    gfx_set_bullet_color(g_bullet_c);
    hit_fix();
    sys_audio_extra(g_sndx);
    sys_audio_chips(g_mchip, g_schip);
    sys_set_scaling(g_scaling);
    sys_set_aspect(g_aspect43);
    sys_set_orientation(g_orient);
    panels_set_aspect(g_aspect43);
    fullscreen_apply(0);
    apply_host_options();
    save_cfg(g);
    rank_flush(g);
}

/* ORIGINAL PRESET (CONFIG, or preset=original in the cfg): the game as
 * on the PC-98 disk, for side-by-side comparison. Turns off what the
 * port adds to play and picture; display options (window, scaling,
 * aspect, orientation, frame pacing), keys, DIFFICULTY, SUICIDE
 * BULLETS, hi-scores, the ranking and passwords stay as they are. */
static void original_preset(RbGame *g)
{
    g_modern = 0;
    g_training = 0;
    g_rank_on = 0;
    g_rank_mode = 0;
    g_panels_on = 0;
    g_bullet_c = 0;
    g_sndx = 0;
    g_ship_aggr = 0;
    g_crt = 0;
    g_hitbox = 0;
    g_aura_on = 0;
    eq_reset_all();
    alt_reset();
    if (g_vm) {
        /* SHIP SELECT ON: the weapon pick scene, as on the disk. */
        set_flag_word((uint16_t)(flag_word() & (uint16_t)~0x0080u));
    }
    gfx_set_bullet_color(g_bullet_c);
    hit_fix();
    sys_audio_extra(g_sndx);
    apply_host_options();
    save_cfg(g);
    rank_flush(g);
}

static void activate_row(RbGame *g)
{
    if (g_page == PAGE_ROOT && g_pmenu && g_cursor >= 4) {
        pause_back_to_list();
        menu_ok();
        return;
    }
    if (g_page == PAGE_ROOT) {
        if (g_cursor == 0) {
            enter_page(PAGE_GAME);
        } else if (g_cursor == 1) {
            enter_page(PAGE_GFX);
        } else if (g_cursor == 2) {
            enter_page(PAGE_SNDOPT);
        } else if (g_cursor == 3) {
            enter_page(g_pmenu ? PAGE_CTRL : PAGE_ALT);
        } else if (g_cursor == 4) {
            enter_page(PAGE_CTRL);
        } else if (g_cursor == 5) {
            handoff(3);
        } else if (g_cursor == 6) {
            handoff(4);
        } else if (g_cursor == 7 || g_cursor == 8) {
            /* ORIGINAL PRESET / RESTORE DEFAULTS: the first press asks,
             * the second does it. */
            if (g_confirm != g_cursor) {
                g_confirm = g_cursor;
            } else {
                g_confirm = 0;
                if (g_cursor == 7) {
                    original_preset(g);
                } else {
                    restore_defaults(g);
                }
            }
        } else {
            handoff(2);
        }
        menu_ok();
        return;
    }
    if (opt_page() && opt_id() == OPT_EQ) {
        enter_page(PAGE_EQ);
        menu_ok();
        return;
    }
    if (opt_page() && opt_id() >= OPT_EQB0 && opt_id() < OPT_EQB0 + EQ_BANDS) {
        /* FIRE on a band puts it back to 0 dB. */
        g_eqc[opt_id() - OPT_EQB0] = 0;
        apply_eq();
        save_cfg(g);
        menu_ok();
        return;
    }
    if (g_page == PAGE_EQ && opt_id() == OPT_BACK) {
        enter_page(PAGE_SNDOPT);
        g_cursor = snd_index(OPT_EQ);
        menu_ok();
        return;
    }
    if (opt_page() && opt_id() == OPT_BACK) {
        int from = g_page;

        enter_page(PAGE_ROOT);
        g_cursor = from == PAGE_GFX ? 1 : from == PAGE_SNDOPT ? 2 : from == PAGE_ALT ? 3 : 0;
        menu_ok();
        return;
    }
    if (g_page == PAGE_ALT && (opt_id() >= OPT_PU0 || opt_id() == OPT_CUSTOM)) {
        change_value(g, 1);
        menu_ok();
        return;
    }
    if (opt_page() && opt_id() == OPT_SNDTEST) {
        enter_page(PAGE_SOUND);
        menu_ok();
        return;
    }
    if (g_page == PAGE_CTRL) {
        if (g_cursor == 1) {
            enter_page(PAGE_KEYS);
            menu_ok();
        } else if (g_cursor == 2) {
            enter_page(PAGE_ROOT);
            g_cursor = g_pmenu ? 3 : 4;
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
        } else if (g_cursor == 9) {
            enter_page(PAGE_SNDOPT);
            g_cursor = snd_index(OPT_SNDTEST);
        } else if (g_cursor == 6 || g_cursor == 7 || g_cursor == 8) {
            change_value(g, 1);
            return;
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
    case PAGE_GFX:
        enter_page(PAGE_ROOT);
        g_cursor = 1;
        break;
    case PAGE_SNDOPT:
        enter_page(PAGE_ROOT);
        g_cursor = 2;
        break;
    case PAGE_ALT:
        enter_page(PAGE_ROOT);
        g_cursor = 3;
        break;
    case PAGE_CTRL:
        enter_page(PAGE_ROOT);
        g_cursor = g_pmenu ? 3 : 4;
        break;
    case PAGE_SOUND:
        enter_page(PAGE_SNDOPT);
        g_cursor = snd_index(OPT_SNDTEST);
        break;
    case PAGE_EQ:
        enter_page(PAGE_SNDOPT);
        g_cursor = snd_index(OPT_EQ);
        break;
    default:
        if (g_pmenu) {
            pause_back_to_list();
        } else {
            handoff(2);
        }
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
    if (up || down || back) {
        g_confirm = 0;
    }
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

static const char *joy_value(void)
{
    if (!sys_joy_present()) {
        return "NONE";
    }
    if (sys_joy_name()[0]) {
        return sys_joy_name();
    }
    return "GAMEPAD";
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

/* GAMEPLAY has 19 rows, so they sit on consecutive lines. */
static int game_row(int index)
{
    return 4 + index;
}

/* Sound Test spectrum, drawn in pixels after present. Like the
 * Distopika player's first layout: white bars, instant attack, about
 * 0.3 s release, plus a peak cap that holds and then falls. */
#define VU_N      16
#define VU_X0     64
#define VU_X1     576
#define VU_TOP    296
#define VU_BOTTOM 376
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

/* 90s micro-system LED look: each bar is a stack of segments, lit from
 * the bottom, green / yellow / red from the PC-98 text palette (DAC
 * steps of 0x11), unlit ones as dark glass of the same hue, and a held
 * peak segment. */
#define VU_COLS  12   /* colour steps, green to red */
#define VU_SEG   16   /* segments per bar: from under the list to the help line */
#define VU_SEG_H 8
#define VU_SEG_G 2
static uint32_t vu_seg_rgb(int k, int lit)
{
    static const uint32_t on[VU_COLS] = {
        0x00EE22FFu, 0x00EE22FFu, 0x22FF22FFu, 0x44FF22FFu, 0x88FF00FFu,
        0xBBFF00FFu, 0xEEEE00FFu, 0xFFDD00FFu, 0xFFAA00FFu, 0xFF7700FFu,
        0xFF4400FFu, 0xFF1100FFu
    };
    uint32_t c = on[k];

    if (!lit) {
        /* about a sixth: the off segment still shows its colour */
        uint32_t r = ((c >> 24) & 0xFFu) / 6u + 0x06u;
        uint32_t g = ((c >> 16) & 0xFFu) / 6u + 0x06u;
        uint32_t b = ((c >> 8) & 0xFFu) / 6u + 0x06u;

        c = (r << 24) | (g << 16) | (b << 8) | 0xFFu;
    }
    return c;
}

/* SPECTRUM: a micro-system analyser screen. Thin frequency bars on a
 * dot grid, a bright line through the band levels (straight segments
 * between band centres) and falling peak dots, all in LCD cyan. */
static void vu_draw_spectrum(uint32_t *fb, int top)
{
    int x0 = VU_X0, x1 = VU_X1 - (VU_X1 - VU_X0) / VU_N / 4;
    int hgt = VU_BOTTOM - top;
    int span = (x1 - x0);
    int x, y, prev_y = -1;

    for (y = top; y <= VU_BOTTOM; y += 10) {
        for (x = x0; x < x1; x += 4) {
            vu_fill(fb, x, y, x + 1, y + 1, 0x1A3344FFu, 255);
        }
    }
    for (x = x0; x < x1; x += 4) {
        /* band position of this column, centres at (i + 0.5) / VU_N */
        float f = (float)(x - x0) * (float)VU_N / (float)span - 0.5f;
        int i = (int)(f < 0.0f ? -1.0f : f);
        float a = f - (float)i;
        float lv, pk;
        int i0 = i < 0 ? 0 : i, i1 = i + 1 >= VU_N ? VU_N - 1 : i + 1;
        int h, hp, yy;

        if (a < 0.0f) {
            a = 0.0f;
        }
        lv = g_vu_lv[i0] + (g_vu_lv[i1] - g_vu_lv[i0]) * a;
        pk = g_vu_pk[i0] + (g_vu_pk[i1] - g_vu_pk[i0]) * a;
        h = (int)(lv * (float)hgt + 0.5f);
        hp = (int)(pk * (float)hgt + 0.5f);
        if (h > 0) {
            vu_fill(fb, x, VU_BOTTOM - h, x + 2, VU_BOTTOM, 0x2299BBFFu, 255);
        }
        yy = VU_BOTTOM - h;
        if (prev_y >= 0) {
            int ya = prev_y < yy ? prev_y : yy, yb = prev_y < yy ? yy : prev_y;

            vu_fill(fb, x - 2, ya, x - 1, yb + 1, 0xAAFFFFFFu, 255);
        }
        vu_fill(fb, x - 1, yy - 1, x + 3, yy + 1, 0xDDFFFFFFu, 255);
        prev_y = yy;
        if (g_pl != 0 && hp > h + 2) {
            vu_fill(fb, x, VU_BOTTOM - hp, x + 2, VU_BOTTOM - hp + 2, 0x66DDFFFFu, 255);
        }
    }
}

static void vu_draw(uint32_t *fb)
{
    int span = (VU_X1 - VU_X0) / VU_N;
    int gap = span / 4;
    int pitch = VU_SEG_H + VU_SEG_G;
    int top = VU_BOTTOM - VU_SEG * pitch + VU_SEG_G;
    int i;

    if (!fb) {
        return;
    }
    /* Dark display window with a thin bezel. */
    vu_fill(fb, VU_X0 - 6, top - 6, VU_X1 + 6 - gap, VU_BOTTOM + 6,
            0x0C0C10FFu, 230);
    vu_fill(fb, VU_X0 - 6, top - 6, VU_X1 + 6 - gap, top - 5, 0x334455FFu, 255);
    vu_fill(fb, VU_X0 - 6, VU_BOTTOM + 5, VU_X1 + 6 - gap, VU_BOTTOM + 6,
            0x334455FFu, 255);
    if (g_vu_style) {
        vu_draw_spectrum(fb, top);
        return;
    }
    for (i = 0; i < VU_N; i++) {
        int x = VU_X0 + i * span;
        int lit = (int)(g_vu_lv[i] * (float)VU_SEG + 0.5f);
        int pk = (int)(g_vu_pk[i] * (float)VU_SEG + 0.5f) - 1;
        int k;

        if (g_vu_lv[i] > 0.0f && lit < 1) {
            lit = 1;
        }
        if (lit > VU_SEG) {
            lit = VU_SEG;
        }
        if (pk >= VU_SEG) {
            pk = VU_SEG - 1;
        }
        for (k = 0; k < VU_SEG; k++) {
            int y1 = VU_BOTTOM - k * pitch;
            int on = k < lit || (g_pl != 0 && k == pk && pk >= lit);

            vu_fill(fb, x, y1 - VU_SEG_H, x + span - gap, y1, vu_seg_rgb(k * VU_COLS / VU_SEG, on), 255);
        }
    }
}

/* CUSTOM EQ sliders: one LED segment per dB from the 0 dB mark, cuts
 * in green, boosts through yellow to red, like the Sound Test meter. */
static void eq_draw(uint32_t *fb)
{
    int b;

    if (!fb) {
        return;
    }
    for (b = 0; b < EQ_BANDS; b++) {
        int row = 4 + b * 2 - 1;
        int y0 = row * 16 + 5;
        int x0 = 400;
        int db = g_eqc[b];
        int k;

        for (k = -EQ_DB_MAX; k <= EQ_DB_MAX; k++) {
            int x = x0 + (k + EQ_DB_MAX) * 8;
            int on = k == 0 || (db < 0 && k >= db && k < 0) || (db > 0 && k <= db && k > 0);
            int seg = k < 0 ? (k + EQ_DB_MAX) * 4 / EQ_DB_MAX : 4 + k * 7 / EQ_DB_MAX;
            uint32_t c;

            if (seg > VU_COLS - 1) {
                seg = VU_COLS - 1;
            }
            c = k == 0 ? (on ? 0xFFFFFFFFu : 0x333333FFu) : vu_seg_rgb(seg, on);
            vu_fill(fb, x, k == 0 ? y0 - 2 : y0, x + 6, k == 0 ? y0 + 8 : y0 + 6, c, 255);
        }
    }
}

static int eq_page_visible(void)
{
    return menu_visible() && g_vm && g_page == PAGE_EQ;
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
    static const char *root_name[10] = {
        "GAMEPLAY", "GRAPHICS", "SOUND", "ALTERNATE VERSIONS", "CONTROLS", "RANKING", "PASSWORD",
        "ORIGINAL PRESET", "RESTORE DEFAULTS", "BACK"
    };
    static const char *key_name[11] = {
        "UP", "DOWN", "LEFT", "RIGHT", "FIRE", "MODE", "SHIFT", "PAUSE",
        "SPEED", "RESTORE DEFAULTS", "BACK"
    };

    if (g_pmenu == 1 && g_vm) {
        static const char *pitem[2] = {"RESUME", "MAIN MENU"};

        /* Dark box in the middle of the playfield, the match's text
         * (HUD) left as it is around it. */
        gfx_set_panel(1, 208, 136, 432, 264);
        for (row = 8; row <= 16; row++) {
            vm_text_write(g_vm, 26, row, "", 14, 0);
        }
        vm_text_write(g_vm, 35, 9, "PAUSE", 0, 0xE1u);
        for (i = 0; i < 2; i++) {
            paint_option_row(34, 0, 11 + i * 2, pitem[i], NULL, 0, i == g_pcur, 0);
        }
        return;
    }
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
    } else if (g_page == PAGE_GFX) {
        title = "GRAPHICS";
    } else if (g_page == PAGE_SNDOPT) {
        title = "SOUND";
    } else if (g_page == PAGE_ALT) {
        title = "ALTERNATE VERSIONS";
    } else if (g_page == PAGE_CTRL) {
        title = "CONTROLS";
    } else if (g_page == PAGE_KEYS) {
        title = "DEFINE KEYS";
    } else if (g_page == PAGE_SOUND) {
        title = "SOUND TEST";
    } else if (g_page == PAGE_EQ) {
        title = "CUSTOM EQ";
    }
    vm_text_write(g_vm, MENU_LABEL_COL, 1, title, 0, 0xE1u);
    if (g_page == PAGE_ROOT && g_pmenu) {
        static const char *proot[5] = {"GAMEPLAY", "GRAPHICS", "SOUND", "CONTROLS", "BACK"};

        for (i = 0; i < 5; i++) {
            paint_line(i, menu_row(4, i), proot[i], NULL, 0);
        }
    } else if (g_page == PAGE_ROOT) {
        for (i = 0; i < 10; i++) {
            int ask = (i == 7 || i == 8) && g_confirm == i;

            paint_line(i, menu_row(4, i), root_name[i],
                       ask ? "PRESS AGAIN" : NULL, ask);
        }
        vm_text_write(g_vm, MENU_VALUE_COL, 1, "VER " RB_VERSION_MENU, MENU_VALUE_W, 0xA1u);
    } else if (opt_page()) {
        int nv;
        const int *v = opt_view(g_page, &nv);
        static const char *k_orient[4] = {"0", "90", "180", "270"};

        for (i = 0; i < nv; i++) {
            int r = game_row(i) + (g_page != PAGE_GFX && g_page != PAGE_ALT ? i : 0) - (g_page == PAGE_EQ);

            switch (v[i]) {
            case 0: paint_line(i, r, "DIFFICULTY", k_diff[diff_index()], diff_index() != 2); break;
            case 1: paint_line(i, r, "SUICIDE BULLETS", suicide_on() ? "ON" : "OFF", suicide_on()); break;
            case 2: paint_line(i, r, "SHIP SELECT", ship_on() ? "ON" : "OFF", ship_on()); break;
            case 3: paint_line(i, r, "SHIP SPEED", g_ship_aggr ? "AGGRESSIVE" : "STANDARD", g_ship_aggr); break;
            case 4: paint_line(i, r, "PALETTE", g_modern ? "MODERN" : "ORIGINAL", g_modern); break;
            case 5: paint_line(i, r, "TRAINING", g_training ? "ON" : "OFF", g_training); break;
            case 6: paint_line(i, r, "FRAME PACING", sys_pace_pref_exact() ? "EXACT" : "SMOOTH", sys_pace_pref_exact()); break;
            case 7: paint_line(i, r, "RANK", !g_rank_on ? "OFF" : g_rank_mode ? "ADVANCED" : "CLASSIC", g_rank_on); break;
            case 8: paint_line(i, r, "SIDE PANELS", g_panels_on ? "ON" : "OFF", g_panels_on); break;
            case 9: paint_line(i, r, "BULLET COLOR", k_bullet_name[g_bullet_c], g_bullet_c != 0); break;
            case 10: paint_line(i, r, "HITBOX", k_hit_name[g_hitbox], g_hitbox != 0); break;
            case 12: paint_line(i, r, "SOUND CHANNELS", g_sndx ? "EXTRA" : "ORIGINAL", g_sndx); break;
            case 13: paint_line(i, r, "FULLSCREEN", g_fullscreen ? "ON" : "OFF", g_fullscreen); break;
            case 14: paint_line(i, r, "SCALING", g_scaling == 0 ? "SMOOTH" : g_scaling == 1 ? "SHARP" : g_scaling == 2 ? "INTEGER" : "AUTO", g_scaling != 3); break;
            case 15: paint_line(i, r, "ASPECT", g_aspect43 ? "4:3" : "8:5", !g_aspect43); break;
            case 16: paint_line(i, r, "COLOR FILTER", cfilter_name(g_cfilter), g_cfilter != CF_ORIGINAL); break;
            case OPT_PU0: case OPT_PU0 + 1: case OPT_PU0 + 2: case OPT_PU0 + 3:
            case OPT_PU0 + 4: case OPT_PU0 + 5: case OPT_PU0 + 6: case OPT_PU0 + 7: case OPT_PU0 + 8:
                paint_line(i, r, k_pu_name[v[i] - OPT_PU0], !g_custom && pu_index() == v[i] - OPT_PU0 ? "ON" : NULL, !g_custom && pu_index() == v[i] - OPT_PU0);
                break;
            case OPT_CUSTOM: paint_line(i, r, "CUSTOM", g_custom || pu_index() < 0 ? "ON" : NULL, g_custom || pu_index() < 0); break;
            case OPT_MCHIP: paint_line(i, r, "MUSIC CHIP", k_chip_name[g_mchip], g_mchip != 0); break;
            case OPT_SCHIP: paint_line(i, r, "SFX CHIP", k_chip_name[g_schip], g_schip != 0); break;
            case OPT_AURA: paint_line(i, r, "I-FRAMES",
                                          g_aura_on == 1 ? "NEW - FIRE" : g_aura_on == 2 ? "NEW - LIME" :
                                          g_aura_on == 3 ? "NEW - BLUE" : "CLASSIC", g_aura_on); break;
            case OPT_MVOL: {
                char vb[8];

                snprintf(vb, sizeof vb, "%d", g_mvol);
                paint_line(i, r, "MASTER VOLUME", vb, g_mvol != 100);
                break;
            }
            case OPT_BAL: {
                char vb[16];

                if (g_bal == 0) {
                    snprintf(vb, sizeof vb, "CENTER");
                } else {
                    snprintf(vb, sizeof vb, "%s %d", g_bal < 0 ? "LEFT" : "RIGHT", g_bal < 0 ? -g_bal : g_bal);
                }
                paint_line(i, r, "BALANCE", vb, g_bal != 0);
                break;
            }
            case OPT_PROF: paint_line(i, r, "OUTPUT PROFILE", eq_profile_name(g_eqp), g_eqp != EQP_FLAT); break;
            case OPT_EQ: paint_line(i, r, "CUSTOM EQ", g_eqp == EQP_CUSTOM ? "IN USE" : NULL, g_eqp == EQP_CUSTOM); break;
            case OPT_EQRST: paint_line(i, r, "FLAT ALL BANDS", NULL, 0); break;
            case OPT_ATTRACT: paint_line(i, r, "ATTRACT MODE", g_attract_on ? "ON" : "OFF", !g_attract_on); break;
            case OPT_ORIENT: paint_line(i, r, "ORIENTATION", k_orient[g_orient & 3], g_orient != 0); break;
            case OPT_SNDTEST: paint_line(i, r, "SOUND TEST", NULL, 0); break;
            case OPT_BACK: paint_line(i, r, "BACK", NULL, 0); break;
            default:
                if (v[i] >= OPT_EQB0 && v[i] < OPT_EQB0 + EQ_BANDS) {
                    char lb[16];
                    char vb[8];
                    int b = v[i] - OPT_EQB0;
                    int db = g_eqc[b];

                    /* the slider is drawn in pixels by eq_draw() */
                    snprintf(lb, sizeof lb, "%s HZ", eq_band_name(b));
                    snprintf(vb, sizeof vb, "%s%d", db > 0 ? "+" : "", db);
                    paint_option_row(MENU_LABEL_COL, MENU_VALUE_COL, r, lb, vb, 3,
                                     i == g_cursor, db != 0);
                } else if (v[i] >= OPT_CRT0 && v[i] < OPT_CRT0 + CRT_COUNT) {
                    int on = (g_crt >> (v[i] - OPT_CRT0)) & 1;

                    paint_line(i, r, crt_name(v[i] - OPT_CRT0), on ? "ON" : "OFF", on);
                }
                break;
            }
        }
        if (g_page == PAGE_ALT) {
        }
    } else if (g_page == PAGE_CTRL) {
        paint_line(0, menu_row(4, 0), "JOYSTICK", joy_flag() ? "ON" : "OFF", joy_flag());
        paint_line(1, menu_row(4, 1), "DEFINE KEYS", NULL, 0);
        paint_line(2, menu_row(4, 2), "BACK", NULL, 0);
        /* The one device in use: this controller, or none. */
        vm_text_write(g_vm, MENU_LABEL_COL, 12, "DEVICE", 0, 0xE1u);
        vm_text_write(g_vm, MENU_VALUE_COL, 12, joy_value(), MENU_VALUE_W,
                      sys_joy_present() && joy_flag() ? 0xC1u : 0xA1u);
        if (!sys_joy_present()) {
            vm_text_write(g_vm, MENU_LABEL_COL, 14, "PLUG IN A PAD ANY TIME",
                          0, 0xA1u);
        } else if (!joy_flag() && g_joy_lockout) {
            vm_text_write(g_vm, MENU_LABEL_COL, 14, "OFF BY HAND  STAYS OFF",
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
        /* compact: one text row each, the meter fills the rest */
        paint_line(0, 3, "SOUND", sound_on() ? "ON" : "OFF",
                   sound_on());
        paint_line(1, 4, "BGM", num, g_pl_kind == 0 && g_pl == 1);
        snprintf(num, sizeof num, "%02d", g_se_no);
        paint_line(2, 5, "SE", num, g_pl_kind == 1 && g_pl == 1);
        paint_line(3, 6, "PLAY", NULL, 0);
        paint_line(4, 7, "PAUSE", NULL, 0);
        paint_line(5, 8, "STOP", NULL, 0);
        /* chips of ALTERNATE VERSIONS, switchable here (not saved) */
        paint_line(6, 9, "MUSIC CHIP", k_chip_name[g_mchip], g_mchip != 0);
        paint_line(7, 10, "SFX CHIP", k_chip_name[g_schip], g_schip != 0);
        paint_line(8, 11, "VU", g_vu_style ? "SPECTRUM" : "LEVEL", g_vu_style);
        paint_line(9, 12, "BACK", NULL, 0);
        if (g_pl == 1) {
            state = "PLAY";
        } else if (g_pl == 2) {
            state = "PAUSE";
        }
        snprintf(track, sizeof track, "%s %02d  %s",
                 g_pl_kind ? "SE" : "BGM",
                 g_pl_kind ? g_se_no : g_bgm_no, state);
                vm_text_write(g_vm, MENU_VALUE_COL, 1, track, MENU_VALUE_W,
                      g_pl == 1 ? 0xC1u : 0xA1u);
    }
    if (g_capture) {
        vm_text_write(g_vm, MENU_LABEL_COL, 24, "ESC OR PAD BACK CANCELS", 0,
                      0xA1u);
    } else {
        vm_text_write(g_vm, MENU_LABEL_COL, 24,
                      "FIRE OK  ESC BACK  LEFT RIGHT SET", 0, 0xA1u);
    }
}

static __attribute__((unused)) int env_on(const char *name)
{
    const char *v = getenv(name);

    return v && v[0] && strcmp(v, "0") != 0;
}

static __attribute__((unused)) int env_is(const char *name, const char *want)
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
    if (!0) {
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
    const char *frames = ((char *)0);

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
            if (0) {
                g_smoke_stop = 1;
                g->state = RB_STATE_QUIT;
            }
        }
        if (!g_saw_clear && vm_global_u8(g_vm, 0xC5) >= 2 && info.chunk >= 3) {
            g_saw_clear = 1;
            fprintf(stderr, "smoke: stage-clear c5=%u chunk=%u frame=%u\n",
                    (unsigned)vm_global_u8(g_vm, 0xC5), (unsigned)info.chunk,
                    (unsigned)g->frame);
            if (0) {
                g_smoke_stop = 1;
                g->state = RB_STATE_QUIT;
            }
        }
        if (!g_saw_over && g->frame > 30 && info.chunk == 1 &&
            info.load_record == 1 && info.load_header == 2) {
            g_saw_over = 1;
            fprintf(stderr, "smoke: game-over frame=%u score=%u\n",
                    (unsigned)g->frame, (unsigned)info.score);
            if (0) {
                g_smoke_stop = 1;
                g->state = RB_STATE_QUIT;
            }
        }
    }
    if (0 && g->frame >= 1 &&
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
    int scaffold = 0;
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
            fprintf(stderr, "rudebreaker: scaffold mode (test hook)\n");
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
    if (((char *)0)) {
        vm_reset_play(g_vm, (uint16_t)atoi(((char *)0)));
    }
    {
        RbVmInfo info;

        vm_info(g_vm, &info);
        g->hiscore = info.hiscore;
    }
    load_cfg(g);
    if (!g_cfg_9r) {
        /* pr9r update: BULLET COLOR defaults to PINK, once. */
        g_bullet_c = 1;
        g_cfg_9r = 1;
        save_cfg(g);
    }
    if (!g_cfg_101) {
        /* 0.10.1 update: SCALING AUTO becomes SHARP (with ASPECT 4:3
         * the side panels then meet the picture), once. */
        if (g_scaling == 3) {
            g_scaling = 1;
        }
        g_cfg_101 = 1;
        save_cfg(g);
    }
    if (!g_cfg_101b) {
        /* 0.10.1 update: I-FRAMES defaults to NEW - BLUE, once (a
         * colour picked by hand stays). */
        if (g_aura_on == 0) {
            g_aura_on = 3;
        }
        g_cfg_101b = 1;
        save_cfg(g);
    }
    if (!g_cfg_9zf) {
        /* Older builds wrote a LOOP line. Two loops are now the game. */
        g_cfg_9zf = 1;
        save_cfg(g);
    }
    gfx_set_bullet_color(g_bullet_c);
    hit_fix();
    sys_audio_extra(g_sndx);
    sys_audio_chips(g_mchip, g_schip);
    sys_audio_volume(g_mvol);
    apply_eq();
    if (g_joy_off_saved) {
        /* JOYSTICK was turned OFF by hand: keep it off. */
        g_joy_lockout = 1;
        set_flag_word((uint16_t)(flag_word() & (uint16_t)~0x0400u));
    }
    sys_set_scaling(g_scaling);
    sys_set_aspect(g_aspect43);
    sys_set_orientation(g_orient);
    panels_set_aspect(g_aspect43);
    /* SHIP SPEED replaced DEFAULT SPEED: every game starts at MID (the
     * in-game SPEED button still cycles LOW / MID / MAX). */
    set_speed(1);
    if (g_fullscreen) {
        fullscreen_apply(1);
    }
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
    if (g_cfg_preset) {
        g_cfg_preset = 0;
        original_preset(g);
    }
    apply_host_options();
    /* Straight to the Compile logo, as in the original. */
    g->state = RB_STATE_TITLE;
    if (0 || ((char *)0)) {
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

    /* test hook latches fire from frame 1. The title prompt looks for
     * a later edge, so drop fire for one frame every 30 while the boot
     * chunk is still up. Real keyboards already produce that edge. */
    if (0) {
        vm_info(g_vm, &info);
        if (info.chunk < 2 && (g->frame % 30u) == 0u) {
            in.buttons = (uint16_t)(in.buttons & (uint16_t)~RB_BTN_A);
        }
    }
    if (0) {
        vm_sustain_ship(g_vm);
    }
    vm_set_input(g_vm, &in);
    {
        int paused = (vm_global_u16(g_vm, 0x72) & 0x8000u) != 0;
        int fair = g_run_ok && !g_training && !g_demo;

        kraken_before(g_vm, g_run_deaths, fair, g_loop, (int)g->stage, paused);
        vm_frame(g_vm);
        kraken_after(g_vm, (int)g->stage, paused);
    }
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
    {
        /* End of a play session (game over, ending, quit to the
         * title): alternate versions go back to the ORIGINAL PC-98. */
        static int was_play;

        if (info.chunk >= 2 && !g_demo) {
            was_play = 1;
        } else if (was_play && info.chunk < 2) {
            was_play = 0;
            alt_reset();
        }
    }
    if (info.chunk >= 2 && info.chunk <= 6) {
        g->stage = (uint16_t)(info.chunk - 1u);
        if (g->state == RB_STATE_TITLE || g->state == RB_STATE_RANKING) {
            g->state = RB_STATE_PLAY;
        }
    }
    if (vm_quit_requested(g_vm)) {
        g->state = RB_STATE_QUIT;
    } else if (vm_restart_requested(g_vm)) {
        /* RETURN on the title menu: back to the Compile logo with the
         * saved settings and scores, like the original's 0x08E6. */
        rank_flush(g);
        vm_reset_boot(g_vm);
        rank_load(g);
        apply_host_options();
        g->state = RB_STATE_TITLE;
    } else if (vm_boot_requested(g_vm)) {
        vm_ack_boot(g_vm);
        if (g->state != RB_STATE_QUIT) {
            g->state = RB_STATE_TITLE;
        }
    }
}

/* test hook=120:DOWN,140:A,200-208:RIGHT
 * One frame, or a range, of extra buttons. Tokens match test hook
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

static __attribute__((unused)) uint16_t script_buttons_env(const char *env, uint32_t frame)
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

/* LOOP: each loop after the first starts one difficulty higher than
 * the one before (capped at VERY HARD). Not in Training. */
static int base_diff(void)
{
    int b = live_diff();

    if (g_loop > 1 && !g_training) {
        b += g_loop - 1;
        if (b > 4) {
            b = 4;
        }
    }
    return b;
}

/* C9 / B2 the game runs with: the chosen difficulty's, or the loop's. */
static uint8_t base_c9(void)
{
    return g_loop > 1 && !g_training ? k_c9[base_diff()] : g_live_c9;
}

static uint8_t base_b2(void)
{
    return g_loop > 1 && !g_training ? k_b2[base_diff()] : g_live_b2;
}

/* Loop multiplier for every rank-driven amount: loop N is xN, capped. */
#define LOOP_MUL_MAX 8
#define LOOP_PCT_MAX 50u
static int loop_mul(void)
{
    if (g_training || g_loop < 2) {
        return 1;
    }
    return g_loop > LOOP_MUL_MAX ? LOOP_MUL_MAX : g_loop;
}

/* Loop 2+: enemy bullet speed by rank inside the loop's interval. Each
 * tenth of the way from the interval's floor to its ceiling adds 5 % per
 * extra loop (loop 2: up to +50 %, loop 3: up to +100 %, ...), on top of
 * the loop's own difficulty step. The VM's bullet speed bonus is one
 * byte and is capped at LOOP_SHOT_MAX (+100 %) in total. */
#define LOOP_SHOT_MAX 100u
static unsigned loop_shot_step(double lo, double hi, double r)
{
    int st;

    if (g_training || g_loop < 2 || hi <= lo) {
        return 0;
    }
    st = (int)(10.0 * (r - lo) / (hi - lo));
    return st < 0 ? 0u : st > 10 ? 10u : (unsigned)st;
}

static unsigned loop_shot_pct(unsigned base, unsigned step)
{
    unsigned v = base + 5u * step * (unsigned)(loop_mul() - 1);

    return v > LOOP_SHOT_MAX ? LOOP_SHOT_MAX : v;
}

/* Rank -> difficulty progression (normal games only). The level in
 * effect, g_eff, starts on the chosen one; rank reaching that level's
 * cap moves it up one level with rank back at the new level's start,
 * and a death drops it back to the chosen level. It is emulated: only
 * the shot-speed copy and the rank cap/start follow g_eff, while C9
 * (bonus tables, the 14D0 script compare, the saved setting) keeps the
 * chosen difficulty. */
static int g_eff = 2;
/* VERY HARD chosen: at the rank cap, enemy bullets gain 1% per 20 s
 * (613 logic frames), up to VH_PCT_MAX. A hit or a death clears it. */
#define VH_PCT_MAX 25u
/* 20 s at the 92/3 Hz logic rate: frames * 3 >= 20 * 92. */
#define VH_PCT_FRAMES3 1840u
static unsigned g_vh_pct, g_vh_t;
static unsigned g_loop_step;     /* loop 2+ bullet speed step, 0..10 */

static int eff_ceiling(void)
{
    static const int k_ceil[5] = { 2, 3, 3, 4, 4 };
    return k_ceil[base_diff()];
}

static void vh_reset(const char *why, unsigned frame)
{
    if (g_vh_pct && 0) {
        fprintf(stderr, "rank vh-reset f=%u %s +%u%% -> 0\n", frame, why, g_vh_pct);
    }
    g_vh_pct = 0;
    g_vh_t = 0;
}

static uint32_t rank_start(int i)
{
    return i >= 3 ? RANK_HALF : 0u;
}

static uint32_t rank_cap(int i)
{
    return i <= 1 ? RANK_HALF : RANK_MAX;
}

static void adv_reset(void);
static int adv_bar(void);
static void live_reset_match(void)
{
    g_eff = base_diff();
    g_live_speed = g_live_menu_speed;
    g_live_dir = g_live_menu_speed >= 2 ? -1 : 1;
    g_rank = rank_start(g_eff);
    g_vh_pct = g_vh_t = 0;
    g_rank_acc = 0;
    g_rk_shot = g_rk_sub = g_rk_ext = 0;
    g_rk_d9 = -1;
    g_rk_subcut = 0;
    adv_reset();
}

/* ---- side panel data ---------------------------------------------- */

#define PT_SECS 7200
static uint8_t  g_pt_rank[PT_SECS];
static int      g_pt_rank_n;
static uint16_t g_pt_death[256];
static int      g_pt_death_n;
static uint16_t g_pt_up[256];
static int      g_pt_up_n;
static int      g_lv_flash;
static uint32_t g_pt_frames;     /* match frames, pause excluded */
static uint32_t g_pt_base;       /* g_pt_frames when this stage's graph began */
static char     g_pt_weapon;
static uint8_t  g_pt_wseg;
static int      g_pt_stage;
static uint16_t g_pt_buttons;
static uint32_t g_pt_foe0;       /* enemy counts at stage start */
static uint32_t g_pt_hit0;
static uint32_t g_pt_fix0;

static void pt_reset(void)
{
    g_pt_rank_n = 0;
    g_pt_death_n = 0;
    g_pt_up_n = 0;
    g_lv_flash = 0;
    g_pt_frames = 0;
    g_pt_base = 0;
    g_pt_weapon = 'V';
    g_pt_wseg = 0x1C;
    g_pt_stage = 0;
}

/* Called each live frame. Time runs at the logic rate of 92/3 Hz. */
static void pt_update(const RbGame *g, int died, uint16_t flags)
{
    g_pt_buttons = g->input.buttons;
    if (flags & 0x0400u) {
        g_pt_buttons = (uint16_t)(g_pt_buttons | g->input.pad_buttons);
    }
    if (g_pt_stage != (int)g->stage) {
        if (((char *)0)) {
            uint32_t seen;
            uint32_t hit;

            vm_foe_counts(g_vm, &seen, &hit);
            fprintf(stderr, "foes: stage %d end at frame %u seen %u hit %u fixed %u / %d\n",
                    g_pt_stage, (unsigned)g->frame, (unsigned)(seen - g_pt_foe0),
                    (unsigned)(hit - g_pt_hit0), (unsigned)(vm_fixed_hits(g_vm) - g_pt_fix0),
                    vm_fixed_total(g_pt_stage == 0 ? 5 : g_pt_stage));
            vm_foe_dump(g_pt_stage);
        }
        g_pt_stage = g->stage;
        /* A new stage starts a fresh RANK graph (rank itself carries on). */
        g_pt_rank_n = 0;
        g_pt_death_n = 0;
        g_pt_up_n = 0;
        g_pt_base = g_pt_frames;
        vm_foe_counts(g_vm, &g_pt_foe0, &g_pt_hit0);
        /* The fifth stage runs with stage number 0. */
        vm_set_foe_stage(g_vm, g->stage == 0 ? 5 : (int)g->stage);
        g_pt_fix0 = vm_fixed_hits(g_vm);
    }
    if (flags & 0x8000u) {
        return;
    }
    g_pt_frames++;
    {
        int sec = (int)((g_pt_frames - g_pt_base) * 3u / 92u);

        while (g_pt_rank_n <= sec && g_pt_rank_n < PT_SECS) {
            g_pt_rank[g_pt_rank_n++] = (uint8_t)(g_rank >> 8);
        }
        if (died && g_pt_death_n < 256) {
            g_pt_death[g_pt_death_n++] = (uint16_t)sec;
        }
    }
}

static int panels_active(const RbGame *g)
{
    return g_vm && g_panels_on && g_live && g->state == RB_STATE_PLAY &&
           !menu_visible() && vm_gfx_active(g_vm) &&
           ((g_orient & 1) || sys_window_wide() || ((char *)0));
}

/* The panel logo and side art come from the game's own title screen.
 * When the title has not been shown (stage select, Training, a quick
 * restart), a second, silent VM runs the boot chunk from SGM1.DAT up to
 * the settled title and that frame is taken instead. Nothing it does
 * reaches the live game: sound and input go through game.c only. */
static void title_prewarm(void)
{
    static int tried;
    static uint32_t buf[RB_NATIVE_W * RB_NATIVE_H];
    RbVm *vm = NULL;
    RbInput in;
    int held = 0;
    int f;

    if (tried || panels_have_title()) {
        return;
    }
    tried = 1;
    if (vm_create(&vm, &g_assets) != 0 || !vm) {
        return;
    }
    vm_reset_boot(vm);
    vm_set_grade(vm, g_modern);
    memset(&in, 0, sizeof in);
    sys_audio_suppress(1);
    for (f = 0; f < 4000; f++) {
        vm_set_input(vm, &in);
        vm_frame(vm);
        if (!vm_frame_shown(vm)) {
            continue;
        }
        vm_present(vm, buf, RB_NATIVE_W, RB_NATIVE_H);
        held = gfx_title_frame() ? held + 1 : 0;
        if (held == 45) {
            panels_capture_title(buf);
            break;
        }
    }
    if (!panels_have_title()) {
        fprintf(stderr, "panels: title art not found in SGM1.DAT\n");
    }
    vm_destroy(vm);
    sys_audio_suppress(0);
}

static void panels_draw(const RbGame *g, uint32_t *frame)
{
    static uint32_t cap[32 * 32];
    uint32_t tile[256];
    RbPanelData d;
    RbVmInfo info;
    int base;
    int t;
    int have = 1;
    uint32_t *wide = sys_wide_pixels();

    if (!wide) {
        sys_set_wide(0);
        return;
    }
    title_prewarm();
    vm_info(g_vm, &info);
    memset(&d, 0, sizeof d);
    d.training = g_training;
    d.rank_on = g_rank_on && !g_training;
    d.rank_now = (int)(g_rank >> 8);
    d.rank_mul = rank_mul_pct();
    d.rank_adv = g_rank_on && g_rank_mode && !g_training;
    d.rank_hist = g_pt_rank;
    d.rank_n = g_pt_rank_n;
    d.deaths = g_pt_death;
    d.death_n = g_pt_death_n;
    d.ups = g_pt_up;
    d.up_n = g_pt_up_n;
    d.level_flash = g_lv_flash;
    d.rank_bar = adv_bar();
    /* Global D8 is the weapon the ship has: 0 VULCAN, 1 MISSILE,
     * 2 LASER (set by the capsule pickup). */
    {
        int w = vm_global_u8(g_vm, 0xD8);

        g_pt_weapon = w == 1 ? 'M' : w == 2 ? 'L' : w == 0 ? 'V' : 0;
    }
    d.weapon = g_pt_weapon;
    /* D5: the ship carries a sub-weapon. A hit while it does takes the
     * sub-weapon (D5 and D6 to 0) instead of a life. */
    d.sub_on = vm_global_u8(g_vm, 0xD5) != 0;
    /* Two separate levels (capsule and power-up natives in the stage
     * chunks): D6 is the V/M/L sub-weapon level 0..2, raised by a
     * capsule of the weapon already held and cleared on death; D9 is
     * the main shot level 0..3, set from the power-up count DE and
     * lowered by one on death. The level is shared by V/M/L: a capsule
     * of another colour switches the weapon and keeps D6. */
    {
        int lv = vm_global_u8(g_vm, 0xD6);
        int sh = vm_global_u8(g_vm, 0xD9);

        d.weapon_lv = (lv > 2 ? 2 : lv) + 1;
        d.shot_lv = (sh > 3 ? 3 : sh) + 1;
        d.shot_items = vm_global_u8(g_vm, 0xDE);
    }
    d.frame = g->frame;
    base = g_pt_weapon == 'L' ? 126 : g_pt_weapon == 'M' ? 124 : 122;
    for (t = 0; t < 4 && have; t++) {
        int idx = base + (t & 1) + (t >> 1) * 16;
        int x;
        int y;

        have = gfx_sprite_rgba(vm_gfx(g_vm), g_pt_wseg, idx, tile);
        for (y = 0; have && y < 16; y++) {
            for (x = 0; x < 16; x++) {
                cap[((t >> 1) * 16 + y) * 32 + (t & 1) * 16 + x] = tile[y * 16 + x];
            }
        }
    }
    d.capsule = have && d.sub_on ? cap : NULL;
    {
        /* Medal: the hidden-spot item (script 0x4DE4), 2x2 cells
         * from 188 on the shared item sheet. */
        static uint32_t medal[32 * 32];
        uint32_t cell[256];
        int k;
        int ok = 1;

        for (k = 0; k < 4 && ok; k++) {
            int r;

            ok = gfx_sprite_rgba(vm_gfx(g_vm), 0x1C, 188 + (k & 1) + (k >> 1) * 16, cell);
            for (r = 0; ok && r < 16; r++) {
                memcpy(medal + ((k >> 1) * 16 + r) * 32 + (k & 1) * 16, cell + r * 16,
                       16 * sizeof cell[0]);
            }
        }
        if (ok) {
            d.medal = medal;
        }
    }
    {
        /* Player ship: seg 0x1A cells 16/17, 32/33, 48/49 (the still
         * frame, 2x3 cells) in the current palette, drawn static. */
        static uint32_t ship[32 * 48];
        static int ship_ok;
        uint32_t cell[256];
        int k;

        static uint32_t tmp[32 * 48];
        int lit = 0;

        /* Re-decoded every frame into tmp; kept only when the palette
         * is fully up (no fade), so the icon never goes dark. */
        for (k = 0; k < 6; k++) {
            int r;

            if (!gfx_sprite_rgba(vm_gfx(g_vm), 0x1A, 16 + (k & 1) + (k >> 1) * 16, cell)) {
                break;
            }
            for (r = 0; r < 16; r++) {
                memcpy(tmp + ((k >> 1) * 16 + r) * 32 + (k & 1) * 16, cell + r * 16,
                       16 * sizeof cell[0]);
            }
            if (k == 5) {
                int q;
                int best = 0;

                for (q = 0; q < 32 * 48; q++) {
                    int v = (int)(tmp[q] >> 24) + (int)((tmp[q] >> 16) & 255) +
                            (int)((tmp[q] >> 8) & 255);

                    if (v > best) best = v;
                }
                lit = best > 600;
            }
        }
        if (lit) {
            memcpy(ship, tmp, sizeof ship);
            ship_ok = 1;
        }
        d.ship = ship_ok ? ship : NULL;
    }
    d.lives = vm_global_u8(g_vm, 0xD2);
    /* Invulnerability: DF counts down once a logic frame. A power-up
     * item adds 16 (to 128 at most), a respawn sets 96, and the
     * capsule pickup sets its own value; the ring's full mark is the
     * value at the latest rise. */
    {
        static int prev_df;
        static int prev_lives = -1;
        static int prev_sub = -1;
        static int top;
        static int respawn;
        int df = vm_global_u8(g_vm, 0xDF);

        if (df > prev_df) {
            top = df;
            /* A hit that only costs the sub-weapon gives 64 too. */
            respawn = (prev_lives >= 0 && d.lives != prev_lives) ||
                      (prev_sub == 1 && !d.sub_on);
        }
        if (df == 0) {
            top = 0;
        }
        prev_df = df;
        prev_lives = d.lives;
        prev_sub = d.sub_on;
        d.iframe = df;
        d.iframe_max = top;
        d.iframe_respawn = respawn;
    }
    d.run_deaths = g_run_deaths;
    d.speed = g_live_speed;
    d.score = info.score;
    d.hiscore = info.hiscore > info.score ? info.hiscore : info.score;
    d.stage = g->stage;
    /* Hit ratio: shootable enemies destroyed out of those that have
     * appeared so far this stage (see foe_scan in vm.c). */
    {
        uint32_t seen;
        uint32_t hit;

        vm_foe_counts(g_vm, &seen, &hit);
        d.foes = (int)(seen - g_pt_foe0);
        d.foes_hit = (int)(hit - g_pt_hit0);
        /* HIT COUNT / RATIO: the stage's fixed enemies destroyed out of
         * its fixed total (a death replays a stretch, so cap it). */
        d.foes_total = vm_fixed_total(g->stage == 0 ? 5 : (int)g->stage);
        if (d.foes_total > 0) {
            d.foes_hit = (int)(vm_fixed_hits(g_vm) - g_pt_fix0);
            if (d.foes_hit > d.foes_total) d.foes_hit = d.foes_total;
        }
    }
    /* Hidden spots counted from the stage's own spawn data; D1 counts
     * the medals taken this stage. */
    d.medal_spots = vm_medal_spots(g_vm, g->stage);
    /* [bp+0xD1] is the original per-stage medal counter: cleared by
     * the stage init (chunk 2 at 0x79BC) and after the clear tally
     * (0x3E55), and raised only by the medal pickup (0x4EA4,
     * `2B D1 06 01`, after the 0x4DE4 spot is revealed). */
    d.medals = vm_global_u8(g_vm, 0xD1);
    d.difficulty = g_live && g_rank_on && !g_training ? g_eff : base_diff();
    d.loop = g_loop;
    d.loop_on = 1;
    d.level_pct = g_live && g_rank_on && !g_training && !g_rank_mode ? (int)g_vh_pct : 0;
    if (((char *)0)) {
        /* Test hook: panel layout of the LEVEL extra. */
        d.level_pct = atoi(((char *)0));
    }
    d.time_cs = g_pt_frames * 300u / 92u;
    d.buttons = g_pt_buttons;
    panels_render(wide, frame, &d);
    if ((g_orient & 1) && sys_tate_pixels()) {
        panels_render_tate(sys_tate_pixels(), wide, frame);
        sys_set_wide(2);
        return;
    }
    sys_set_wide(1);
}

/* ---- RANK MODE ADVANCED ------------------------------------------- */
/* Hidden rank 0..1000. It rises with survival time (faster the longer
 * since the last death), main shot power, sub-weapon pickups, medals,
 * lives in stock and holding fire; a death cuts it by 15-25 % and a
 * lost sub-weapon by 4 %. It never leaves [floor, ceiling] of the
 * chosen difficulty (raised each loop). It drives the enemy shot speed
 * copy (C9 at spawn), an extra bullet-speed percent near the top,
 * RETURN FIRE from 70 % and a score bonus. Fire interval and bullet
 * count are per-script counters in the game data, not touched. Logic runs at 92/3 frames per second. */
#define ADV_FPS (92.0 / 3.0)
static const double k_adv_start[5] = {0, 80, 150, 280, 400};
static const double k_adv_ceil[5] = {400, 550, 800, 900, 1000};
static double   g_ar;
static double   g_ar_floor, g_ar_ceil;
static uint32_t g_ar_t;          /* frames since start / last death */
static int      g_ar_d9 = -1, g_ar_d5, g_ar_d6, g_ar_d1, g_ar_lives;
static int      g_ar_band;
static double   g_ar_score_acc;
static uint32_t g_ar_prev_score;

static int adv_active(void)
{
    return g_rank_on && g_rank_mode && !g_training;
}

static void adv_limits(void)
{
    int d = live_diff();
    int lp = g_loop > 1 ? g_loop - 1 : 0;

    if (d < 0) d = 0;
    if (d > 4) d = 4;
    g_ar_floor = k_adv_start[d] + 100.0 * lp;
    g_ar_ceil = k_adv_ceil[d] + 50.0 * lp;
    if (g_ar_ceil > 1000.0) g_ar_ceil = 1000.0;
    if (g_ar_floor > g_ar_ceil - 100.0) g_ar_floor = g_ar_ceil - 100.0;
}

static void adv_reset(void)
{
    adv_limits();
    g_ar = g_ar_floor;
    g_ar_t = 0;
    g_ar_d9 = -1;
    g_ar_band = -1;
    g_ar_score_acc = 0;
    g_ar_prev_score = 0;
}

static int adv_band(double r)
{
    int b = r < 200 ? 0 : r < 400 ? 1 : r < 600 ? 2 : r < 800 ? 3 : 4;

    return b < base_diff() ? base_diff() : b;
}

/* Panel bar: progress toward the next LEVEL band, 0..1000. The band
 * the chosen DIFFICULTY clamps to starts at the floor; the last band
 * (or a ceiling below the next threshold) ends at the ceiling, and at
 * the ceiling the bar stays full. */
static int adv_bar(void)
{
    int b = adv_band(g_ar);
    double lo = 200.0 * b, hi = b < 4 ? 200.0 * (b + 1) : g_ar_ceil;
    double v;

    if (lo > g_ar_floor && b == adv_band(g_ar_floor)) lo = g_ar_floor;
    if (hi > g_ar_ceil) hi = g_ar_ceil;
    if (g_ar >= g_ar_ceil || hi <= lo) return 1000;
    v = (g_ar - lo) * 1000.0 / (hi - lo);
    return v < 0 ? 0 : v > 1000 ? 1000 : (int)v;
}

static void adv_update(RbGame *g, uint16_t flags, int lives, const RbVmInfo *info)
{
    int d9 = vm_global_u8(g_vm, 0xD9);
    int d5 = vm_global_u8(g_vm, 0xD5);
    int d6 = vm_global_u8(g_vm, 0xD6);
    int d1 = vm_global_u8(g_vm, 0xD1);
    double gain = 0.0;
    double lmul = (double)loop_mul();
    unsigned c9, pct;
    int band;

    adv_limits();
    if ((flags & 0x8000u) == 0u) {
        if (g_ar_d9 < 0) {
            g_ar_d9 = d9; g_ar_d5 = d5; g_ar_d6 = d6; g_ar_d1 = d1;
            g_ar_lives = lives;
            g_ar_prev_score = info->score;
        }
        if (lives < g_ar_lives) {
            double cut = g_ar * (0.15 + 0.10 * g_ar / 1000.0);

            g_ar -= cut;
            g_ar_t = 0;
            if (0) {
                fprintf(stderr, "arank death f=%u cut=%.1f now=%.1f\n", (unsigned)g->frame, cut, g_ar);
            }
        } else {
            double ts = g_ar_t / ADV_FPS;
            double surv = 0.6 * (1.0 + (ts / 90.0 > 2.0 ? 2.0 : ts / 90.0));

            g_ar_t++;
            gain += surv / ADV_FPS;
            if (d9 >= 2) {
                gain += 0.15 * (d9 - 1) / ADV_FPS;
            }
            if (lives > 1) {
                /* Lives in stock, counted up to 5. */
                gain += 0.08 * (lives - 1 > 5 ? 5 : lives - 1) / ADV_FPS;
            }
            if ((g->input.buttons | g->input.pad_buttons) & RB_BTN_A) {
                gain += 0.10 / ADV_FPS;
            }
            if (d9 > g_ar_d9) {
                gain += 12.0 * (d9 - g_ar_d9);
            }
            if (d5 && !g_ar_d5) {
                gain += 8.0 * (d6 + 1);
            } else if (d5 && d6 > g_ar_d6) {
                gain += 8.0 * (d6 - g_ar_d6);
            }
            if (d1 > g_ar_d1) {
                int n = d1 > 10 ? 10 : d1;

                gain += 4.0 * n;   /* the n-th medal of the stage */
            }
            if (!d5 && g_ar_d5) {
                g_ar -= g_ar * 0.04;
            }
            if (g_run_deaths == 0) {
                gain *= 1.10;      /* NO DEATH */
            }
            if (gain >= 1.0 && 0) {
                fprintf(stderr, "arank event f=%u +%.1f d9 %d->%d sub %d/%d->%d/%d medals %d->%d\n",
                        (unsigned)g->frame, gain * lmul, g_ar_d9, d9, g_ar_d5, g_ar_d6, d5, d6,
                        g_ar_d1, d1);
            }
            g_ar += gain * lmul;
        }
        if (g_ar < g_ar_floor) g_ar = g_ar_floor;
        if (g_ar > g_ar_ceil) g_ar = g_ar_ceil;
        /* Score bonus: up to +50 % of what was scored, by rank. */
        if (info->score > g_ar_prev_score && info->score - g_ar_prev_score < 200000u) {
            uint32_t sc;

            g_ar_score_acc += (info->score - g_ar_prev_score) * 0.5 * g_ar / 1000.0;
            if (g_ar_score_acc >= 10.0) {
                uint32_t add = (uint32_t)(g_ar_score_acc / 10.0) * 10u;

                g_ar_score_acc -= add;
                sc = info->score + add;
                vm_set_global_u8(g_vm, 0x8E, (uint8_t)sc);
                vm_set_global_u8(g_vm, 0x8F, (uint8_t)(sc >> 8));
                vm_set_global_u8(g_vm, 0x90, (uint8_t)(sc >> 16));
                vm_set_global_u8(g_vm, 0x91, (uint8_t)(sc >> 24));
                g_ar_prev_score = sc;
            } else {
                g_ar_prev_score = info->score;
            }
        } else {
            g_ar_prev_score = info->score;
        }
        g_ar_d9 = d9; g_ar_d5 = d5; g_ar_d6 = d6; g_ar_d1 = d1;
        g_ar_lives = lives;
        g_live_prev_score = info->score;
        g_live_prev_lives = lives;
        if (g_lv_flash > 0) {
            g_lv_flash--;
        }
    }
    band = adv_band(g_ar);
    if (g_ar_band >= 0 && band > g_ar_band) {
        /* Band up: same as a CLASSIC step-up on the panel (LEVEL flash,
         * green line and up arrow in the RANK graph). */
        g_lv_flash = 61;
        if (g_pt_up_n < 256) {
            g_pt_up[g_pt_up_n++] = (uint16_t)((g_pt_frames - g_pt_base) * 3u / 92u);
        }
    }
    g_ar_band = band;
    g_eff = band;
    g_rank = (uint32_t)(g_ar * 159.0 / 1000.0 * 256.0);
    /* Shot speed copy: 0/200/400/600/1000 -> C9 0/4/7/10/28, never
     * below the chosen difficulty. */
    {
        static const double px[5] = {0, 200, 400, 600, 1000};
        int i = 0;
        double v;

        while (i < 3 && g_ar >= px[i + 1]) i++;
        v = k_c9[i] + (k_c9[i + 1] - k_c9[i]) * (g_ar - px[i]) / (px[i + 1] - px[i]);
        if (v > k_c9[4]) v = k_c9[4];
        c9 = (unsigned)(v + 0.5);
        if (c9 < base_c9()) c9 = base_c9();
    }
    pct = g_ar > 850.0 ? (unsigned)((g_ar - 850.0) * 15.0 / 150.0 * lmul + 0.5) : 0u;
    if (pct > 30u) pct = 30u;
    g_vh_pct = pct;
    g_loop_step = loop_shot_step(g_ar_floor, g_ar_ceil, g_ar);
    vm_set_shot_pct(g_vm, loop_shot_pct(pct, g_loop_step));
    vm_set_return_fire(g_vm, g_ar >= 700.0);
    g_live_c9_now = (uint8_t)c9;
    vm_set_global_u8(g_vm, 0xC9, base_c9());
    vm_set_shot_c9(g_vm, g_live_c9_now != base_c9() ? g_live_c9_now : 0u);
    if (0 && (g->frame % 60u) == 0u) {
        fprintf(stderr, "arank f=%u r=%.1f [%.0f,%.0f] band=%d c9=%u pct=%u rf=%d hits=%u lives=%d d9=%d score=%u\n",
                (unsigned)g->frame, g_ar, g_ar_floor, g_ar_ceil, band, c9, pct,
                g_ar >= 700.0, vm_shot_c9_hits(g_vm), lives, d9,
                (unsigned)info->score);
    }
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
    if (0) {
        vm_set_global_u8(g_vm, 0xCC, 1);
    }
    on = live_match() && (!menu_visible() || g_pmenu);
    vm_info(g_vm, &info);
    lives = vm_global_u8(g_vm, 0xD2);
    if (vm_take_loop(g_vm)) {
        /* Stage 5 cleared with LOOP on: stage 1 of the next loop. The
         * VM already put the new C9 / B2 in before the stage init. */
        if (g_loop < 99) {
            g_loop++;
        }
        if (!g_training) {
            g_eff = base_diff();
            g_rank = rank_start(g_eff);
            g_rank_acc = 0;
            g_rk_subcut = 0;
            vh_reset("loop", (unsigned)g->frame);
            g_lv_flash = 61;
        }
        if (0) {
            fprintf(stderr, "loop %d f=%u base=%d c9=%u b2=%u mul=%d score=%u lives=%d d9=%u d5=%u d6=%u\n",
                    g_loop, (unsigned)g->frame, base_diff(),
                    (unsigned)vm_global_u8(g_vm, 0xC9), (unsigned)vm_global_u8(g_vm, 0xB2),
                    loop_mul(), (unsigned)info.score, lives, (unsigned)vm_global_u8(g_vm, 0xD9),
                    (unsigned)vm_global_u8(g_vm, 0xD5), (unsigned)vm_global_u8(g_vm, 0xD6));
        }
    }
    if (on && !g_live) {
        g_live_menu_speed = speed_index();
        if (info.score == 0u) {
            g_loop = 1;
        }
        if (((char *)0)) {
            /* Test hook: play as if this difficulty (0..4) was chosen. */
            int n = atoi(((char *)0));
            vm_set_global_u8(g_vm, 0xC9, k_c9[n < 0 ? 0 : n > 4 ? 4 : n]);
        }
        if (g_loop <= 1) {
            /* In a later loop the globals hold the loop's values; the
             * chosen ones were kept when the game started. */
            g_live_c9 = vm_global_u8(g_vm, 0xC9);
            g_live_b2 = vm_global_u8(g_vm, 0xB2);
        } else {
            vm_set_global_u8(g_vm, 0xC9, base_c9());
            vm_set_global_u8(g_vm, 0xB2, base_b2());
        }
        g_live_c9_now = base_c9();
        if (info.score == 0u) {
            live_reset_match();
            pt_reset();
            g_run_deaths = 0;
            /* A fight already on screen (debug shortcut on the first
             * stage frame) keeps its medal count and its phase. */
            if (!kraken_active()
#if KRAKEN_DEBUG
                && !kraken_debug_armed()
#endif
            ) {
                kraken_reset();
            }
            /* A run from the start, without Training or stage select. */
            g_run_ok = !g_training && !g_demo && !((char *)0) && g->stage <= 1;
        }
        g_live = 1;
        g_live_prev_score = info.score;
        g_live_prev_lives = lives;
        g_live_prev_btn = 0xFFFFu;
    } else if (!on && g_live) {
        suicide_force(0);
        set_speed(g_live_menu_speed);
        vm_set_global_u8(g_vm, 0xC9, g_live_c9);
        if (g_loop > 1) {
            vm_set_global_u8(g_vm, 0xB2, g_live_b2);
        }
        vm_set_shot_c9(g_vm, 0);
        g_live = 0;
    }
    {
        /* The next loop's difficulty, for the VM to set at the switch. */
        int nb = live_diff() + (g_training ? 0 : g_loop);

        if (nb > 4) {
            nb = 4;
        }
        /* Loop 1 returns to stage 1. Loop 2, and the Kraken fight, end. */
        vm_set_loop(g_vm, g_loop < 2 && !kraken_active(),
                    g_training ? g_live_c9 : k_c9[nb],
                    g_training ? (g_live_b2 ? g_live_b2 : 2u) : k_b2[nb]);
    }
    {
        /* The flag is still up from stage 4 for a few frames after the
         * stage number turns 5, so wait for it to drop first. */
        static int armed;
        int cc = vm_global_u8(g_vm, 0xCC);

        if (g->stage < 5) {
            armed = 0;
        } else if (cc == 0) {
            armed = 1;
        }
        g_last_clear = armed && cc != 0;
    }
    if (g_run_ok && !g_onecc && g_last_clear && !0) {
        g_onecc = 1;
        save_cfg(g);
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

    if (g_training || (flags & 0x0800u) || (flags & 0x0100u) ||
        lives > g_live_prev_lives + 1) {
        /* Training, invincibility, the debug keys, or a continue (the
         * stock comes back in one go; an extend adds one) end the 1CC
         * attempt. */
        g_run_ok = 0;
    }
    /* Stage scripts set 0xCC when a stage is cleared; after stage 5,
     * the last one, the game goes to the ending. */
    if ((flags & 0x8000u) == 0u && lives < g_live_prev_lives && g_run_deaths < 999) {
        g_run_deaths++;
    }
    pt_update(g, (flags & 0x8000u) == 0u && lives < g_live_prev_lives, flags);
    if (((char *)0) && g->frame == 30u) {
        /* Test hook (debug env): preset the ADVANCED rank. */
        g_ar = atof(((char *)0));
    }
    suicide_force(g_live && !g_live_parked && g_loop > 1 && !g_training && !g_demo);
    boss_hp_update(g);
    flags = flag_word();
    if (adv_active()) {
        adv_update(g, flags, lives, &info);
    } else {
    vm_set_return_fire(g_vm, 0);
    if ((flags & 0x8000u) == 0u) {
        const int prog = g_rank_on && !g_training;
        uint32_t cap = rank_cap(prog ? g_eff : base_diff());

        {
            int d9 = vm_global_u8(g_vm, 0xD9);
            int d6 = vm_global_u8(g_vm, 0xD6);
            int d5 = vm_global_u8(g_vm, 0xD5);

            if (g_rk_d9 >= 0) {
                /* Each level actually gained counts; a capsule that
                 * only scores or swaps colour does not. */
                if (d9 > g_rk_d9) {
                    g_rk_shot += d9 - g_rk_d9;
                }
                if (d5 && !g_rk_d5) {
                    g_rk_sub += 1;
                } else if (d5 && d6 > g_rk_d6) {
                    g_rk_sub += d6 - g_rk_d6;
                }
                if (lives == g_rk_lives + 1 && info.score > 0u) {
                    g_rk_ext += 1;
                }
                if (!d5 && g_rk_d5) {
                    /* Sub-weapon lost: rank drops by a third. */
                    uint32_t cut = g_rank / 3u;

                    g_rank -= cut;
                    g_rk_subcut += cut;
                    vh_reset("hit", (unsigned)g->frame);
                    if (0) {
                        fprintf(stderr, "rank sub-lost f=%u cut=%u.%02u now=%u.%02u\n",
                                (unsigned)g->frame, (unsigned)(cut >> 8),
                                (unsigned)((cut & 0xFFu) * 100u / 256u),
                                (unsigned)(g_rank >> 8),
                                (unsigned)((g_rank & 0xFFu) * 100u / 256u));
                    }
                } else if (d5 && !g_rk_d5 && g_rk_subcut) {
                    /* Regained: the same amount comes back (capped below). */
                    g_rank += g_rk_subcut;
                    if (g_rank > cap) {
                        g_rank = cap;
                    }
                    if (0) {
                        fprintf(stderr, "rank sub-back f=%u add=%u.%02u now=%u.%02u\n",
                                (unsigned)g->frame, (unsigned)(g_rk_subcut >> 8),
                                (unsigned)((g_rk_subcut & 0xFFu) * 100u / 256u),
                                (unsigned)(g_rank >> 8),
                                (unsigned)((g_rank & 0xFFu) * 100u / 256u));
                    }
                    g_rk_subcut = 0;
                }
            }
            g_rk_d9 = d9;
            g_rk_d6 = d6;
            g_rk_d5 = d5;
            g_rk_lives = lives;
        }
        if (lives < g_live_prev_lives && prog) {
            /* A death: back to the chosen level and its starting rank. */
            if (0) {
                fprintf(stderr, "rank reset f=%u level %d -> %d rank %u -> %u\n",
                        (unsigned)g->frame, g_eff, base_diff(), (unsigned)(g_rank >> 8),
                        (unsigned)(rank_start(base_diff()) >> 8));
            }
            g_eff = base_diff();
            g_rank = rank_start(g_eff);
            g_rank_acc = 0;
            g_rk_subcut = 0;
            cap = rank_cap(g_eff);
            vh_reset("death", (unsigned)g->frame);
        } else if (lives < g_live_prev_lives) {
            g_rank >>= 1;
        } else {
            uint32_t rise = RANK_PER_FRAME;

            if (info.score != g_live_prev_score) {
                rise += RANK_PER_SCORE;
            }
            if (((char *)0)) {
                /* Test hook: a faster rank climb, to force step-ups. */
                rise *= (uint32_t)atoi(((char *)0));
            }
            rise *= (uint32_t)loop_mul();
            g_rank_acc += rise * (uint32_t)rank_mul_pct();
            g_rank += g_rank_acc / 300u;
            g_rank_acc %= 300u;
        }
        if (g_rank > cap) {
            g_rank = cap;
        }
        if (prog && g_rank >= cap && g_eff < eff_ceiling()) {
            if (0) {
                fprintf(stderr, "rank step-up f=%u level %d -> %d rank %u -> %u\n",
                        (unsigned)g->frame, g_eff, g_eff + 1, (unsigned)(g_rank >> 8),
                        (unsigned)(rank_start(g_eff + 1) >> 8));
            }
            g_eff++;
            g_rank = rank_start(g_eff);
            g_lv_flash = 61;
            /* The game's own 1000 bonus popup, above the ship. */
            {
                int rc = vm_bonus_1000(g_vm, -24);

                if (0) {
                    fprintf(stderr, "rank step-bonus f=%u 1000 %s\n", (unsigned)g->frame,
                            rc == 0 ? "ok" : "skipped");
                }
            }
            if (g_pt_up_n < 256) {
                g_pt_up[g_pt_up_n++] = (uint16_t)((g_pt_frames - g_pt_base) * 3u / 92u);
            }
            g_rank_acc = 0;
            g_rk_subcut = 0;
        }
        if (prog && (base_diff() == 2 || base_diff() == 4) && g_eff == eff_ceiling() && g_rank >= cap) {
            unsigned vmax = VH_PCT_MAX * (unsigned)loop_mul();

            if (vmax > LOOP_PCT_MAX && loop_mul() > 1) {
                vmax = LOOP_PCT_MAX;
            }
            if (g_vh_pct < vmax && ++g_vh_t * 3u >= VH_PCT_FRAMES3) {
                g_vh_t = 0;
                g_vh_pct += (unsigned)loop_mul();
                if (g_vh_pct > vmax) {
                    g_vh_pct = vmax;
                }
                if (0) {
                    fprintf(stderr, "rank vh-up f=%u +%u%%\n", (unsigned)g->frame, g_vh_pct);
                }
            }
        } else if (!prog || g_eff != eff_ceiling()) {
            g_vh_pct = g_vh_t = 0;
        }
        if (g_lv_flash > 0) {
            g_lv_flash--;
        }
        g_live_prev_score = info.score;
        g_live_prev_lives = lives;
    }
    g_loop_step = loop_shot_step((double)rank_start(g_eff), (double)rank_cap(g_eff),
                                 (double)g_rank);
    vm_set_shot_pct(g_vm, loop_shot_pct(g_vh_pct, g_loop_step));
    g_live_c9_now = base_c9();
    if (g_rank_on && !g_training) {
        unsigned add = (unsigned)((g_rank >> 8) * RANK_SPAN + 0x4Fu) / 0x9Fu *
                       (unsigned)loop_mul();
        unsigned base = k_c9[g_eff] > base_c9() ? k_c9[g_eff] : base_c9();
        unsigned c9 = base + add;

        if (c9 > k_c9[4]) {
            c9 = k_c9[4];
        }
        g_live_c9_now = (uint8_t)c9;
    }
    /* C9 stays on the chosen difficulty (the loop's, from loop 2); only
     * shot spawns see rank. */
    vm_set_global_u8(g_vm, 0xC9, base_c9());
    vm_set_shot_c9(g_vm, g_live_c9_now != base_c9() ? g_live_c9_now : 0u);
    }
    gfx_set_hud_speed(g_live_speed + 1);
    if (((char *)0) && g->frame == 900u) {
        /* Test hook: preset the power-up item count once. */
        vm_set_global_u8(g_vm, 0xDE, (uint8_t)atoi(((char *)0)));
    }
    if (0 && (g->frame % 60u) == 0u) {
        fprintf(stderr, "wpn f=%u d5=%u d6=%u d9=%u de=%u w=%c\n", (unsigned)g->frame,
                (unsigned)vm_global_u8(g_vm, 0xD5), (unsigned)vm_global_u8(g_vm, 0xD6),
                (unsigned)vm_global_u8(g_vm, 0xD9), (unsigned)vm_global_u8(g_vm, 0xDE),
                g_pt_weapon ? g_pt_weapon : '-');
        fprintf(stderr, "rank mul=%d lvl=%d f=%u r=%u.%02u shot_c9=%u c9=%u hits=%u spd=%d lives=%d\n",
                rank_mul_pct(), g_eff, (unsigned)g->frame, (unsigned)(g_rank >> 8),
                (unsigned)((g_rank & 0xFFu) * 100u / 256u),
                (unsigned)g_live_c9_now, (unsigned)vm_global_u8(g_vm, 0xC9),
                vm_shot_c9_hits(g_vm),
                g_live_speed, lives);
    }
}

/* ---- attract mode ------------------------------------------------------
 *
 * The original's own attract is the boot script: Compile logo, the
 * intro flight, the title with PUSH TRIGGER and the ranking, in a loop.
 * That loop is kept. After ATTRACT_IDLE frames of it with no input the
 * host plays DEMO_FRAMES (about 30 s) of one stage from its recording,
 * then goes back to the boot script (Compile logo), so the rotation is
 * logo/title, stage 1, logo/title, stage 2, ... stage 5, and again.
 * Any key or button ends a demo and returns to the title sequence. */
/* The demo starts as the boot loop's ranking page goes away (where the
 * Compile logo would come next). ATTRACT_IDLE is only a fallback in case
 * the ranking is never seen: about two full passes with no input. */
#define ATTRACT_IDLE 3600u
#define RANK_MIN     45u     /* ranking up at least this many frames */

static int user_active(const RbGame *g)
{
    return ((g->input.buttons | g->input.pad_buttons) & 0x0FFFu) != 0 || sys_any_press();
}

static void demo_text(int on)
{
    /* Arcade joke: free play, but the cabinet still asks. 1 Hz blink. */
    vm_text_write(g_vm, 29, 12,
                  (on && ((g_demo_f / 15u) & 1u) == 0u) ? "INSERT COIN" : "",
                  11, 0xC1u);
}

static void demo_start(RbGame *g)
{
    g_demo = g_demo_next;
    g_demo_next = g_demo_next % 5 + 1;
    g_demo_f = 0;
    g_demo_lives = -1;
    g_demo_hi = g->hiscore;
    /* The recordings were made with these host rules; the menu values
     * come back from the cfg globals kept here when the demo ends. */
    g_demo_keep[0] = g_training;
    g_demo_keep[1] = g_rank_on;
    g_demo_keep[2] = g_rank_mode;
    g_demo_t0 = sys_ticks_ms();
    g_rank_seen = 0;
    /* No music in the demo, the game's effects only. */
    sys_mute_music(1);
    g_training = 0;
    g_rank_on = 1;
    g_rank_mode = 0;
    apply_host_options();
    begin_stage(g, (uint16_t)g_demo, 1);
    g->score = 0;
    g->state = RB_STATE_PLAY;
}

static void demo_end(RbGame *g)
{
    demo_text(0);
    sys_mute_music(0);
    g_training = g_demo_keep[0];
    g_rank_on = g_demo_keep[1];
    g_rank_mode = g_demo_keep[2];
    /* Same as RETURN on the title menu, without the save: the boot
     * script restarts and RBRANK.DAT (scores and settings) is read back. */
    vm_reset_boot(g_vm);
    rank_load(g);
    apply_host_options();
    g->hiscore = g_demo_hi;
    g->score = 0;
    g->state = RB_STATE_TITLE;
    g_idle = 0;
    g_demo = 0;
}

/* Pause. The match's own pause is the PC-98 Escape bit (flags 0x8000),
 * from the keyboard PAUSE key. Pad START (SDL controller mapping, or
 * button 7 / 9 / a learned one on plain joysticks) presses the same
 * key. While the bit is set the host shows RESUME / CONFIG / EXIT. */
static void pause_exit(RbGame *g)
{
    suicide_force(0);
    g_pmenu = 0;
    g_menu_latched = 0;
    set_flag_word((uint16_t)(flag_word() & 0x7FFFu));
    sys_stop_music();
    rank_flush(g);
    vm_reset_boot(g_vm);
    rank_load(g);
    apply_host_options();
    g->score = 0;
    g->state = RB_STATE_TITLE;
    g_idle = 0;
    g_rank_seen = 0;
}

static void pause_input(RbGame *g)
{
    uint16_t kb = g->input.buttons;
    uint16_t pd = g->input.pad_buttons;
    uint16_t pad_edge = (uint16_t)(pd & (uint16_t)~g_pad_prev);
    int paused;

    g_pad_prev = pd;
    if (!g_vm || g_demo || g->state != RB_STATE_PLAY) {
        g_pmenu = 0;
        return;
    }
    paused = (flag_word() & 0x8000u) != 0;
    if (!paused) {
        if (g_pmenu) {
            g_pmenu = 0;
            g_menu_latched = 0;
        }
        if ((pad_edge & RB_BTN_START) && !menu_visible()) {
            /* Pad START = the PAUSE key for this frame's edge. */
            g->input.buttons = (uint16_t)(g->input.buttons | RB_BTN_START);
        }
        return;
    }
    if (g_pmenu == 0) {
        vm_text_save(g_vm, g_ptext);
        g_pmenu = 1;
        g_pcur = 0;
        g_pprev = 0xFFFFu;
    }
    if (g_pmenu == 1) {
        uint16_t now = (uint16_t)((kb | pd) & (RB_BTN_UP | RB_BTN_DOWN));
        uint16_t edge;

        if ((kb & (RB_BTN_A | RB_BTN_B)) || (pd & RB_BTN_A)) {
            now = (uint16_t)(now | 0x1000u);
        }
        if ((kb & RB_BTN_START) || (pd & (RB_BTN_START | RB_BTN_B))) {
            now = (uint16_t)(now | 0x2000u);
        }
        edge = (uint16_t)(now & (uint16_t)~g_pprev);
        g_pprev = now;
        if (edge & RB_BTN_UP) {
            g_pcur = (g_pcur + 1) % 2;
            menu_beep();
        } else if (edge & RB_BTN_DOWN) {
            g_pcur = (g_pcur + 1) % 2;
            menu_beep();
        } else if ((edge & 0x2000u) || ((edge & 0x1000u) && g_pcur == 0)) {
            /* RESUME (also START / Esc / pad B). */
            vm_text_restore(g_vm, g_ptext);
            set_flag_word((uint16_t)(flag_word() & 0x7FFFu));
            g_pmenu = 0;
            menu_ok();
        } else if ((edge & 0x1000u) && g_pcur == 1) {
            menu_ok();
            pause_exit(g);
        }
    }
    if (g->state == RB_STATE_PLAY && g_pmenu != 2) {
        /* Nothing reaches the paused match but the quit bit. */
        g->input.buttons = (uint16_t)(g->input.buttons & RB_BTN_QUIT);
        g->input.pad_buttons = 0;
    }
}

/* Ship movement scale, applied on top of what the game moved this frame
 * (sub-pixel remainders kept in 1/100 px). SHIP SPEED AGGRESSIVE: LOW
 * x0.85, MID x1, MAX x1.15. ASPECT 1:1: vertical x1.2, so the ship
 * crosses the square-pixel playfield in the same time as on 4:3. Never
 * in the attract demo, which replays the original moves. */
#define SHIP_MIN_X 96
#define SHIP_MAX_X 544
#define SHIP_MIN_Y 56
#define SHIP_MAX_Y 360
static int g_sp_have, g_sp_px, g_sp_py, g_sp_ax, g_sp_ay;

static int sp_axis(int d, int held, int mul, int *acc)
{
    int e;

    if (d == 0 || !held || mul == 100 || d > 16 || d < -16) {
        *acc = 0;
        return 0;
    }
    *acc += d * (mul - 100);
    e = *acc / 100;
    *acc -= e * 100;
    return e;
}

static void ship_move_adjust(RbGame *g)
{
    int x, y, ex, ey, mx = 100, my = 100;
    uint16_t btn;

    if (!g_vm || !g_live || g_live_parked || g_demo ||
        (flag_word() & 0x8000u) || !vm_ship_pos(g_vm, &x, &y)) {
        g_sp_have = 0;
        g_sp_ax = g_sp_ay = 0;
        return;
    }
    if (!g_sp_have) {
        g_sp_have = 1;
        g_sp_px = x;
        g_sp_py = y;
        return;
    }
    if (g_ship_aggr) {
        int sp = speed_index();

        mx = my = sp == 0 ? 85 : sp == 2 ? 115 : 100;
    }
    if (!g_aspect43) {
        my = my * 120 / 100;
    }
    btn = g->input.buttons;
    if (flag_word() & 0x0400u) {
        btn = (uint16_t)(btn | g->input.pad_buttons);
    }
    ex = sp_axis(x - g_sp_px, (btn & (RB_BTN_LEFT | RB_BTN_RIGHT)) != 0, mx, &g_sp_ax);
    ey = sp_axis(y - g_sp_py, (btn & (RB_BTN_UP | RB_BTN_DOWN)) != 0, my, &g_sp_ay);
    /* Stay inside the box the game itself keeps the ship in. */
    if (x + ex < SHIP_MIN_X) ex = SHIP_MIN_X - x;
    if (x + ex > SHIP_MAX_X) ex = SHIP_MAX_X - x;
    if (y + ey < SHIP_MIN_Y) ey = SHIP_MIN_Y - y;
    if (y + ey > SHIP_MAX_Y) ey = SHIP_MAX_Y - y;
    /* Never turn a move around (a slowdown only shortens it). */
    if ((x - g_sp_px > 0 && x - g_sp_px + ex < 0) || (x - g_sp_px < 0 && x - g_sp_px + ex > 0)) ex = 0;
    if ((y - g_sp_py > 0 && y - g_sp_py + ey < 0) || (y - g_sp_py < 0 && y - g_sp_py + ey > 0)) ey = 0;
    vm_ship_shift(g_vm, ex, ey);
    g_sp_px = x + ex;
    g_sp_py = y + ey;
}

void game_update(RbGame *g)
{
    uint16_t user_buttons;
    uint16_t swallowed = 0;

    g->frame++;
    if (sys_take_fullscreen_key()) {
        /* F11 any time: same as the FULLSCREEN option, and saved. */
        if (g_demo) {
            demo_end(g);
        }
        fullscreen_apply(!g_fullscreen);
        save_cfg(g);
    }
    g->input.buttons = (uint16_t)(g->input.buttons |
                                  0);
    /* Same syntax, pressed on the gamepad side (pad_buttons). */
    g->input.pad_buttons = (uint16_t)(g->input.pad_buttons |
                                      0);
    if (g_demo && (user_active(g) || g_demo_f >= DEMO_FRAMES ||
                   sys_ticks_ms() - g_demo_t0 >= DEMO_MS ||
                   g->state != RB_STATE_PLAY)) {
        /* The key that ends the demo is held into the boot script,
         * which takes it as the skip past the Compile logo. */
        demo_end(g);
    }
    user_buttons = g->input.buttons;
    gfx_set_clock(g->frame);
    pause_input(g);
    host_menu_input(g, &swallowed);
    if (g_demo) {
        g->input.buttons = demo_buttons(g_demo, g_demo_f);
        g->input.pad_buttons = 0;
    }
    apply_host_options();
#if KRAKEN_DEBUG
    /* F9: the debug build jumps into the Kraken fight. Release builds
     * do not compile this, and the key is not read. One stage entry
     * only; resetting every frame left a black screen with the HUD. */
    {
        static int dbg_stage;

        if ((user_buttons & RB_BTN_BOSS) && !(g->input_prev.buttons & RB_BTN_BOSS)) {
            kraken_debug_arm();
            dbg_stage = 0;
        }
        if (kraken_debug_armed() && !kraken_active() && g_vm && !dbg_stage) {
            RbVmInfo kinfo;

            vm_info(g_vm, &kinfo);
            if (kinfo.chunk < 2 || kinfo.chunk > 6) {
                /* Stage 5 so the fight has that starfield behind it.
                 * Enter once; resetting every frame left a black HUD. */
                begin_stage(g, 5, 1);
                g->state = RB_STATE_PLAY;
                g_loop = 2;
                dbg_stage = 1;
            }
        }
    }
#endif

    switch (g->state) {
    case RB_STATE_BOOT:
        g->state = RB_STATE_TITLE;
        break;

    case RB_STATE_TITLE:
        if (g_vm) {
            int busy = user_active(g) || menu_visible() || script_settings();

            g_idle = busy ? 0 : g_idle + 1;
            if (busy) {
                g_rank_seen = 0;
            } else if (gfx_rank_frame()) {
                g_rank_seen++;
            } else if (g_rank_seen >= RANK_MIN && g_attract_on) {
                /* Ranking just ended: demo instead of the Compile logo. */
                demo_start(g);
                break;
            } else {
                g_rank_seen = 0;
            }
            if (g_attract_on && g_idle >= ATTRACT_IDLE) {
                /* The demo's first scripted frame is the next update. */
                demo_start(g);
                break;
            }
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
             * its slot is still ahead of the parent. test hook is a
             * smoke-test switch; the frame loop itself never calls it.
             * Lives and the escape bit stay inside the script. */
            step_script(g);
            if (g_demo) {
                int lives = vm_global_u8(g_vm, 0xD2);

                if (g_demo_f == 60u) {
                    g_demo_lives = lives;
                }
                /* A demo that loses a life ends there. */
                if (g_demo_lives >= 0 && lives < g_demo_lives) {
                    g_demo_f = DEMO_FRAMES;
                }
                demo_text(1);
                g_demo_f++;
            }
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
    ship_move_adjust(g);
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
    if (0 && (g->frame == 1 || (g->frame % 120u) == 0)) {
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

            if (frame) {
                vm_present(g_vm, frame, RB_NATIVE_W, RB_NATIVE_H);
                {
                    /* Logo and side art for the panels: the title once it
                     * has settled and faded in. */
                    static int held;

                    held = gfx_title_frame() && !menu_visible() ? held + 1 : 0;
                    if (held == 45) {
                        panels_capture_title(frame);
                    }
                }
                if (sound_page_visible()) {
                    vu_update();
                    vu_draw(frame);
                }
                if (eq_page_visible()) {
                    eq_draw(frame);
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
    if (g_vm && g_live && g->state == RB_STATE_PLAY && vm_gfx_active(g_vm) &&
        sys_frame_pixels()) {
        /* COLOR FILTER and CRT: gameplay only (attract demo included),
         * playfield only. Title, menus, ranking and the side panels stay
         * as drawn; the panels copy the playfield after this. While the
         * match is paused the last filtered frame stays under the pause
         * menu and its config pages, which are drawn over it. */
        static uint32_t snap_raw[RB_NATIVE_W * RB_NATIVE_H];
        static uint32_t snap_f[RB_NATIVE_W * RB_NATIVE_H];
        static int snap_ok;
        uint32_t *px = sys_frame_pixels();
        size_t i, n = (size_t)RB_NATIVE_W * RB_NATIVE_H;

        if (!g_pmenu && !menu_visible()) {
            memcpy(snap_raw, px, n * 4u);
            cfilter_set_layer(gfx_layer_mask());
            cfilter_set_bg(gfx_bg_luma());
            cfilter_set_kind(gfx_kind_mask());
            {
                const uint16_t *wsid;
                const uint8_t *wbgi;
                int wsc = gfx_world(&wsid, &wbgi);

                cfilter_set_world(wsid, wbgi, wsc);
                {
                    const uint8_t *tm;
                    const uint32_t *tp;

                    gfx_text_mask(&tm, &tp);
                    cfilter_set_text(tm, tp);
                }
            }
            {
                RbVmInfo zi;

                vm_info(g_vm, &zi);
                cfilter_set_stage(zi.chunk >= 2 && zi.chunk <= 6 ? (int)zi.chunk - 1 : 1);
            }
            aura_draw(px, g);
            {
                /* ZX: ship white, aura in its mode colour, shots and
                 * options in the weapon colour (global D8: 0 V red,
                 * 1 M yellow, 2 L blue). */
                int w8 = g_vm ? vm_global_u8(g_vm, 0xD8) : 0;

                cfilter_set_shade(gfx_shade_mask());
                cfilter_set_player(gfx_own_mask(), g_aura_any ? g_aura_px : NULL,
                                   g_aura_on == 2 ? 12 : g_aura_on == 3 ? 9 : 10,
                                   w8 == 1 ? 14 : w8 == 2 ? 9 : 10);
            }
            cfilter_apply(px, RB_NATIVE_W, RB_NATIVE_H, g_cfilter);
            crt_apply(px, RB_NATIVE_W, 80, 0, 480, RB_NATIVE_H, g_crt, g->frame);
            /* after the filters, so its 1 px sides stay crisp */
            hitbox_draw(px);
            memcpy(snap_f, px, n * 4u);
            snap_ok = 1;
        } else if (g_pmenu && snap_ok) {
            for (i = 0; i < n; i++) {
                uint32_t r0 = snap_raw[i], p = px[i];
                uint32_t dr = (((r0 >> 24) & 0xFFu) * 46u) >> 8;
                uint32_t dg = (((r0 >> 16) & 0xFFu) * 46u) >> 8;
                uint32_t db = ((((r0 >> 8) & 0xFFu) * 46u) >> 8) + 6u;
                uint32_t dim0 = (dr << 24) | (dg << 16) | (db << 8) | 0xFFu;

                if (p == r0) {
                    px[i] = snap_f[i];
                } else if (p == dim0) {
                    /* Under the menu box: the filtered frame, dimmed
                     * less than the box so the game shows through. */
                    uint32_t f = snap_f[i];
                    uint32_t fr = (((f >> 24) & 0xFFu) * 80u) >> 8;
                    uint32_t fg = (((f >> 16) & 0xFFu) * 80u) >> 8;
                    uint32_t fb = ((((f >> 8) & 0xFFu) * 80u) >> 8) + 6u;

                    px[i] = (fr << 24) | (fg << 16) | (fb << 8) | 0xFFu;
                }
            }
        }
    }
    if (g_vm && kraken_active() && sys_frame_pixels()) {
        kraken_draw(g_vm, vm_gfx(g_vm), sys_frame_pixels(), RB_NATIVE_W, RB_NATIVE_H);
    }
    gfx_set_hud_inset(panels_active(g));
    if (panels_active(g)) {
        panels_draw(g, sys_frame_pixels());
    } else {
        sys_set_wide(0);
        if (g_training && g->state == RB_STATE_PLAY) {
            sys_draw_text(8, 8, "TRAINING", 0x40FF40FFu);
        }
    }
    {
        /* Headless check of the wide layout: test hook=path dumps
         * the 1920x1080 frame at test hook. */
        const char *ps = ((char *)0);
        const char *at = ((char *)0);
        static int psaved;

        if (ps && ps[0] && !psaved && at && g->frame >= strtoul(at, NULL, 10) &&
            panels_active(g)) {
            FILE *fp = fopen(ps, "wb");

            if (fp) {
                if ((g_orient & 1)) {
                    fwrite(sys_tate_pixels(), 4, (size_t)RB_TATE_FW * RB_TATE_FH, fp);
                } else {
                    fwrite(sys_wide_pixels(), 4, (size_t)RB_WIDE_W * RB_WIDE_H, fp);
                }
                fclose(fp);
                fprintf(stderr, "panelshot %s\n", ps);
                psaved = 1;
            }
        }
    }
    if (menu_visible() || ((char *)0)) {
        const char *shot = ((char *)0);
        static int saved;
        uint32_t *px;
        FILE *fp;

        /* test hook waits until that logic frame. The settings
         * page draws its lower rows only after the cursor has visited
         * them, so a headless shot can land on the finished list. */
        if (shot && shot[0] && !saved) {
            const char *at = ((char *)0);
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

