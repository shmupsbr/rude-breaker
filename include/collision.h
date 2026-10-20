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

int  rect_overlap(const RbRect *a, const RbRect *b);
void collision_player_vs_enemies(RbPlayer *player, RbEnemyPool *enemies);
void collision_bullets_vs_enemies(RbBulletPool *bullets, RbEnemyPool *enemies,
                                  uint16_t *score_out);
void collision_bullets_vs_player(RbBulletPool *bullets, RbPlayer *player);

#ifdef __cplusplus
}
#endif

#endif /* RUDEBREAKER_COLLISION_H */
