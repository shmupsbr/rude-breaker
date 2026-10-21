/**
 * YM2608 (PC-98 OPNA) for the SDL2 mixer.
 *
 * The core is Aaron Giles' ymfm, BSD-3-Clause, vendored under
 * third_party/ymfm. Rhythm ADPCM-A reads return 0: the Yamaha ROM is
 * not included.
 */
#ifndef RUDEBREAKER_OPNA_H
#define RUDEBREAKER_OPNA_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void opna_reset(void);
void opna_write(uint16_t port, uint8_t value);
/* One 44100 Hz stereo frame. FM is data[0]/data[1]; SSG is mixed from data[2]. */
void opna_sample(int16_t *stereo);

#ifdef __cplusplus
}
#endif

#endif
