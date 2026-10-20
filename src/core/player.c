/**
 * Player ship.
 *
 * Per-frame movement is the native routine at SGM1 chunk 1 offset 0x53B4
 * (falls into the velocity add at 0x5404). The ship is an entity: X is
 * field +0x22, Y is field +0x24. Direction comes from the low nibble of
 * the held-input word (loader poll at 0x7846):
 *   bit0 up, bit1 left, bit2 right, bit3 down.
 * Three 16-entry dx/dy tables sit at chunk offsets 0x54BE (slow),
 * 0x54FE (normal) and 0x553E (fast). Debug flag bits 0x10 / 0x20 on
 * the flag word select slow / fast; a normal frame uses the middle table.
 *
 * The origin is clamped to x in [0x60, 0x220] and y in [0x38, 0x168].
 * Spawn coordinates 0x140, 0x158 and the Y hitbox (-8 .. +10) come from
 * the sub-opcode block immediately before the routine, at chunk 0x536E.
 * X hitbox offsets are rewritten every frame: -6 .. +6, or -10 .. +10
 * when mode byte [bp+0xD5] is 1. This port uses the default box.
 */
#include "player.h"

#include <string.h>

/* Origin of the ship, inclusive. */
#define SHIP_X_MIN 0x0060
#define SHIP_X_MAX 0x0220
#define SHIP_Y_MIN 0x0038
#define SHIP_Y_MAX 0x0168

#define SHIP_SPAWN_X 0x0140
#define SHIP_SPAWN_Y 0x0158

/* Default hitbox around the origin (mode byte != 1). */
#define SHIP_BOX_L (-6)
#define SHIP_BOX_R 6
#define SHIP_BOX_T (-8)
#define SHIP_BOX_B 10

enum {
    SHIP_SPEED_SLOW = 0,
    SHIP_SPEED_NORMAL = 1,
    SHIP_SPEED_FAST = 2
};

/* Indexed by (input & 0x0F). Order: none, U, L, UL, R, UR, LR, ULR,
 * D, UD, DL, UDL, DR, UDR, DLR, all. */
static const int16_t SHIP_VEL[3][16][2] = {
    { /* 0x54BE slow */
        { 0, 0}, { 0,-5}, {-6, 0}, {-5,-4},
        { 6, 0}, { 5,-4}, { 0, 0}, { 0,-5},
        { 0, 5}, { 0, 0}, {-5, 4}, {-6, 0},
        { 5, 4}, { 6, 0}, { 0, 4}, { 0, 0}
    },
    { /* 0x54FE normal */
        { 0, 0}, { 0,-6}, {-8, 0}, {-6,-5},
        { 8, 0}, { 6,-5}, { 0, 0}, { 0,-6},
        { 0, 6}, { 0, 0}, {-6, 5}, {-8, 0},
        { 6, 5}, { 8, 0}, { 0, 5}, { 0, 0}
    },
    { /* 0x553E fast */
        { 0, 0}, { 0,-8},{-10, 0}, {-8,-7},
        {10, 0}, { 8,-7}, { 0, 0}, { 0,-8},
        { 0, 8}, { 0, 0}, {-8, 7},{-10, 0},
        { 8, 7}, {10, 0}, { 0, 7}, { 0, 0}
    }
};

static int16_t clamp_origin(int16_t v, int16_t lo, int16_t hi)
{
    if (v < lo) {
        return lo;
    }
    if (v > hi) {
        return hi;
    }
    return v;
}

static void place_hitbox(RbPlayer *p)
{
    p->hitbox.x = (int16_t)(p->pos.x + SHIP_BOX_L);
    p->hitbox.y = (int16_t)(p->pos.y + SHIP_BOX_T);
    p->hitbox.w = (int16_t)(SHIP_BOX_R - SHIP_BOX_L);
    p->hitbox.h = (int16_t)(SHIP_BOX_B - SHIP_BOX_T);
}

/* Low nibble of the original held-input word. */
static unsigned ship_dir(const RbInput *in)
{
    unsigned dir = 0;

    if (in->buttons & RB_BTN_UP) {
        dir |= 0x1u;
    }
    if (in->buttons & RB_BTN_LEFT) {
        dir |= 0x2u;
    }
    if (in->buttons & RB_BTN_RIGHT) {
        dir |= 0x4u;
    }
    if (in->buttons & RB_BTN_DOWN) {
        dir |= 0x8u;
    }
    return dir;
}

void player_reset(RbPlayer *p)
{
    memset(p, 0, sizeof(*p));
    p->pos.x = SHIP_SPAWN_X;
    p->pos.y = SHIP_SPAWN_Y;
    p->hp = 1;
    p->lives = 3;
    p->invuln_frames = 0;
    place_hitbox(p);
}

void player_update(RbPlayer *p, const RbInput *in)
{
    unsigned dir = ship_dir(in);
    /* Normal table. Slow/fast are debug-flag selections, not the shipping frame. */
    const int16_t *vel = SHIP_VEL[SHIP_SPEED_NORMAL][dir & 0x0Fu];

    p->vel.x = vel[0];
    p->vel.y = vel[1];
    p->pos.x = clamp_origin((int16_t)(p->pos.x + p->vel.x), SHIP_X_MIN, SHIP_X_MAX);
    p->pos.y = clamp_origin((int16_t)(p->pos.y + p->vel.y), SHIP_Y_MIN, SHIP_Y_MAX);
    place_hitbox(p);

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
    spr.x = p->hitbox.x;
    spr.y = p->hitbox.y;
    spr.w = (uint16_t)p->hitbox.w;
    spr.h = (uint16_t)p->hitbox.h;
    spr.pixels = NULL;
    spr.solid_rgba = 0xFF40C0FFu;
    sys_draw_sprite(&spr);
}
