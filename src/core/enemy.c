/**
 * Enemy pool — placeholder behaviours until SGC.COM tables are ported.
 */
#include "enemy.h"

#include <string.h>

void enemy_pool_init(RbEnemyPool *pool)
{
    memset(pool, 0, sizeof(*pool));
}

int enemy_spawn(RbEnemyPool *pool, uint16_t type, int16_t x, int16_t y)
{
    RbEnemy *e;

    if (pool->count >= RB_MAX_ENEMIES) {
        return -1;
    }
    e = &pool->items[pool->count++];
    memset(e, 0, sizeof(*e));
    e->type = type;
    e->pos.x = x;
    e->pos.y = y;
    e->vel.x = 0;
    e->vel.y = 2;
    e->hitbox.w = 16;
    e->hitbox.h = 16;
    e->hitbox.x = x;
    e->hitbox.y = y;
    e->hp = 1;
    e->flags = 1; /* alive */
    return (int)(pool->count - 1);
}

void enemy_pool_update(RbEnemyPool *pool)
{
    uint16_t i = 0;

    while (i < pool->count) {
        RbEnemy *e = &pool->items[i];
        e->pos.x = (int16_t)(e->pos.x + e->vel.x);
        e->pos.y = (int16_t)(e->pos.y + e->vel.y);
        e->hitbox.x = e->pos.x;
        e->hitbox.y = e->pos.y;
        e->timer++;

        if (e->hp <= 0 || e->pos.y > RB_NATIVE_H + 16) {
            pool->items[i] = pool->items[pool->count - 1];
            pool->count--;
            continue;
        }
        i++;
    }
}

void enemy_pool_render(const RbEnemyPool *pool)
{
    uint16_t i;

    for (i = 0; i < pool->count; i++) {
        const RbEnemy *e = &pool->items[i];
        RbSprite spr;
        memset(&spr, 0, sizeof(spr));
        spr.x = e->pos.x;
        spr.y = e->pos.y;
        spr.w = 16;
        spr.h = 16;
        spr.pixels = NULL;
        spr.solid_rgba = 0xFFFF6040u;
        sys_draw_sprite(&spr);
    }
}
