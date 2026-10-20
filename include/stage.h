/**
 * Stage director. Scroll, phase switch, and the spawn timeline decoded
 * from SGM1 chunks 2-6.
 */
#ifndef RUDEBREAKER_STAGE_H
#define RUDEBREAKER_STAGE_H

#include "hal.h"
#include "enemy.h"

#ifdef __cplusplus
extern "C" {
#endif

/* stage_index is 1..5 (chunks 2..6). */
void     stage_begin(uint16_t stage_index);
void     stage_tick(RbEnemyPool *pool);
void     stage_note_despawn(uint16_t counted);
int      stage_finished(void);
uint16_t stage_index(void);
uint16_t stage_cell(void);
int16_t  stage_scroll_y(void);
int16_t  stage_scroll_speed(void);

#ifdef __cplusplus
}
#endif

#endif /* RUDEBREAKER_STAGE_H */
