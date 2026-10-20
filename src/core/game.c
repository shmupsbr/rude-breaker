/**
 * Game state machine. Platform-agnostic; talks to hardware only via hal.h.
 */
#include "game.h"
#include "player.h"
#include "enemy.h"
#include "collision.h"
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

void game_init(RbGame *g, const char *asset_dir)
{
    memset(g, 0, sizeof(*g));
    g->state = RB_STATE_BOOT;
    g->hiscore = 0;
    g->stage = 1;
    g->scroll_y = 0;
    g->scroll_speed = 2;
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
            player_reset(&g->player);
            enemy_pool_init(&g_enemies);
            bullet_pool_init(&g_bullets);
            g->score = 0;
            g->scroll_y = 0;
            g->scroll_speed = 2;
            g->state = RB_STATE_PLAY;
            sys_play_music(0);
        }
        break;

    case RB_STATE_PLAY:
        /* Entity phase of frame_loop (game image 0x933): the ship update
         * runs as one entity, then the remaining actors. Spawn scripts are
         * still bytecode, so the wave below stays a stand-in. */
        player_update(&g->player, &g->input);
        enemy_pool_update(&g_enemies);
        bullet_pool_update(&g_bullets);

        /* Opcode 0x4B: scroll_y += scroll_speed, wrap at 400. */
        g->scroll_y = (int16_t)(g->scroll_y + g->scroll_speed);
        if (g->scroll_y >= 0x190) {
            g->scroll_y = (int16_t)(g->scroll_y - 0x190);
        }

        /* Placeholder wave until the stage bytecode spawn tables are ported. */
        if ((g->frame % 90u) == 0u && g_enemies.count < 8u) {
            int16_t x = (int16_t)(40 + (g->frame * 37u) % (RB_NATIVE_W - 80));
            enemy_spawn(&g_enemies, 1, x, -16);
        }

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
        collision_bullets_vs_enemies(&g_bullets, &g_enemies, &g->score);
        collision_bullets_vs_player(&g_bullets, &g->player);

        if (g->player.lives <= 0) {
            sys_stop_music();
            if (g->score > g->hiscore) {
                g->hiscore = g->score;
            }
            g->state = RB_STATE_RANKING;
        }
        if (btn_pressed(g, RB_BTN_START)) {
            /* Pause returns to title for now. */
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
    char line[64];

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
        snprintf(line, sizeof(line), "SCORE %05u  HI %05u  LIVES %d",
                 (unsigned)g->score, (unsigned)g->hiscore, (int)g->player.lives);
        sys_draw_text(8, 8, line, 0xFFFFFFFFu);
        break;

    case RB_STATE_RANKING:
        sys_draw_text(240, 160, "GAME OVER", 0xFFFF4040u);
        snprintf(line, sizeof(line), "SCORE %05u", (unsigned)g->score);
        sys_draw_text(240, 200, line, 0xFFFFFFFFu);
        sys_draw_text(200, 260, "PRESS Z / ENTER", 0xFFFFFF00u);
        break;

    default:
        break;
    }
}
