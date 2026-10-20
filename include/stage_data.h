/**
 * Decoded stage tables. Produced by tools/gen_stage_tables.py from
 * decompressed SGM1 chunks 2-6 (stages 1-5).
 */
#ifndef RUDEBREAKER_STAGE_DATA_H
#define RUDEBREAKER_STAGE_DATA_H

#include "hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RB_STAGE_COUNT 5

#define RB_EV_SPAWN 0u
#define RB_EV_GATE  1u
#define RB_EV_PHASE 2u

#define RB_SPEED_KEEP 0xFFFFu
#define RB_PHASE_KEEP 0xFFu

/* match_op: 0 = body always, 1 = cell equals, 2 = cell <=, 3 = cell >=. */
#define RB_MATCH_ALWAYS 0u
#define RB_MATCH_EQ     1u
#define RB_MATCH_LE     2u
#define RB_MATCH_GE     3u

typedef struct RB_PACKED RbSpawnDef {
    int16_t  x;
    uint16_t x_mod;     /* non-zero: x = (rng % x_mod) + x_add */
    int16_t  x_add;
    int16_t  y;
    int16_t  hp;
    uint16_t vx;        /* 8.8, entity +0x26 */
    uint16_t vy;        /* 8.8, entity +0x28 */
    uint16_t ax;        /* entity +0x2A */
    uint16_t ay;        /* entity +0x2C */
    uint16_t angle;     /* entity +0x2E */
    uint16_t spin;      /* entity +0x30 */
    uint16_t speed;     /* entity +0x32 */
    int16_t  box_l;
    int16_t  box_r;
    int16_t  box_t;
    int16_t  box_b;
    uint16_t type;
    uint8_t  mover;
    uint8_t  counted;   /* script adds 1 to [bp+0xAE] and the gate waits for 0 */
} RbSpawnDef;

typedef struct RB_PACKED RbStageEvent {
    uint16_t delay;     /* frames to wait before the action */
    uint16_t kind;
    uint16_t arg;       /* spawn index, or the phase byte */
} RbStageEvent;

typedef struct RB_PACKED RbCellWave {
    uint16_t cell;      /* [bp+0x7C] */
    uint16_t spawn;
} RbCellWave;

typedef struct RB_PACKED RbPhaseRule {
    uint8_t  phase;         /* [bp+0xC8] */
    uint8_t  match_op;
    uint16_t match_cell;
    uint16_t enter_speed;   /* RB_SPEED_KEEP = leave [bp+0x86] */
    uint16_t match_speed;
    uint16_t rewind_add;    /* 0 = do not touch [bp+0x7A] */
    uint8_t  enter_next;    /* RB_PHASE_KEEP = leave the phase */
    uint8_t  next_phase;
} RbPhaseRule;

typedef struct RbStageInfo {
    const RbSpawnDef  *spawns;
    const RbStageEvent *events;
    const RbCellWave  *waves;
    const RbPhaseRule *rules;
    uint16_t spawn_count;
    uint16_t event_count;
    uint16_t wave_count;
    uint8_t  rule_count;
    uint8_t  popup;         /* cell-edge object 0x40A3 */
} RbStageInfo;

extern const RbStageInfo rb_stages[RB_STAGE_COUNT];

#ifdef __cplusplus
}
#endif

#endif /* RUDEBREAKER_STAGE_DATA_H */
