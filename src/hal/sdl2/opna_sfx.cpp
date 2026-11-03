/* Second YM2203 for the sound-effect FPLAY instance (fplay_sfx.c). */
#define OPNA_INST   1
#define opna_reset  opna2_reset
#define opna_write  opna2_write
#define opna_sample opna2_sample
#include "opna_ymfm.cpp"
