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
/* SGM1 chunk 1 header 0: logo, title, and attract. */
void vm_reset_boot(RbVm *vm);
/* Wait counts at [bp+0x60]. Zero becomes one (the following vsync edge). */
unsigned vm_vsync_count(const RbVm *vm);
/* Clear the "header 0 was loaded" flag after the host has noticed it. */
void vm_ack_boot(RbVm *vm);
void vm_set_hiscore(RbVm *vm, uint32_t score);
/* RBRANK.DAT is 10 bytes of config plus 10 records of 22. A buffer
 * that is not exactly 230 bytes installs the loader's default table
 * (100000 down to 10000, C9 = 7, B2 = 2). */
void vm_rank_load(RbVm *vm, const uint8_t *bytes, size_t n);
void vm_rank_export(const RbVm *vm, uint8_t *out);
/* 1 once, after native 0x9213 registers a score outside training. */
int  vm_take_rank_commit(RbVm *vm);
/* Highest live entity hit-point word. */
uint16_t vm_peak_hp(const RbVm *vm);
void vm_set_input(RbVm *vm, const RbInput *in);
/* Host options. Training skips player-body damage and blocks the
 * ranking insert. The grade is the modern palette at present time.
 * Neither flag is cleared by a stage reset. */
void vm_set_training(RbVm *vm, int on);
int  vm_training(const RbVm *vm);
void vm_set_grade(RbVm *vm, int modern);
void vm_set_global_u8(RbVm *vm, uint8_t field, uint8_t value);
/* OR these PC-98 held bits into the edge history so a swallowed key
 * does not fire again on the next frame while the player is still
 * holding it. */
void vm_pretend_held(RbVm *vm, uint16_t game_bits);
/* One settings row, ASCII in the same 0x23 fold the menu uses for
 * OFF/ON. width pads with blanks; 0 writes the string as-is. */
void vm_text_write(RbVm *vm, int col, int row, const char *s, int width,
                   uint8_t attr);
/* Selected rows blink attribute 0x41. Put a row back to the resting
 * label and value colors when the cursor leaves it. */
void vm_text_settle_row(RbVm *vm, int row);
/* 1 when two JIS characters sit next to each other in the text layer. */
int  vm_text_has_jis(const RbVm *vm, uint16_t a, uint16_t b);
void vm_frame(RbVm *vm);

/* Keep the player ship (id 0x0012) above the hit that starts the
 * death script. The stage itself ends on [bp+0xCC], not on that death. */
void vm_sustain_ship(RbVm *vm);
uint8_t vm_global_u8(const RbVm *vm, uint8_t field);
uint16_t vm_global_u16(const RbVm *vm, uint8_t field);
/* Expanded playfield palette, red/green/blue. */
void vm_palette_rgb(const RbVm *vm, uint8_t rgb[16][3]);
/* Live entities whose draw byte [+0x4F] equals attr. */
int vm_count_attr(const RbVm *vm, uint8_t attr);
/* Current instruction of the first live entity spawned at origin, or 0. */
uint16_t vm_origin_ip(const RbVm *vm, uint16_t origin);
/* Print live entities grouped by spawn address. */
void vm_debug_origins(const RbVm *vm);

void vm_info(const RbVm *vm, RbVmInfo *out);
int  vm_count_origin(const RbVm *vm, uint16_t script_ip);
int  vm_collect_sprites(const RbVm *vm, RbVmSprite *out, int max);
int  vm_quit_requested(const RbVm *vm);
int  vm_boot_requested(const RbVm *vm);
/* 1 once, when this frame loaded the next stage (records 3..6). */
int  vm_take_stage_gap(RbVm *vm);

/* 1 once a stage script has filled or composed the playfield. */
int  vm_gfx_active(const RbVm *vm);
/* Write the 640x400 playfield. scroll is applied here. */
void vm_present(const RbVm *vm, uint32_t *rgba, int w, int h);

/* Host checks: aim step, mover, delay, and a decompressed chunk. */
int  vm_self_test(RbVm *vm);

#ifdef __cplusplus
}
#endif

#endif /* RUDEBREAKER_VM_H */
