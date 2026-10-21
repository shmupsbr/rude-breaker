/**
 * Game state machine. Platform-agnostic; talks to hardware only via hal.h.
 */
#include "game.h"
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

static int file_in_dir(const char *dir, const char *name)
{
    char path[1200];
    FILE *fp;
    int n;

    if (!dir || !dir[0] || !name) {
        return 0;
    }
    n = snprintf(path, sizeof(path), "%s/%s", dir, name);
    if (n <= 0 || (size_t)n >= sizeof(path)) {
        return 0;
    }
    fp = fopen(path, "rb");
    if (!fp) {
        return 0;
    }
    fclose(fp);
    return 1;
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

static void report_disk(const char *dir, int audio_ok)
{
    int sgm1 = file_in_dir(dir, "SGM1.DAT");
    int fplay = file_in_dir(dir, "FPLAY.COM");
    int song = file_in_dir(dir, "SONG.DAT");
    char body[1400];

    if (sgm1 && fplay && song && audio_ok) {
        fprintf(stderr, "rudebreaker: SGM1.DAT, FPLAY.COM, and SONG.DAT found\n");
        return;
    }
    snprintf(body, sizeof(body),
             "Data directory:\n%s\n\n"
             "Copy these files from your own Rude Breaker disk into that folder\n"
             "(the folder that contains this program, unless you passed a path):\n"
             "  SGM1.DAT     required   %s\n"
             "  FPLAY.COM    required   %s\n"
             "  SONG.DAT     required   %s\n"
             "  RBRANK.DAT   optional   %s\n\n"
             "SGC.COM, READJS.COM, and RUDEBR.BAT are on the disk and are not read.\n"
             "ym2608_adpcm_rom.bin is optional (8192 bytes, CRC32 23c9e0d8).\n",
             dir ? dir : "",
             sgm1 ? "found" : "MISSING",
             fplay ? "found" : "MISSING",
             song ? "found" : "MISSING",
             file_in_dir(dir, "RBRANK.DAT") ? "found" : "absent");
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

void game_init(RbGame *g, const char *asset_dir)
{
    memset(g, 0, sizeof(*g));
    g->state = RB_STATE_BOOT;
    g->hiscore = 0;
    g->stage = 1;
    g->scroll_y = 0;
    g->scroll_speed = 0;
    if (asset_dir) {
        snprintf(g->asset_dir, sizeof(g->asset_dir), "%s", asset_dir);
    }

    player_reset(&g->player);
    enemy_pool_init(&g_enemies);
    bullet_pool_init(&g_bullets);

    {
        int audio_ok = sys_audio_bind(g->asset_dir) == 0;

        if (assets_load(&g_assets, g->asset_dir) == 0) {
            fprintf(stderr, "assets: loaded SGM1 (%u chunks, %zu bytes) from %s\n",
                    (unsigned)g_assets.sgm1.count, g_assets.sgm1_size, g->asset_dir);
            if (vm_create(&g_vm, &g_assets) != 0) {
                fprintf(stderr, "vm: create failed, using the scaffold director\n");
                g_vm = NULL;
            }
        } else {
            fprintf(stderr, "assets: SGM1.DAT was not loaded from %s\n",
                    g->asset_dir);
        }
        report_disk(g->asset_dir, audio_ok);
    }

    g->state = RB_STATE_TITLE;
    if (env_on("RUDEBR_TRACE") || getenv("RUDEBR_STOP")) {
        fprintf(stderr, "smoke: title\n");
    }
    smoke_trace(g, "boot");
}

int game_smoke_done(void)
{
    return g_smoke_stop;
}

int game_wants_quit(const RbGame *g)
{
    return g->state == RB_STATE_QUIT || (g->input.buttons & RB_BTN_QUIT);
}

void game_update(RbGame *g)
{
    g->frame++;

    switch (g->state) {
    case RB_STATE_BOOT:
        g->state = RB_STATE_TITLE;
        break;

    case RB_STATE_TITLE:
        if (btn_pressed(g, RB_BTN_START) || btn_pressed(g, RB_BTN_A)) {
            g->score = 0;
            begin_stage(g, 1, 1);
            g->state = RB_STATE_PLAY;
            sys_play_music(0);
        }
        break;

    case RB_STATE_PLAY:
        if (g_vm) {
            RbVmInfo info;

            /* One forward scan. A child spawned this frame runs only if
             * its slot is still ahead of the parent. RUDEBR_SUSTAIN is a
             * smoke-test switch; the frame loop itself never calls it. */
            if (env_on("RUDEBR_SUSTAIN")) {
                vm_sustain_ship(g_vm);
            }
            vm_set_input(g_vm, &g->input);
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
            }
            if (vm_quit_requested(g_vm)) {
                g->state = RB_STATE_QUIT;
            } else if (vm_boot_requested(g_vm) || info.lives <= 0) {
                sys_stop_music();
                if (g->score > g->hiscore) {
                    g->hiscore = g->score;
                }
                g->state = info.lives <= 0 ? RB_STATE_RANKING : RB_STATE_TITLE;
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
        /* START is the PC-98 ESC bit. During play it returns to the
         * title. F10 is the quit bit. */
        if (g->state == RB_STATE_PLAY && btn_pressed(g, RB_BTN_START)) {
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

    g->input_prev = g->input;
    watch_smoke(g);
    if (env_on("RUDEBR_TRACE") && (g->frame == 1 || (g->frame % 120u) == 0)) {
        smoke_trace(g, "frame");
    }
}

void game_render(const RbGame *g)
{
    char line[80];

    sys_clear(8, 8, 24);

    switch (g->state) {
    case RB_STATE_TITLE:
        sys_draw_text(200, 140, "RUDE BREAKER", 0xFFFFFFFFu);
        sys_draw_text(180, 180, "PC-98 clean-room C99 port", 0xFFA0A0A0u);
        sys_draw_text(200, 240, "PRESS Z / SPACE", 0xFFFFFF00u);
        if (g_assets.loaded) {
            snprintf(line, sizeof(line), "SGM1 chunks: %u",
                     (unsigned)g_assets.sgm1.count);
            sys_draw_text(200, 280, line, 0xFF80FF80u);
        } else {
            sys_draw_text(160, 280, "No assets (scaffold mode)", 0xFFFF8080u);
        }
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
}
