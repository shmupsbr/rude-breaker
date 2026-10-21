/**
 * FPLAY.COM (INT 7Fh) run as a 16-bit COM image, with SONG.DAT at 0x4000
 * and the YM2608 ports it writes. The SDL2 mixer calls fplay_render.
 */
#ifndef RUDEBREAKER_FPLAY_H
#define RUDEBREAKER_FPLAY_H

#include <stddef.h>
#include <stdint.h>

int fplay_bind(const char *dir);
int fplay_bind_mem(const uint8_t *com, size_t com_n,
                   const uint8_t *song, size_t song_n,
                   const char *rhythm_dir);
int fplay_ready(void);
void fplay_command(uint8_t ah, uint8_t al);
/* Interleaved stereo S16. Silent when the driver is not bound. */
void fplay_render(int16_t *stereo, int frames);

#endif
