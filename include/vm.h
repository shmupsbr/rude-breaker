/**
 * Bytecode VM for SGM1 chunks 1-6.
 *
 * Entities are the 256 x 0x52 records dispatched by game image 0x1A30.
 * Script bytes live in the decompressed chunk. Native islands inside a
 * chunk (ship motion, movers, score, option followers) run when opcode
 * 0x0C calls them.
 */
#ifndef RUDEBREAKER_VM_H
#define RUDEBREAKER_VM_H

#include "hal.h"
#include "assets.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RbVm RbVm;

typedef struct RbVmInfo {
    uint16_t chunk;
    uint16_t cell;
    int16_t  speed;
    int16_t  scroll_y;
    int16_t  lives;
    int16_t  ship_x;
    int16_t  ship_y;
    uint32_t score;
    uint32_t hiscore;
    uint16_t alive;
    uint32_t spawns;
    uint32_t step_faults;
    uint32_t op_miss;
    uint32_t nat_miss;
    uint16_t miss_ip;
    uint8_t  miss_op;
    uint16_t nat_miss_ip;
    uint32_t aim_ops;
    uint32_t ship_natives;
    uint32_t option_natives;
    uint32_t loads;
    uint16_t load_record;
    uint16_t load_header;
} RbVmInfo;

typedef struct RbVmSprite {
    int16_t  x;
    int16_t  y;
    int16_t  w;
    int16_t  h;
    uint16_t type;
    uint16_t id;
    uint32_t rgba;
} RbVmSprite;

int  vm_create(RbVm **out, const RbAssets *assets);
void vm_destroy(RbVm *vm);

/* stage is 1..5 and selects SGM1 chunk (stage + 1). */
void vm_reset_play(RbVm *vm, uint16_t stage);
void vm_set_hiscore(RbVm *vm, uint32_t score);
void vm_set_input(RbVm *vm, const RbInput *in);
void vm_frame(RbVm *vm);

void vm_info(const RbVm *vm, RbVmInfo *out);
int  vm_count_origin(const RbVm *vm, uint16_t script_ip);
int  vm_collect_sprites(const RbVm *vm, RbVmSprite *out, int max);
int  vm_quit_requested(const RbVm *vm);
int  vm_boot_requested(const RbVm *vm);

/* Host checks: aim step, mover, delay, and a decompressed chunk. */
int  vm_self_test(RbVm *vm);

#ifdef __cplusplus
}
#endif

#endif /* RUDEBREAKER_VM_H */
