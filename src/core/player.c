/**
 * Player ship — logic only; rendering via HAL sprites.
 */
#include "player.h"

#include <string.h>

void player_reset(RbPlayer *p)
{
    memset(p, 0, sizeof(*p));
    p->pos.x = (int16_t)(RB_NATIVE_W / 2 - 8);
    p->pos.y = (int16_t)(RB_NATIVE_H - 48);
    p->hitbox.w = 16;
    p->hitbox.h = 16;
    p->hp = 1;
    p->lives = 3;
    p->invuln_frames = 0;
}

void player_update(RbPlayer *p, const RbInput *in)
{
    const int16_t speed = 4;

    p->vel.x = (int16_t)(in->axis_x * speed);
    p->vel.y = (int16_t)(in->axis_y * speed);
    p->pos.x = (int16_t)(p->pos.x + p->vel.x);
    p->pos.y = (int16_t)(p->pos.y + p->vel.y);

    if (p->pos.x < 0) {
        p->pos.x = 0;
    }
    if (p->pos.y < 0) {
        p->pos.y = 0;
    }
    if (p->pos.x > (int16_t)(RB_NATIVE_W - p->hitbox.w)) {
        p->pos.x = (int16_t)(RB_NATIVE_W - p->hitbox.w);
    }
    if (p->pos.y > (int16_t)(RB_NATIVE_H - p->hitbox.h)) {
        p->pos.y = (int16_t)(RB_NATIVE_H - p->hitbox.h);
    }

    p->hitbox.x = p->pos.x;
    p->hitbox.y = p->pos.y;

    if (p->invuln_frames > 0) {
        p->invuln_frames--;
    }
}

void player_render(const RbPlayer *p)
{
    RbSprite spr;

    if (p->invuln_frames && (p->invuln_frames & 2u)) {
        return;
    }

    memset(&spr, 0, sizeof(spr));
    spr.x = p->pos.x;
    spr.y = p->pos.y;
    spr.w = 16;
    spr.h = 16;
    spr.pixels = NULL;
    spr.solid_rgba = 0xFF40C0FFu;
    sys_draw_sprite(&spr);
}
