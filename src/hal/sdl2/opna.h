/**
 * YM2608 (PC-98 OPNA) for the SDL2 mixer.
 *
 * The core is Aaron Giles' ymfm, BSD-3-Clause, vendored under
 * third_party/ymfm. ADPCM-A rhythm samples come from an optional
 * external ROM. Without that file, those reads return 0.
 */
#ifndef RUDEBREAKER_OPNA_H
#define RUDEBREAKER_OPNA_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void opna_reset(void);
void opna_write(uint16_t port, uint8_t value);
/* 450 Hz lowpass on track 13's SSG tone channels 0 and 1. Noise and
 * every other track, including sound effects, stay unfiltered. */
void opna_melody_lpf(int enable);
/* One 44100 Hz stereo frame. FM is data[0]/data[1]; SSG is mixed from data[2].
 * ADPCM-A is already summed into the FM pair. */
void opna_sample(int16_t *stereo);

/* Full path from --ym2608-rom. Empty or NULL clears the override. */
void opna_set_rhythm_override(const char *path);
/* CLI override, else RUDEBR_YM2608_ROM, else beside the executable,
 * else ym2608_adpcm_rom.bin in data_dir. A missing file stays silent. */
void opna_autoload_rhythm(const char *data_dir);
int opna_rhythm_loaded(void);
uint32_t opna_adpcm_a_reads(void);
uint32_t opna_adpcm_b_reads(void);
/* Data writes to low-port addresses 0x10-0x1F, the ADPCM-A window. */
uint32_t opna_rhythm_port_writes(void);
void opna_adpcm_reads_reset(void);

#ifdef __cplusplus
}
#endif

#endif
