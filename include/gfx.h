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

/* scroll_speed is [bp+0x82], the speed the next present copies into
 * [bp+0x84]. blank_sides is the stage window (chunks 2..6). */
void gfx_draw_entities(RbGfx *gfx, const uint8_t *ent, int ent_n, int stride,
                       const uint8_t *const *chunk, const uint32_t *clen,
                       const uint8_t *cof, int scroll76, int scroll_speed,
                       int blank_sides);

void gfx_palette_abs(RbGfx *gfx, const uint8_t *p, uint32_t n);
void gfx_palette_delta(RbGfx *gfx, const uint8_t *p, uint32_t n);
void gfx_palette_step(RbGfx *gfx, uint8_t shift);
/* 4-bit DAC sample expanded the way present does (level * 17).
 * Channel 0 is red, 1 green, 2 blue. This is the original PC-98
 * color, before the optional modern grade. */
void gfx_palette_rgb(const RbGfx *gfx, uint8_t rgb[16][3]);
/* 0 = original 4-bit lookup (level * 17). 1 = the measured modern
 * grade, applied to the same 4-bit sample so a fade still tracks. */
void gfx_set_grade(int modern);
/* Loader 0x6C0B. The analog DAC reads the palette; it does not
 * store the sampled level back into the fade words. */
void gfx_dac_flush(RbGfx *gfx);

/* User glyphs registered by opcode 0x33 (INT 18h AH=1Ah). Codes are 0x76xx. */
void gfx_define_glyph(RbGfx *gfx, uint16_t code, const uint8_t *bits, uint32_t n);
const uint8_t *gfx_bank_data(const RbGfx *gfx, uint8_t slot, uint32_t *len);

/* Opcode 0x31: four LZ planes, blue / red / green / intensity. */
void gfx_clear_frame(RbGfx *gfx);
void gfx_merge_plane(RbGfx *gfx, int plane, const uint8_t *src, uint32_t n);

/* kind 0 = tile (index*160, skip 32), kind 1 = sprite (index*128). */
void gfx_blit_grid(RbGfx *gfx, const uint8_t *bank, uint32_t blen,
                   const uint8_t *cells, uint32_t cell_n,
                   uint16_t di, int cols, int rows, int kind, int shift);
/* Opcode 0x3A / 0x3B. src_planes is BH, dst_planes is BL.
 * shift is the AL the loader's 0x7428 passes into the GRCG blit at
 * 0x7541. Zero with neither mask bit (or only one of them) is the
 * plain MOVSW copy. Any other combination is the masked RMW path. */
void gfx_blit_span(RbGfx *gfx, const uint8_t *src, uint32_t slen,
                   uint16_t di, uint16_t row_bytes, uint16_t rows,
                   uint8_t src_planes, uint8_t dst_planes, uint8_t shift);

int  gfx_active(const RbGfx *gfx);
/* blank_sides forces the 80 px columns black. The stage playfield does
 * that; the logo, title, and menus are drawn across the full 640. */
void gfx_present(RbGfx *gfx, int scroll76, int scroll_speed,
                 const uint8_t *text, uint32_t *rgba, int w, int h,
                 int blank_sides);

#endif /* RUDEBREAKER_GFX_H */
