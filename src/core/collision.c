/**
 * Opcode 0x4F (hit every match) and 0x50 (hit the first match).
 *
 * Both use the same five signed compares. The live ship response is the
 * proxy entity at chunk 0x5641: opcode 0x50, mask 0x00E0, then
 * `word [target+0x3C] -= 4`. The proxy's own counter starts at 1 and is
 * decremented once, so one connecting frame spends the life. The script
 * tail that respawns that proxy is not ported; separation stands in for it.
 *
 * Mask 0x00C0 / damage 6 (the ship script at 0x4F9A) is a different class
 * and is not wired to a spawned type yet.
 */
#include "collision.h"

#include <string.h>

#define RB_PF_HIT_LATCH 0x0001u

/* Proxy box at chunk 0x5641, not the movement routine's Y box. */
#define PROXY_L (-6)
#define PROXY_R 6
#define PROXY_T (-6)
#define PROXY_B 6

#define HIT_DAMAGE 4
#define BODY_MASK  0x00E0u
#define BODY_SKIP  0x0100u

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

int rb_hit_test(int16_t sx, int16_t sy,
                int16_t sl, int16_t sr, int16_t st, int16_t sb,
                int16_t ox, int16_t oy,
                int16_t ol, int16_t oright, int16_t ot, int16_t ob)
{
    int16_t self_l = (int16_t)(sx + sl);
    int16_t self_r = (int16_t)(sx + sr);
    int16_t self_t = (int16_t)(sy + st);
    int16_t self_b = (int16_t)(sy + sb);
    int16_t other_l = (int16_t)(ox + ol);
    int16_t other_r = (int16_t)(ox + oright);
    int16_t other_t = (int16_t)(oy + ot);
    int16_t other_b = (int16_t)(oy + ob);

    /* Miss on JGE / JL pairs from 0x426E and 0x4314, plus other_bottom < 0x20. */
    if (self_l >= other_r) {
        return 0;
    }
    if (self_r < other_l) {
        return 0;
    }
    if (self_t >= other_b) {
        return 0;
    }
    if (other_b < 0x20) {
        return 0;
    }
    if (self_b < other_t) {
        return 0;
    }
    return 1;
}

int rect_overlap(const RbRect *a, const RbRect *b)
{
    return rb_hit_test(a->x, a->y, 0, a->w, 0, a->h,
                       b->x, b->y, 0, b->w, 0, b->h);
}

static int body_candidate(const RbEnemy *e)
{
    if ((e->type & BODY_SKIP) != 0u) {
        return 0;
    }
    return (e->type & BODY_MASK) != 0u;
}

static int proxy_hits(const RbPlayer *player, const RbEnemy *e)
{
    return rb_hit_test(player->pos.x, player->pos.y,
                       PROXY_L, PROXY_R, PROXY_T, PROXY_B,
                       e->pos.x, e->pos.y,
                       e->box_l, e->box_r, e->box_t, e->box_b);
}

void collision_player_vs_enemies(RbPlayer *player, RbEnemyPool *enemies)
{
    uint16_t i;
    int overlapped = 0;

    for (i = 0; i < enemies->count; i++) {
        RbEnemy *e = &enemies->items[i];
        if (body_candidate(e) && proxy_hits(player, e)) {
            overlapped = 1;
            break;
        }
    }

    if (player->flags & RB_PF_HIT_LATCH) {
        if (!overlapped) {
            player->flags = (uint16_t)(player->flags & (uint16_t)~RB_PF_HIT_LATCH);
        }
        return;
    }
    if (!overlapped) {
        return;
    }

    for (i = 0; i < enemies->count; i++) {
        RbEnemy *e = &enemies->items[i];
        if (!body_candidate(e) || !proxy_hits(player, e)) {
            continue;
        }
        /* Opcode 0x50: decrement the attacker counter, run the sub-op on
         * the first target, then stop the scan. */
        e->hp = (int16_t)(e->hp - HIT_DAMAGE);
        player->lives--;
        player->flags = (uint16_t)(player->flags | RB_PF_HIT_LATCH);
        sys_play_sound(2, 220);
        break;
    }
}

void collision_bullets_vs_enemies(RbBulletPool *bullets, RbEnemyPool *enemies)
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
            int16_t bl = (int16_t)(b->hitbox.x - b->pos.x);
            int16_t bt = (int16_t)(b->hitbox.y - b->pos.y);

            if (!body_candidate(e)) {
                continue;
            }
            if (rb_hit_test(b->pos.x, b->pos.y, bl, (int16_t)(bl + b->hitbox.w),
                            bt, (int16_t)(bt + b->hitbox.h),
                            e->pos.x, e->pos.y,
                            e->box_l, e->box_r, e->box_t, e->box_b)) {
                /* Stand-in shot. The real shot script (chunk 0x62E3) is
                 * not ported; damage 1 is not an original constant. */
                e->hp = (int16_t)(e->hp - b->damage);
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

    /* Enemy shots of mask 0x00C0 are not spawned. Kept so a later port of
     * that class can use the same edges and the separation latch. */
    if (player->flags & RB_PF_HIT_LATCH) {
        return;
    }
    for (i = 0; i < bullets->count; i++) {
        RbBullet *b = &bullets->items[i];
        int16_t bl;
        int16_t bt;

        if (b->owner == 0) {
            continue;
        }
        bl = (int16_t)(b->hitbox.x - b->pos.x);
        bt = (int16_t)(b->hitbox.y - b->pos.y);
        if (rb_hit_test(player->pos.x, player->pos.y,
                        PROXY_L, PROXY_R, PROXY_T, PROXY_B,
                        b->pos.x, b->pos.y,
                        bl, (int16_t)(bl + b->hitbox.w),
                        bt, (int16_t)(bt + b->hitbox.h))) {
            player->lives--;
            player->flags = (uint16_t)(player->flags | RB_PF_HIT_LATCH);
            b->life = 0;
            sys_play_sound(2, 220);
            break;
        }
    }
}
