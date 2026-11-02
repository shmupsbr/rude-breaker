/**
 * SOUND CHANNELS: EXTRA. A second FPLAY instance on its own YM2203 that
 * plays only the sound effects, so they never take a channel from the
 * music the first instance is playing. Same code as fplay.c, renamed.
 */
#define fplay_bind      fplay2_bind
#define fplay_bind_mem  fplay2_bind_mem
#define fplay_ready     fplay2_ready
#define fplay_command   fplay2_command
#define fplay_render    fplay2_render
#define opna_reset      opna2_reset
#define opna_write      opna2_write
#define opna_sample     opna2_sample
#define FPLAY_SECOND    1
#include "fplay.c"
