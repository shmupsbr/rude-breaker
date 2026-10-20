#ifndef RUDEBREAKER_COLLISION_H
#define RUDEBREAKER_COLLISION_H

#include "hal.h"
#include "enemy.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RB_MAX_BULLETS 128

typedef struct RbBulletPool {
    RbBullet items[RB_MAX_BULLETS];
    uint16_t count;
    uint16_t _pad;
} RbBulletPool;

void bullet_pool_init(RbBulletPool *pool);
void bullet_pool_update(RbBulletPool *pool);
void bullet_pool_render(const RbBulletPool *pool);
int  bullet_spawn(RbBulletPool *pool, const RbBullet *proto);

/* Opcode 0x4F / 0x50 geometry. Offsets are added to each origin.
 * A hit also requires the target's bottom edge to be at or below y = 0x20.
 * Touching edges count: self_right >= other_left and self_bottom >= other_top. */
int  rb_hit_test(int16_t sx, int16_t sy,
                 int16_t sl, int16_t sr, int16_t st, int16_t sb,
                 int16_t ox, int16_t oy,
                 int16_t ol, int16_t oright, int16_t ot, int16_t ob);
int  rect_overlap(const RbRect *a, const RbRect *b);

/* Hit-first (opcode 0x50) of the ship proxy against enemies whose type
 * has a bit in 0x00E0 and not bit 0x0100. Damage is 4. One life is lost
 * per overlap; the respawn delay after the proxy counter hits 0 is not
 * ported, so further hits wait until the boxes separate. */
void collision_player_vs_enemies(RbPlayer *player, RbEnemyPool *enemies);

/* Stand-in player shots. They use the same box test and reduce hp.
 * Score is not applied here; native 0x71DC scores an hp change. */
void collision_bullets_vs_enemies(RbBulletPool *bullets, RbEnemyPool *enemies);
void collision_bullets_vs_player(RbBulletPool *bullets, RbPlayer *player);

#ifdef __cplusplus
}
#endif

#endif /* RUDEBREAKER_COLLISION_H */
