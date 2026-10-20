#ifndef RUDEBREAKER_ENEMY_H
#define RUDEBREAKER_ENEMY_H

#include "hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RB_MAX_ENEMIES 64

typedef struct RbEnemyPool {
    RbEnemy  items[RB_MAX_ENEMIES];
    uint16_t count;
    uint16_t _pad;
} RbEnemyPool;

void enemy_pool_init(RbEnemyPool *pool);
void enemy_pool_update(RbEnemyPool *pool);
void enemy_pool_render(const RbEnemyPool *pool);
int  enemy_spawn(RbEnemyPool *pool, uint16_t type, int16_t x, int16_t y);

#ifdef __cplusplus
}
#endif

#endif /* RUDEBREAKER_ENEMY_H */
