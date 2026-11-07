/**
 * Minimal PNG writer. Stored (uncompressed) deflate, RGB 8-bit.
 * Original code: no third-party encoder is compiled in.
 */
#ifndef RUDEBREAKER_PNGWRITE_H
#define RUDEBREAKER_PNGWRITE_H

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* pixels are 0xRRGGBBAA, row-major. Returns 0 on success. */
int rb_write_png(FILE *fp, const uint32_t *px, int w, int h);

/* Create <root>/screenshots on first call and write
 * RudeBreaker_YYYYMMDD_HHMMSS.png, or _2, _3, ... when that name
 * already exists. root is the game folder. Returns 0 on success. */
int rb_save_screenshot(const uint32_t *px, int w, int h, const char *root);

#ifdef __cplusplus
}
#endif

#endif
