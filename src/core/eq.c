/* Output profiles, the 10-band graphic EQ and software balance.
 * Peaking filters use the standard bilinear-transform biquad (one
 * octave wide per band); everything runs in float on the mixed
 * stereo stream. */
#include "eq.h"

#include <math.h>
#include <string.h>

static const float k_hz[EQ_BANDS] = {
    31.0f, 62.0f, 125.0f, 250.0f, 500.0f,
    1000.0f, 2000.0f, 4000.0f, 8000.0f, 16000.0f
};
static const char *const k_band[EQ_BANDS] = {
    "31", "62", "125", "250", "500", "1K", "2K", "4K", "8K", "16K"
};
static const char *const k_name[EQP_N] = {
    "FLAT", "HEADPHONES", "TV", "MONITOR", "STEREO SPEAKERS", "2.1",
    "LARGE SPEAKERS", "HOME THEATER", "LAPTOP", "SMALL SPEAKERS", "CUSTOM"
};
static const char *const k_key[EQP_N] = {
    "flat", "headphones", "tv", "monitor", "stereo_speakers", "2.1",
    "large_speakers", "home_theater", "laptop", "small_speakers", "custom"
};
/* dB per band, 31 Hz .. 16 kHz. */
static const int8_t k_gain[EQP_N - 1][EQ_BANDS] = {
    {  0,  0,  0, 0,  0, 0, 0,  0,  0,  0},   /* FLAT */
    {  0,  0,  0, 0,  0, 0, 0, -1, -2, -2},   /* HEADPHONES + crossfeed */
    { -6, -4, -2, 0,  1, 2, 3,  2,  0, -2},   /* TV: speech forward */
    { -2, -1,  0, 0,  0, 0, 0,  1,  1,  0},   /* MONITOR / desktop */
    {  1,  2,  1, 0, -1, 0, 0,  1,  2,  1},   /* STEREO SPEAKERS */
    {  3,  3,  1, 0,  0, 0, 0,  0,  0,  0},   /* 2.1 + mono bass */
    { -2, -1,  0, 0,  0, 0, 0,  0,  1,  2},   /* LARGE SPEAKERS */
    {  4,  3,  1, 0, -1, 0, 1,  1,  2,  1},   /* HOME THEATER */
    {-12,-12, -6, 0,  2, 2, 1,  1,  0, -2},   /* LAPTOP */
    {-12, -6, -1, 1,  1, 1, 0,  0,  0, -1},   /* SMALL SPEAKERS */
};

const char *eq_band_name(int band)
{
    return band >= 0 && band < EQ_BANDS ? k_band[band] : "";
}

const char *eq_profile_name(int p)
{
    return p >= 0 && p < EQP_N ? k_name[p] : k_name[0];
}

const char *eq_profile_key(int p)
{
    return p >= 0 && p < EQP_N ? k_key[p] : k_key[0];
}

int eq_profile_from_key(const char *s)
{
    int p;

    for (p = 0; s && p < EQP_N; p++) {
        if (strcmp(s, k_key[p]) == 0) {
            return p;
        }
    }
    return -1;
}

const int8_t *eq_profile_gains(int p, const int8_t *custom)
{
    if (p == EQP_CUSTOM && custom) {
        return custom;
    }
    return k_gain[p > 0 && p < EQP_N - 1 ? p : 0];
}

static void peaking(EqBiquad *f, float fs, float f0, float db)
{
    const float q = 1.41f;
    float a = powf(10.0f, db / 40.0f);
    float w = 6.2831853f * f0 / fs;
    float al = sinf(w) / (2.0f * q);
    float cw = cosf(w);
    float a0 = 1.0f + al / a;

    f->b0 = (1.0f + al * a) / a0;
    f->b1 = (-2.0f * cw) / a0;
    f->b2 = (1.0f - al * a) / a0;
    f->a1 = (-2.0f * cw) / a0;
    f->a2 = (1.0f - al / a) / a0;
}

static void lowpass(EqBiquad *f, float fs, float f0)
{
    float w = 6.2831853f * f0 / fs;
    float al = sinf(w) / (2.0f * 0.7071f);
    float cw = cosf(w);
    float a0 = 1.0f + al;

    f->b0 = (1.0f - cw) * 0.5f / a0;
    f->b1 = (1.0f - cw) / a0;
    f->b2 = f->b0;
    f->a1 = (-2.0f * cw) / a0;
    f->a2 = (1.0f - al) / a0;
}

void eq_design(EqChain *c, int rate, int profile, const int8_t *custom,
               int balance)
{
    const int8_t *g;
    float fs = (float)(rate > 0 ? rate : 44100);
    int i;
    int top = 0;

    memset(c, 0, sizeof *c);
    if (profile < 0 || profile >= EQP_N) {
        profile = EQP_FLAT;
    }
    if (balance < -EQ_BAL_MAX) balance = -EQ_BAL_MAX;
    if (balance > EQ_BAL_MAX) balance = EQ_BAL_MAX;
    g = eq_profile_gains(profile, custom);
    for (i = 0; i < EQ_BANDS; i++) {
        int db = g[i];

        if (db < -EQ_DB_MAX) db = -EQ_DB_MAX;
        if (db > EQ_DB_MAX) db = EQ_DB_MAX;
        if (db == 0 || k_hz[i] > fs * 0.45f) {
            continue;
        }
        if (db > top) {
            top = db;
        }
        peaking(&c->band[c->nb++], fs, k_hz[i], (float)db);
    }
    /* Headroom so a boost does not clip. */
    c->pre = powf(10.0f, (float)-top / 20.0f);
    c->gl = balance > 0 ? (float)(EQ_BAL_MAX - balance) / EQ_BAL_MAX : 1.0f;
    c->gr = balance < 0 ? (float)(EQ_BAL_MAX + balance) / EQ_BAL_MAX : 1.0f;
    if (profile == EQP_HEADPHONES) {
        c->crossfeed = 1;
        c->xf_a = 1.0f - expf(-6.2831853f * 700.0f / fs);
    }
    if (profile == EQP_21) {
        c->monobass = 1;
        lowpass(&c->lp, fs, 120.0f);
    }
    c->bypass = c->nb == 0 && !c->crossfeed && !c->monobass && balance == 0;
}

static float run1(const EqBiquad *f, float *z, float x)
{
    float y = f->b0 * x + z[0];

    z[0] = f->b1 * x - f->a1 * y + z[1];
    z[1] = f->b2 * x - f->a2 * y;
    return y;
}

void eq_run(EqChain *c, int16_t *s, int frames)
{
    int n;

    if (!c || c->bypass) {
        return;
    }
    for (n = 0; n < frames; n++) {
        float v[2];
        int ch;
        int b;

        v[0] = (float)s[2 * n];
        v[1] = (float)s[2 * n + 1];
        if (c->monobass) {
            float l0 = run1(&c->lp, c->lz[0], v[0]);
            float l1 = run1(&c->lp, c->lz[1], v[1]);
            float d = (l1 - l0) * 0.5f;

            v[0] += d;
            v[1] -= d;
        }
        if (c->crossfeed) {
            float l = v[0];
            float r = v[1];

            c->xf_l += (l - c->xf_l) * c->xf_a;
            c->xf_r += (r - c->xf_r) * c->xf_a;
            v[0] = (l + 0.35f * c->xf_r) * (1.0f / 1.35f);
            v[1] = (r + 0.35f * c->xf_l) * (1.0f / 1.35f);
        }
        for (ch = 0; ch < 2; ch++) {
            float x = v[ch] * c->pre;

            for (b = 0; b < c->nb; b++) {
                x = run1(&c->band[b], c->z[b][ch], x);
            }
            x *= ch ? c->gr : c->gl;
            s[2 * n + ch] = (int16_t)(x > 32767.0f ? 32767 : x < -32768.0f ? -32768 : (int)x);
        }
    }
}
