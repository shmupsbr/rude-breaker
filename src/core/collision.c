/**
 * Collision helpers and bullet pool.
 */
#include "collision.h"

#include <string.h>

void bullet_pool_init(RbBulletPool *pool)
{
    memset(pool, 0, sizeof(*pool));
}

int bullet_spawn(RbBulletPool *pool, const RbBullet *proto)
{
    if (pool->count >= RB_MAX_BULLETS) {
        return -1;
    }
    pool->items[pool->count++] = *proto;
    return (int)(pool->count - 1);
}

void bullet_pool_update(RbBulletPool *pool)
{
    uint16_t i = 0;

    while (i < pool->count) {
        RbBullet *b = &pool->items[i];
        b->pos.x = (int16_t)(b->pos.x + b->vel.x);
        b->pos.y = (int16_t)(b->pos.y + b->vel.y);
        b->hitbox.x = b->pos.x;
        b->hitbox.y = b->pos.y;
        if (b->life > 0) {
            b->life--;
        }
        if (b->life == 0
            || b->pos.y < -16
            || b->pos.y > RB_NATIVE_H + 16
            || b->pos.x < -16
            || b->pos.x > RB_NATIVE_W + 16) {
            pool->items[i] = pool->items[pool->count - 1];
            pool->count--;
            continue;
        }
        i++;
    }
}

void bullet_pool_render(const RbBulletPool *pool)
{
    uint16_t i;

    for (i = 0; i < pool->count; i++) {
        const RbBullet *b = &pool->items[i];
        RbSprite spr;
        memset(&spr, 0, sizeof(spr));
        spr.x = b->pos.x;
        spr.y = b->pos.y;
        spr.w = (uint16_t)(b->hitbox.w > 0 ? b->hitbox.w : 4);
        spr.h = (uint16_t)(b->hitbox.h > 0 ? b->hitbox.h : 8);
        spr.pixels = NULL;
        spr.solid_rgba = (b->owner == 0) ? 0xFFFFFFFFu : 0xFFFF20FFu;
        sys_draw_sprite(&spr);
    }
}

int rect_overlap(const RbRect *a, const RbRect *b)
{
    return a->x < (int16_t)(b->x + b->w)
        && (int16_t)(a->x + a->w) > b->x
        && a->y < (int16_t)(b->y + b->h)
        && (int16_t)(a->y + a->h) > b->y;
}

void collision_player_vs_enemies(RbPlayer *player, RbEnemyPool *enemies)
{
    uint16_t i;

    if (player->invuln_frames > 0) {
        return;
    }
    for (i = 0; i < enemies->count; i++) {
        RbEnemy *e = &enemies->items[i];
        if (rect_overlap(&player->hitbox, &e->hitbox)) {
            player->lives--;
            player->invuln_frames = 90;
            e->hp = 0;
            sys_play_sound(2, 220);
            break;
        }
    }
}

void collision_bullets_vs_enemies(RbBulletPool *bullets, RbEnemyPool *enemies,
                                  uint16_t *score_out)
{
    uint16_t bi = 0;

    while (bi < bullets->count) {
        RbBullet *b = &bullets->items[bi];
        uint16_t ei;
        int hit = 0;

        if (b->owner != 0) {
            bi++;
            continue;
        }
        for (ei = 0; ei < enemies->count; ei++) {
            RbEnemy *e = &enemies->items[ei];
            if (rect_overlap(&b->hitbox, &e->hitbox)) {
                e->hp = (int16_t)(e->hp - b->damage);
                if (e->hp <= 0 && score_out) {
                    *score_out = (uint16_t)(*score_out + 100);
                }
                hit = 1;
                break;
            }
        }
        if (hit) {
            bullets->items[bi] = bullets->items[bullets->count - 1];
            bullets->count--;
            continue;
        }
        bi++;
    }
}

void collision_bullets_vs_player(RbBulletPool *bullets, RbPlayer *player)
{
    uint16_t i;

    if (player->invuln_frames > 0) {
        return;
    }
    for (i = 0; i < bullets->count; i++) {
        RbBullet *b = &bullets->items[i];
        if (b->owner == 0) {
            continue;
        }
        if (rect_overlap(&b->hitbox, &player->hitbox)) {
            player->lives--;
            player->invuln_frames = 90;
            b->life = 0;
            sys_play_sound(2, 220);
            break;
        }
    }
}
