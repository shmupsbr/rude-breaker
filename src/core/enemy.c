/**
 * Enemy pool.
 *
 * Motion is the 8.8 integrator shared by the stream native (chunk 0x6F2E),
 * the set-piece native (0x6F7C), and the polar natives (0x7066 / 0x7102).
 * A polar script rebuilds the velocity words from angle and speed with the
 * sincos table at game-image 0x1848 before that add.
 */
#include "enemy.h"

#include <string.h>

/* 64-entry quarter sine, game image 0x1848. sincos (0xE80) mirrors it. */
static const int16_t SIN_Q[64] = {
    0, 804, 1608, 2410, 3212, 4011, 4808, 5602,
    6393, 7179, 7962, 8739, 9512, 10278, 11039, 11793,
    12539, 13279, 14010, 14732, 15446, 16151, 16846, 17530,
    18204, 18868, 19519, 20159, 20787, 21403, 22005, 22594,
    23170, 23731, 24279, 24811, 25329, 25832, 26319, 26790,
    27245, 27683, 28105, 28510, 28898, 29268, 29621, 29956,
    30273, 30571, 30852, 31113, 31356, 31580, 31785, 31971,
    32137, 32285, 32412, 32521, 32609, 32678, 32728, 32757
};

static int16_t sincos(uint8_t angle)
{
    uint16_t bx = angle;

    if ((bx & 0xC0u) == 0u) {
        return SIN_Q[bx];
    }
    if ((bx & 0x80u) == 0u) {
        bx = (uint16_t)(0u - bx);
        bx = (uint16_t)(bx + 0x7Fu);
        return SIN_Q[bx & 0x3Fu];
    }
    if ((bx & 0x40u) == 0u) {
        bx = (uint16_t)(bx - 0x80u);
        return (int16_t)(-SIN_Q[bx & 0x3Fu]);
    }
    bx = (uint16_t)(0u - bx);
    bx = (uint16_t)(bx + 0xFFu);
    return (int16_t)(-SIN_Q[bx & 0x3Fu]);
}

/* High byte of sincos, times the high byte of the speed word (IMUL DH). */
static int16_t polar_component(uint8_t angle, uint16_t speed)
{
    int16_t sample = sincos(angle);
    int16_t s = (int16_t)(int8_t)((uint16_t)sample >> 8);
    int16_t m = (int16_t)(int8_t)(speed >> 8);

    return (int16_t)(s * m);
}

static int16_t step_axis(int16_t pos, uint16_t vel, uint8_t *frac)
{
    uint8_t frac_vel = (uint8_t)vel;
    int16_t whole = (int16_t)(int8_t)(vel >> 8);
    uint16_t sum = (uint16_t)(*frac) + (uint16_t)frac_vel;

    *frac = (uint8_t)sum;
    if (sum > 0xFFu) {
        whole = (int16_t)(whole + 1);
    }
    return (int16_t)(pos + whole);
}

static void place_box(RbEnemy *e)
{
    int16_t l = e->box_l;
    int16_t r = e->box_r;
    int16_t t = e->box_t;
    int16_t b = e->box_b;

    e->hitbox.x = (int16_t)(e->pos.x + l);
    e->hitbox.y = (int16_t)(e->pos.y + t);
    e->hitbox.w = (int16_t)(r - l);
    e->hitbox.h = (int16_t)(b - t);
}

static int outside(const RbEnemy *e, int16_t x, int16_t y)
{
    switch (e->mover) {
    case RB_MOVER_STREAM:
        /* 0x6F2E: X < -16 or X >= 0x290, Y < 0 or Y >= 0x1A0. */
        return x < -16 || x >= 0x290 || y < 0 || y >= 0x1A0;
    case RB_MOVER_POLAR_WIDE:
        /* 0x7102: X < -16 or X >= 0x290, Y < -160 or Y >= 0x1F0. */
        return x < -16 || x >= 0x290 || y < (int16_t)0xFF60 || y >= 0x1F0;
    case RB_MOVER_SET:
        /* 0x6F7C: X < 0x10 or X >= 0x270, Y < -64 or Y >= 0x1D0. */
        return x < 0x10 || x >= 0x270 || y < (int16_t)0xFFC0 || y >= 0x1D0;
    case RB_MOVER_POLAR:
        /* 0x7066: X < 0x10 or X >= 0x270, Y < -160 or Y >= 0x1A0. */
        return x < 0x10 || x >= 0x270 || y < (int16_t)0xFF60 || y >= 0x1A0;
    default:
        return y > RB_NATIVE_H + 32 || y < -80 || x < -32 || x > RB_NATIVE_W + 32;
    }
}

static void integrate(RbEnemy *e, uint16_t vx, uint16_t vy)
{
    e->pos.x = step_axis(e->pos.x, vx, &e->frac_x);
    e->pos.y = step_axis(e->pos.y, vy, &e->frac_y);
    if (outside(e, e->pos.x, e->pos.y)) {
        e->hp = 0;
        e->type = (uint16_t)(e->type | 0x400u);
    }
}

void enemy_pool_init(RbEnemyPool *pool)
{
    memset(pool, 0, sizeof(*pool));
}

int enemy_spawn_spec(RbEnemyPool *pool, const RbEnemySpec *spec)
{
    RbEnemy *e;

    if (pool->count >= RB_MAX_ENEMIES) {
        return -1;
    }
    e = &pool->items[pool->count++];
    memset(e, 0, sizeof(*e));
    e->type = spec->type;
    e->pos.x = spec->x;
    e->pos.y = spec->y;
    e->vel.x = (int16_t)spec->vx;
    e->vel.y = (int16_t)spec->vy;
    e->ax = (int16_t)spec->ax;
    e->ay = (int16_t)spec->ay;
    e->angle = spec->angle;
    e->spin = spec->spin;
    e->speed = spec->speed;
    e->hp = spec->hp > 0 ? spec->hp : 1;
    e->hp_seen = e->hp;
    e->mover = spec->mover;
    e->counted = spec->counted;
    e->flags = 1;
    if (spec->box_l == 0 && spec->box_r == 0 && spec->box_t == 0 && spec->box_b == 0) {
        /* The opener did not write a box. A zero box never overlaps, so
         * the visible stand-in is ±8 until that sub-op is ported. */
        e->box_l = -8;
        e->box_r = 8;
        e->box_t = -8;
        e->box_b = 8;
    } else {
        e->box_l = spec->box_l;
        e->box_r = spec->box_r;
        e->box_t = spec->box_t;
        e->box_b = spec->box_b;
    }
    place_box(e);
    return (int)(pool->count - 1);
}

int enemy_spawn(RbEnemyPool *pool, uint16_t type, int16_t x, int16_t y)
{
    RbEnemySpec spec;

    memset(&spec, 0, sizeof(spec));
    spec.type = type;
    spec.x = x;
    spec.y = y;
    spec.hp = 1;
    spec.vy = 0x0200; /* +2.0 px/frame, the old stand-in step */
    spec.mover = RB_MOVER_STREAM;
    spec.box_l = -8;
    spec.box_r = 8;
    spec.box_t = -8;
    spec.box_b = 8;
    return enemy_spawn_spec(pool, &spec);
}

uint16_t enemy_pool_update(RbEnemyPool *pool)
{
    uint16_t i = 0;
    uint16_t counted = 0;

    while (i < pool->count) {
        RbEnemy *e = &pool->items[i];
        uint16_t vx;
        uint16_t vy;

        if (e->mover == RB_MOVER_POLAR || e->mover == RB_MOVER_POLAR_WIDE) {
            /* Sub-ops 0x22 and 0x1E replace +0x2A / +0x2C, then 0x06 adds
             * the spin, then the native adds +0x26 / +0x28 once. */
            int16_t px = polar_component((uint8_t)((e->angle + 0x4000u) >> 8), e->speed);
            int16_t py = polar_component((uint8_t)(e->angle >> 8), e->speed);

            e->angle = (uint16_t)(e->angle + e->spin);
            vx = (uint16_t)(px + e->vel.x);
            vy = (uint16_t)(py + e->vel.y);
            integrate(e, vx, vy);
        } else if (e->mover == RB_MOVER_STREAM || e->mover == RB_MOVER_SET) {
            vx = (uint16_t)e->vel.x;
            vy = (uint16_t)e->vel.y;
            integrate(e, vx, vy);
            e->vel.x = (int16_t)((uint16_t)e->vel.x + (uint16_t)e->ax);
            e->vel.y = (int16_t)((uint16_t)e->vel.y + (uint16_t)e->ay);
        } else {
            e->pos.x = (int16_t)(e->pos.x + (int16_t)(int8_t)((uint16_t)e->vel.x >> 8));
            e->pos.y = (int16_t)(e->pos.y + (int16_t)(int8_t)((uint16_t)e->vel.y >> 8));
            if (outside(e, e->pos.x, e->pos.y)) {
                e->hp = 0;
            }
        }
        place_box(e);
        e->timer++;

        if (e->hp <= 0) {
            if (e->counted) {
                counted++;
            }
            pool->items[i] = pool->items[pool->count - 1];
            pool->count--;
            continue;
        }
        i++;
    }
    return counted;
}

uint32_t enemy_collect_score(RbEnemyPool *pool)
{
    uint16_t i;
    uint32_t add = 0;

    for (i = 0; i < pool->count; i++) {
        RbEnemy *e = &pool->items[i];

        if (e->hp != e->hp_seen) {
            /* 0x71DC: one +10 per frame the saved hp differs, not per point. */
            add += 10u;
            e->hp_seen = e->hp;
            sys_play_sound(0x25, 180);
        }
    }
    return add;
}

void enemy_pool_render(const RbEnemyPool *pool)
{
    uint16_t i;

    for (i = 0; i < pool->count; i++) {
        const RbEnemy *e = &pool->items[i];
        RbSprite spr;
        uint32_t color = 0xFF80FF80u;

        memset(&spr, 0, sizeof(spr));
        spr.x = e->hitbox.x;
        spr.y = e->hitbox.y;
        spr.w = (uint16_t)(e->hitbox.w > 0 ? e->hitbox.w : 8);
        spr.h = (uint16_t)(e->hitbox.h > 0 ? e->hitbox.h : 8);
        spr.pixels = NULL;
        if (e->mover == RB_MOVER_STREAM) {
            color = 0xFFFF6040u;
        } else if (e->mover == RB_MOVER_SET) {
            color = 0xFFFF40FFu;
        } else if (e->mover == RB_MOVER_POLAR || e->mover == RB_MOVER_POLAR_WIDE) {
            color = 0xFFFFFF40u;
        }
        spr.solid_rgba = color;
        sys_draw_sprite(&spr);
    }
}
