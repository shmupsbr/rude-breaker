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

#include <stdio.h>
#include <string.h>

static RbEnemyPool  g_enemies;
static RbBulletPool g_bullets;
static RbAssets     g_assets;

static int btn_pressed(const RbGame *g, uint16_t mask)
{
    return (g->input.buttons & mask) && !(g->input_prev.buttons & mask);
}

static void begin_stage(RbGame *g, uint16_t stage, int fresh_ship)
{
    int16_t lives = g->player.lives;

    enemy_pool_init(&g_enemies);
    bullet_pool_init(&g_bullets);
    stage_begin(stage);
    g->stage = stage;
    g->scroll_y = stage_scroll_y();
    g->scroll_speed = stage_scroll_speed();
    if (fresh_ship) {
        player_reset(&g->player);
    } else {
        /* Each stage's ship script calls the same spawn (320, 344).
         * Lives stay; the original stage-clear script is not ported. */
        player_reset(&g->player);
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

    if (assets_load(&g_assets, g->asset_dir) == 0) {
        fprintf(stderr, "assets: loaded SGM1 (%u chunks, %zu bytes) from %s\n",
                (unsigned)g_assets.sgm1.count, g_assets.sgm1_size, g->asset_dir);
    } else {
        fprintf(stderr, "assets: optional load failed for %s (running without ROM data)\n",
                g->asset_dir);
    }

    g->state = RB_STATE_TITLE;
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
        /* Entity phase of frame_loop (game image 0x933). The ship native
         * runs, then the other actors move, then the director spawns.
         * A child created this frame does not move until the next one. */
        player_update(&g->player, &g->input);
        stage_note_despawn(enemy_pool_update(&g_enemies));
        stage_tick(&g_enemies);
        g->scroll_y = stage_scroll_y();
        g->scroll_speed = stage_scroll_speed();
        bullet_pool_update(&g_bullets);

        /* Stand-in for the shot controller at chunk 0x62E3. Damage 1 and
         * velocity -8 are not original constants. Score comes from the
         * hp-change path, not from the shot. */
        if (btn_pressed(g, RB_BTN_A)) {
            RbBullet shot;
            memset(&shot, 0, sizeof(shot));
            shot.pos.x = (int16_t)(g->player.pos.x + 8);
            shot.pos.y = (int16_t)(g->player.pos.y - 8);
            shot.vel.x = 0;
            shot.vel.y = -8;
            shot.hitbox.x = shot.pos.x;
            shot.hitbox.y = shot.pos.y;
            shot.hitbox.w = 4;
            shot.hitbox.h = 8;
            shot.damage = 1;
            shot.owner = 0;
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
            /* Heuristic. The original stage-clear script is not ported.
             * Stage 5's decoded timeline does not reach the terminal rule,
             * so that stage stays up until the ship is lost. */
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
}

void game_render(const RbGame *g)
{
    char line[80];

    sys_clear(8, 8, 24);

    switch (g->state) {
    case RB_STATE_TITLE:
        sys_draw_text(200, 140, "RUDE BREAKER", 0xFFFFFFFFu);
        sys_draw_text(180, 180, "PC-98 clean-room C99 port", 0xFFA0A0A0u);
        sys_draw_text(200, 240, "PRESS Z / ENTER", 0xFFFFFF00u);
        if (g_assets.loaded) {
            snprintf(line, sizeof(line), "SGM1 chunks: %u",
                     (unsigned)g_assets.sgm1.count);
            sys_draw_text(200, 280, line, 0xFF80FF80u);
        } else {
            sys_draw_text(160, 280, "No assets (scaffold mode)", 0xFFFF8080u);
        }
        break;

    case RB_STATE_PLAY:
        player_render(&g->player);
        enemy_pool_render(&g_enemies);
        bullet_pool_render(&g_bullets);
        snprintf(line, sizeof(line), "SCORE %u  HI %u  LIVES %d",
                 (unsigned)g->score, (unsigned)g->hiscore, (int)g->player.lives);
        sys_draw_text(8, 8, line, 0xFFFFFFFFu);
        snprintf(line, sizeof(line), "STAGE %u  CELL %u  SPD %d",
                 (unsigned)g->stage, (unsigned)stage_cell(), (int)g->scroll_speed);
        sys_draw_text(8, 24, line, 0xFFC0C0C0u);
        break;

    case RB_STATE_RANKING:
        sys_draw_text(240, 160, "GAME OVER", 0xFFFF4040u);
        snprintf(line, sizeof(line), "SCORE %u", (unsigned)g->score);
        sys_draw_text(240, 200, line, 0xFFFFFFFFu);
        sys_draw_text(200, 260, "PRESS Z / ENTER", 0xFFFFFF00u);
        break;

    default:
        break;
    }
}
