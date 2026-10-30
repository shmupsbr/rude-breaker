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
/* Enemy bullet colour preset: 0 ORIGINAL, 1 PINK, 2 LILAC, 3 HOT PINK. */
void gfx_set_hud_inset(int on);
void gfx_set_bullet_color(int preset);
/* One 16x16 sprite of a bound sheet as RGBA (0 = transparent), in the
 * palette and grade on screen now. Returns 0 when the slot is empty. */
int gfx_sprite_rgba(const RbGfx *gfx, uint8_t slot, int index, uint32_t out[256]);
/* 1 when the last present showed the settled title screen. */
int gfx_title_frame(void);
/* Host intro card before the Compile logo, t frames from its start
 * (t < 0: black). Particles gather into the logo, it holds, the two
 * credit lines slide up from below, it holds, then fades. */
#define CARD_FORM   40
#define CARD_HOLD1  18
#define CARD_SLIDE  16
#define CARD_HOLD2  46
#define CARD_OUT    10
#define CARD_TOTAL  (CARD_FORM + CARD_HOLD1 + 2 * CARD_SLIDE + CARD_HOLD2 + CARD_OUT)
void gfx_intro_card(uint32_t *rgba, int w, int h, int t, const uint32_t *title);
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
/* Host CONFIG menu: darken the graphics inside x0..x1, y0..y1 (pixels,
 * end exclusive) before the text layer is painted, so the menu text
 * does not sit on the title logo. on = 0 turns it off. */
/* Speed readout in the stage HUD: 0 off, 1..3 chevrons. */
void gfx_set_hud_speed(int level);
/* Host frame counter for the title effects (glow, PUSH TRIGGER). */
void gfx_set_clock(uint32_t frame);
/* 0..256: the 30TH ANNIVERSARY EDITION glow, once every 5 s. */
void gfx_halo(uint32_t *rgba, int w, int h, int x0, int y0, int x1, int y1, int amt);
int gfx_glow_level(uint32_t frame);
void gfx_set_panel(int on, int x0, int y0, int x1, int y1);
/* blank_sides forces the 80 px columns black. The stage playfield does
 * that; the logo, title, and menus are drawn across the full 640. */
void gfx_present(RbGfx *gfx, int scroll76, int scroll_speed,
                 const uint8_t *text, uint32_t *rgba, int w, int h,
                 int blank_sides);

#endif /* RUDEBREAKER_GFX_H */
