/**
 * YM2203 (OPN, the PC-98 sound chip FPLAY.COM drives) for the SDL2 mixer.
 *
 * The core is Aaron Giles' ymfm, BSD-3-Clause, vendored under
 * third_party/ymfm.
 */
#ifndef RUDEBREAKER_OPNA_H
#define RUDEBREAKER_OPNA_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void opna_reset(void);
/* OUT to 0x188 (address) or 0x18A (data). Other ports are ignored. */
void opna_write(uint16_t port, uint8_t value);
/* One 44100 Hz stereo frame (same sample on both sides): FM plus SSG. */
void opna_sample(int16_t *stereo);

/* MUSIC CHIP / SFX CHIP (chipsynth.c). inst 0 = music chip, 1 = effects. */
enum { CHIPX_ORIGINAL, CHIPX_SID, CHIPX_GAMEBOY, CHIPX_PSG, CHIPX_PCENGINE, CHIPX_VBOY, CHIPX_ZX, CHIPX_MSX2, CHIPX_MD, CHIPX_COUNT };
void chipx_set_mode(int inst, int mode);
int chipx_mode(int inst);
void chipx_jingle(int inst);
void chipx_write(int inst, uint8_t addr, uint8_t value);
int16_t chipx_next(int inst);

#ifdef __cplusplus
}
#endif

#endif
