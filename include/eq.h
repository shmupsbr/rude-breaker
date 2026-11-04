#ifndef RB_EQ_H
#define RB_EQ_H

#include <stdint.h>

/* SOUND > BALANCE, OUTPUT PROFILE and CUSTOM EQ: host-side output
 * shaping after the chips are mixed. FLAT with the balance centred is a
 * bypass, so the samples are exactly what the mixer made. */
#define EQ_BANDS 10
#define EQ_DB_MAX 12
#define EQ_BAL_MAX 10

enum {
    EQP_FLAT = 0, EQP_HEADPHONES, EQP_TV, EQP_MONITOR, EQP_STEREO,
    EQP_21, EQP_LARGE, EQP_HT, EQP_LAPTOP, EQP_SMALL, EQP_CUSTOM, EQP_N
};

typedef struct {
    float b0, b1, b2, a1, a2;
} EqBiquad;

typedef struct {
    int bypass;
    int nb;                         /* active peaking bands */
    EqBiquad band[EQ_BANDS];
    float z[EQ_BANDS][2][2];        /* [band][channel][state] */
    float pre;                      /* linear headroom gain */
    float gl, gr;                   /* balance */
    int crossfeed;                  /* HEADPHONES */
    float xf_a, xf_l, xf_r;
    int monobass;                   /* 2.1: one bass channel below 120 Hz */
    EqBiquad lp;
    float lz[2][2];
} EqChain;

const char *eq_band_name(int band);          /* "31" .. "16K" */
const char *eq_profile_name(int p);          /* menu text */
const char *eq_profile_key(int p);           /* cfg text */
int eq_profile_from_key(const char *s);      /* -1 when unknown */
/* Gains in dB of profile p; CUSTOM returns custom. */
const int8_t *eq_profile_gains(int p, const int8_t *custom);
/* Builds the chain for the device rate. balance -10 (left) .. 10. */
void eq_design(EqChain *c, int rate, int profile, const int8_t *custom,
               int balance);
/* Interleaved stereo in place; nothing at all when c->bypass. */
void eq_run(EqChain *c, int16_t *s, int frames);

#endif
