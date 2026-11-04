#ifndef RB_CFILTER_H
#define RB_CFILTER_H

#include <stddef.h>
#include <stdint.h>

/* COLOR FILTER: an output-only recolour of the finished RGBA8888 frame
 * (0xRRGGBBAA). The game's own palette and art are not touched. */
enum {
    CF_ORIGINAL = 0,
    CF_GAMEBOY,
    CF_VIRTUALBOY,
    CF_C64,
    CF_GAMEGEAR,
    CF_PCENGINE,
    CF_ZX,
    CF_MSX2,
    CF_MD,
    CF_COUNT
};

const char *cfilter_name(int mode);   /* menu label */
const char *cfilter_key(int mode);    /* cfg value */
int cfilter_from_key(const char *s);  /* -1 when unknown */
/* Whole frame, w by h: palette plus the machine's colour-per-cell
 * limits (attribute clash) where the mode has them. */
void cfilter_apply(uint32_t *px, int w, int h, int mode);
/* Palette only, per pixel (for the side-panel frame, whose playfield
 * copy has already been through cfilter_apply). */
/* Sprite layer mask for the next cfilter_apply (w*h, 1 = sprite), or NULL. */
void cfilter_set_layer(const uint8_t *mask);
/* Background luma under sprite pixels for the next cfilter_apply, or NULL. */
void cfilter_set_bg(const uint8_t *luma);
void cfilter_set_kind(const uint8_t *kind);
void cfilter_set_player(const uint8_t *own, const uint8_t *aura, int aura_ink, int shot_ink);
/* Pre-grade luma of each shown pixel (0..255), so ship shading is the
 * art's own grey, not the lifted grade. NULL falls back to RGB luma. */
void cfilter_set_shade(const uint8_t *shade);
void cfilter_set_stage(int stage);
/* ZX: per shown pixel sprite family id (0 = scenery) and background
 * index, and the page scroll, so colours can follow the scenery. */
void cfilter_set_text(const uint8_t *mask, const uint32_t *pre);
void cfilter_set_world(const uint16_t *sid, const uint8_t *bgi, int scroll);
void cfilter_apply_flat(uint32_t *px, size_t n, int mode);

#endif
