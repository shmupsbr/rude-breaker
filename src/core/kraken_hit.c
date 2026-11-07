#include "kraken_hit.h"

int kr_shot_damage(int core_max)
{
    int arm;
    int hit;

    if (core_max < 64) {
        core_max = 480;
    }
    arm = core_max / 2;
    if (arm < 32) {
        arm = 32;
    }
    hit = arm / 16;
    if (hit < 8) {
        hit = 8;
    }
    return hit;
}

int kr_hits_to_zero(int hp, int dmg)
{
    int n = 0;

    if (dmg < 1) {
        dmg = 1;
    }
    while (hp > 0 && n < 100000) {
        hp -= dmg;
        n++;
    }
    return n;
}

int kr_core_hit(int hx, int hy, int x, int y)
{
    /* 48 by 80, with a few pixels of slack so a shot into the sprite counts. */
    return x >= hx - 28 && x <= hx + 28 && y >= hy - 46 && y <= hy + 46;
}

int kr_is_projectile(unsigned type, unsigned kind, unsigned origin,
                     unsigned seg, int w, int h)
{
    if (kind == 0x0012u || kind == 0x0014u) {
        return 0;
    }
    /* Kraken's own parts, and the shot controller (no bullet sprite). */
    if (origin == 0xCE11u || origin == 0x62E3u || origin == 0x62E9u) {
        return 0;
    }
    if (w <= 0 || h <= 0 || w * h > 8) {
        return 0;
    }
    if ((type & 0x0080u) && !(type & 0x1000u)) {
        return 0;
    }
    if (type & 0x1000u) {
        return 1;
    }
    if ((type & 0x0004u) && (kind == 0u || seg == 26u)) {
        return 1;
    }
    if (kind == 0u && seg == 26u) {
        return 1;
    }
    if (origin == 0x6331u || origin == 0x6498u ||
        origin == 0x6337u || origin == 0x649Eu) {
        return 1;
    }
    return 0;
}
