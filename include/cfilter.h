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
void cfilter_apply_flat(uint32_t *px, size_t n, int mode);

#endif
