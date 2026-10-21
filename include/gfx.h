/**
 * PC-98 playfield for the SDL2 build.
 *
 * The stage script binds SGM1 records with opcode 0x32. Tiles are 16x16,
 * three bit planes (blue, red, green), 96 bytes each. Sprites are 16x16,
 * 128 bytes: a mask plane and the same three color planes, expanded through
 * the GRCG read-modify-write sequence at game image 0x481B. The screen map
 * is the 30 by 25 word grid filled by 0x108E.
 */
#ifndef RUDEBREAKER_GFX_H
#define RUDEBREAKER_GFX_H

#include <stdint.h>

#define RB_GFX_W 640
#define RB_GFX_H 400

typedef struct RbGfx RbGfx;

RbGfx *gfx_create(void);
void   gfx_destroy(RbGfx *gfx);
void   gfx_reset(RbGfx *gfx);

/* Slot is the BP index from opcode 0x32 (0x16 map, 0x26 tiles, 0x1A+ sprites). */
void gfx_bind(RbGfx *gfx, uint8_t slot, const uint8_t *data, uint32_t len);

void gfx_fill(RbGfx *gfx, uint16_t di, uint16_t nbytes, uint16_t rows, uint8_t color);
void gfx_copy_row(RbGfx *gfx, uint16_t screen_row, uint16_t src_row);
void gfx_compose(RbGfx *gfx);

void gfx_draw_entities(RbGfx *gfx, const uint8_t *ent, int ent_n, int stride,
                       const uint8_t *const *chunk, const uint32_t *clen,
                       const uint8_t *cof, int scroll76);

void gfx_palette_abs(RbGfx *gfx, const uint8_t *p, uint32_t n);
void gfx_palette_delta(RbGfx *gfx, const uint8_t *p, uint32_t n);
void gfx_palette_step(RbGfx *gfx, uint8_t shift);

int  gfx_active(const RbGfx *gfx);
void gfx_present(const RbGfx *gfx, int scroll76, uint32_t *rgba, int w, int h);

#endif /* RUDEBREAKER_GFX_H */
