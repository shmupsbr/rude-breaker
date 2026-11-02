#ifndef RB_CRT_H
#define RB_CRT_H

#include <stdint.h>

/* CRT effects: output-only post-process of the playfield rectangle of
 * the finished RGBA8888 frame (0xRRGGBBAA). Game art is not touched.
 * Each effect is one bit of the mask. */
enum {
    CRT_SCANLINES = 0,
    CRT_MASK,
    CRT_CURVATURE,
    CRT_BLOOM,
    CRT_VIGNETTE,
    CRT_BADRF,
    CRT_COUNT
};

const char *crt_name(int fx);  /* menu label */
const char *crt_key(int fx);   /* cfg key */
/* Rect x0,y0,w,h of px (stride pitch); frame drives the animated ones. */
void crt_apply(uint32_t *px, int pitch, int x0, int y0, int w, int h,
               unsigned mask, uint32_t frame);

#endif
