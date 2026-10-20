#ifndef RUDEBREAKER_ENEMY_H
#define RUDEBREAKER_ENEMY_H

#include "hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The dispatcher walks 256 slots. The stream spawns about one enemy per
 * frame and each one lives on the order of 150 frames. */
#define RB_MAX_ENEMIES 256

/* Which native integrator the script calls after its opener. */
#define RB_MOVER_NONE      0u
#define RB_MOVER_STREAM    1u  /* chunk 0x6F2E */
#define RB_MOVER_SET       2u  /* chunk 0x6F7C */
#define RB_MOVER_POLAR     3u  /* sincos into +0x2A/+0x2C, then 0x7066 */
#define RB_MOVER_POLAR_WIDE 4u /* same polar step, kill bounds of 0x7102 */

typedef struct RbEnemySpec {
    int16_t  x;
    int16_t  y;
    int16_t  hp;
    uint16_t vx;
    uint16_t vy;
    uint16_t ax;
    uint16_t ay;
    uint16_t angle;
    uint16_t spin;
    uint16_t speed;
    int16_t  box_l;
    int16_t  box_r;
    int16_t  box_t;
    int16_t  box_b;
    uint16_t type;
    uint8_t  mover;
    uint8_t  counted;
} RbEnemySpec;

typedef struct RbEnemyPool {
    RbEnemy  items[RB_MAX_ENEMIES];
    uint16_t count;
    uint16_t _pad;
} RbEnemyPool;

void enemy_pool_init(RbEnemyPool *pool);
/* Moves the pool. Returns how many gate-counted enemies were removed. */
uint16_t enemy_pool_update(RbEnemyPool *pool);
void enemy_pool_render(const RbEnemyPool *pool);
int  enemy_spawn(RbEnemyPool *pool, uint16_t type, int16_t x, int16_t y);
int  enemy_spawn_spec(RbEnemyPool *pool, const RbEnemySpec *spec);
/* +10 for each enemy whose hp changed since the last call (native 0x71DC). */
uint32_t enemy_collect_score(RbEnemyPool *pool);

#ifdef __cplusplus
}
#endif

#endif /* RUDEBREAKER_ENEMY_H */
