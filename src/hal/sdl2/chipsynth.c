/**
 * MUSIC CHIP / SFX CHIP: a playful "downgrade" of the YM2203 output.
 *
 * The register writes FPLAY.COM sends to the YM2203 are mirrored here.
 * From them we read, for each FM and SSG channel, the note frequency,
 * key on/off and loudness (carrier TL plus a rough envelope), then hand
 * the most important notes to a small software model of another sound
 * chip with its own voice limit, waveforms, pitch grid and DAC depth:
 *   SID        3 voices: saw, pulse with PWM, triangle, noise, and a
 *              resonant low-pass filter.
 *   GAME BOY   2 pulse (12.5/25/50 %), 1 4-bit wave (bass), 1 noise,
 *              4-bit DAC per channel.
 *   PSG        SN76489: 3 squares (10-bit period, no deep bass) + noise,
 *              2 dB volume steps.
 *   PC ENGINE  6 channels of 32-step 5-bit wavetables, noise on demand.
 * The guitar patches get up to two extra chips of the same kind for
 * layered, detuned power chords, a pick, and an amp/cabinet stage.
 * This is for fun, not an emulation. All code here is original.
 */
#include "opna.h"

#include <math.h>
#include <stdint.h>
#include <string.h>

#define RATE      44100.0f
#define TICK      32            /* samples between voice re-allocation */
#define FM_FS     (3993600.0f / 144.0f)
#define SSG_CLK   (3993600.0f / 32.0f)   /* tone f = SSG_CLK / period */
#define MAXV      6
/* Mix: rest-of-song bus gain and FM-part (bass) trim per chip, set so
 * guitar / bass / drums sit at the original's balance (+-2 dB). */
#ifndef MIX_R
/* MEGA DRIVE mix (tuned by measurement) */
#ifndef MD_REST
#define MD_REST 3.0f
#endif
#ifndef MD_OUT
#define MD_OUT 1.15f
#endif
#ifndef MD_GTR
#define MD_GTR 0.5f
#endif
#ifndef MD_DRUM
#define MD_DRUM 2.8f
#endif
#ifndef MD_JINGLE
#define MD_JINGLE 0.9f
#endif
#ifndef MD_KM
#define MD_KM 1.0f
#endif
#ifndef MD_DRV
#define MD_DRV 2.0f
#endif
#define MIX_R  {0, 0, 2.85f, 1.95f, 2.88f, 3.13f, 3.13f, 2.88f, 3.0f, MD_REST}
#endif
#ifndef MIX_KB
#define MIX_KB {0, 0, 0.43f, 1.0f, 0.51f, 1.0f, 0.66f, 0.72f, 0.8f, 0.8f}
#endif
#ifndef MIX_KD
#define MIX_KD {0, 0, 1.0f, 0.74f, 0.80f, 0.62f, 0.60f, 0.77f, 0.8f, 0.8f}
#endif
#ifndef MIX_OUT
#define MIX_OUT {1, 1, 1.01f, 1.459f, 0.997f, 1.059f, 1.33f, 1.114f, 1.653f, MD_OUT}
#endif
#ifndef MIX_PRES
#define MIX_PRES {0.9f, 0.9f, 0.9f, 0.3f, 0.4f, 0.7f, 0.6f, 0.3f, 0.3f, 0.15f}
#endif
static const float g_mix_kd[] = MIX_KD;   /* drums (SSG/noise) trim */
static const float g_mix_out[] = MIX_OUT; /* loudness match (LUFS) */
static const float g_mix_pres[] = MIX_PRES; /* presence shelf amount */
static const float g_mix_r[] = MIX_R;
static const float g_mix_kb[] = MIX_KB;
#define MIX_REST(m) g_mix_r[(m) + 1]
/* guitar bus levels per chip */
#ifndef GTR_SID
#define GTR_SID   0.57f
#endif
/* dedicated guitar chips (1 = two chips in all: one guitar chip, and
 * any second guitar part plays on the main chip). VIRTUAL BOY and the
 * SID keep two guitar chips. */
/* MSX2 is PSG + SCC only (no FM); build with -DMSX_FM for the OPLL */
#ifndef MSX_FM
#define MSX_PSCC 1
#endif
#ifndef PSCC_KM
#define PSCC_KM 1.0f
#endif
#ifndef PSCC_KD
#define PSCC_KD 1.0f
#endif
#ifndef PSCC_BASS
#define PSCC_BASS 0.9f
#endif
#ifndef PSCC_DRUM
#define PSCC_DRUM 1.85f
#endif
#ifndef PSCC_GSAW
#define PSCC_GSAW 1.0f
#endif
#ifndef PSCC_GSQ
#define PSCC_GSQ 0.5f
#endif
#ifndef GTR_REST_ATT_PSG
#define GTR_REST_ATT_PSG 5
#endif
#ifndef GTR_REST_ATT_PCE
#define GTR_REST_ATT_PCE 12
#endif
#ifndef GTR_ON_REST
#define GTR_ON_REST 0.25f
#endif
#ifndef CHIPX_GTR_CHIPS
#define CHIPX_GTR_CHIPS 1
#endif
#ifndef GTR_GB
#define GTR_GB    0.32f
#endif
#ifndef GTR_PSG
#define GTR_PSG   0.32f
#endif
#ifndef GTR_VB
#define GTR_VB    0.32f
#endif
#ifndef GTR_ZX
#define GTR_ZX    0.32f
#endif
#ifndef GTR_MSX2
#define GTR_MSX2  0.32f
#endif
#ifndef GTR_DRV_MSX2
#define GTR_DRV_MSX2 1.8f
#endif
#ifndef GTR_PCE
#define GTR_PCE   0.32f
#endif

enum { SRC_NONE = -1 };

typedef struct {
    int keyed;
    int stage;          /* 0 off, 1 attack, 2 decay, 3 sustain, 4 release */
    float att;          /* envelope attenuation, dB */
} FmEnv;

typedef struct {
    int src;            /* source id: 0..2 FM, 3..5 SSG tone, 6..8 SSG noise,
                         * 9 pick noise burst */
    int dbl;            /* doubles another voice's source (power chord) */
    int bright;         /* bright FM patch: the guitar-like lead */
    float ratio;
    float phase;
    float phase2;
    float phase3;
    float ph[6];        /* extra layers on the 2nd/3rd chip (guitar) */
    float freq;
    float amp;
    uint32_t lfsr;
    float nacc;
    int nbit;
} Voice;

typedef struct {
    int mode;
    uint8_t reg[256];
    uint8_t addr;
    FmEnv fm[3];
    float ssg_env_t;
    int tick;
    Voice v[MAXV];
    /* sources, refreshed every TICK */
    float sf[10];
    float sa[10];
    int bright[3];
    int kon_age[3];     /* ticks since the last FM key-on */
    /* SID filter */
    float lp, bp;
    float lfo;
    float glp, gbp;
    float cab[6];
    float g2lp, g2bp;
    float g3lp, g3bp;
    float hs;
    /* SID guitar instruments: one whole SID per guitar channel */
    struct SidGtr {
        int src;            /* FM channel, -1 idle */
        int kon;            /* last key-on count seen */
        int frame;          /* 50 Hz player frames since the note */
        int fcnt;           /* samples to the next frame */
        uint32_t acc[3];    /* 24-bit phase accumulators */
        uint32_t lfsr;
        uint32_t nlast;
        uint16_t freq[3];   /* frequency registers */
        uint16_t pw[3];     /* 12-bit pulse width registers */
        uint8_t ctrl[3];    /* control registers */
        float env[3];       /* 0..255 */
        int est[3];         /* 0 off, 1 attack, 2 decay/sustain, 3 release */
        uint16_t cut;       /* 11-bit cutoff */
    int pwd;
        float lp, bp;
        float ph[6];        /* GB/PSG/PCE guitar channel phases */
        float cab[6];       /* own cabinet filter state */
    } sg[2];
    float ghp;
    float gbn;          /* GB noise shelf state */
    float lim;          /* bus limiter gain */
    int sid_nv;         /* voices left for the other parts */
    int kon_cnt[3];
} Chip;

static Chip g_c[2];

static int gtr_chips(int mode)
{
    return (mode == CHIPX_SID || mode == CHIPX_VBOY) ? 2 : CHIPX_GTR_CHIPS;
}
static void msx2_reset(int ci);

/* the extend (1UP) jingle plays about 0.9 dB down: ~2 s from its
 * trigger, with 10 ms ramps */
/* per chip: 0.9 (about -0.9 dB) of the level it had with three chips;
 * the two-chip arrangement plays the jingle's extra parts on the main
 * chip, much quieter, so those chips get that back first */
#ifndef JINGLE_GAIN
#define JINGLE_GAIN g_jingle_k[c->mode]
#endif
static const float g_jingle_k[CHIPX_COUNT] = {1.0f, 0.9f, 1.158f, 1.848f, 2.626f, 0.9f, 1.435f, 2.479f, MD_JINGLE};
static int g_jingle_left[2];
static float g_jingle_g[2] = {1.0f, 1.0f};

void chipx_jingle(int inst)
{
    g_jingle_left[inst & 1] = (int)(2.0f * RATE);
}

int chipx_mode(int inst)
{
    return g_c[inst & 1].mode;
}

void chipx_set_mode(int inst, int mode)
{
    Chip *c = &g_c[inst & 1];
    int i;

    if (mode < 0 || mode >= CHIPX_COUNT) {
        mode = CHIPX_ORIGINAL;
    }
    if (c->mode == mode) {
        return;
    }
    c->mode = mode;
    for (i = 0; i < MAXV; i++) {
        c->v[i].src = SRC_NONE;
        c->v[i].lfsr = 0x7FFFu;
    }
    c->lp = c->bp = 0.0f;
    c->sg[0].src = c->sg[1].src = -1;
    c->sid_nv = 3;
    msx2_reset(inst & 1);
    g_jingle_left[inst & 1] = 0;
    g_jingle_g[inst & 1] = 1.0f;
}

static float rate_time(int er)
{
    /* Seconds for a 96 dB sweep at OPN effective rate er (0..63). */
    if (er <= 0) {
        return 1e9f;
    }
    if (er > 62) {
        er = 62;
    }
    return 0.0045f * powf(2.0f, (float)(62 - er) / 4.0f);
}

void chipx_write(int inst, uint8_t a, uint8_t v)
{
    Chip *c = &g_c[inst & 1];

    c->reg[a] = v;
    if (a == 0x28) {
        int ch = v & 3;
        int on = (v >> 4) != 0;

        if (ch < 3) {
            FmEnv *e = &c->fm[ch];

            if (on && !e->keyed) {
                e->stage = 1;
                c->kon_age[ch] = 0;
                c->kon_cnt[ch]++;
                if (e->att > 96.0f || e->att < 0.0f) {
                    e->att = 96.0f;
                }
            } else if (!on && e->keyed && e->stage) {
                e->stage = 4;
            }
            e->keyed = on;
        }
    } else if (a == 0x0D) {
        c->ssg_env_t = 0.0f;
    }
}

/* Operator register offsets for op1..op4 (OPN slot order). */
static const int k_op[4] = {0x00, 0x08, 0x04, 0x0C};

static void fm_env_step(Chip *c, int ch, float dt)
{
    FmEnv *e = &c->fm[ch];
    int o = k_op[3] + ch;   /* carrier op4 drives the envelope */
    int ar = c->reg[0x50 + o] & 31;
    int d1r = c->reg[0x60 + o] & 31;
    int d2r = c->reg[0x70 + o] & 31;
    int sl = c->reg[0x80 + o] >> 4;
    int rr = c->reg[0x80 + o] & 15;
    float sld = sl == 15 ? 93.0f : (float)sl * 3.0f;

    switch (e->stage) {
    case 1:
        if (ar >= 30) {
            e->att = 0.0f;
        } else {
            e->att -= dt * 96.0f / (rate_time(ar * 2) * 0.25f);
        }
        if (e->att <= 0.0f) {
            e->att = 0.0f;
            e->stage = 2;
        }
        break;
    case 2:
        e->att += dt * 96.0f / rate_time(d1r * 2);
        if (e->att >= sld) {
            e->att = sld;
            e->stage = 3;
        }
        break;
    case 3:
        e->att += dt * 96.0f / rate_time(d2r * 2);
        break;
    case 4:
        e->att += dt * 96.0f / rate_time(rr * 4 + 2);
        break;
    default:
        e->att = 96.0f;
        break;
    }
    if (e->att >= 96.0f) {
        e->att = 96.0f;
        if (e->stage >= 3) {
            e->stage = 0;
        }
    }
}

static int ssg_env_level(Chip *c)
{
    int ep = c->reg[0x0B] | (c->reg[0x0C] << 8);
    int sh = c->reg[0x0D] & 15;
    float ramp = 16.0f * 256.0f * (float)(ep ? ep : 1) / (3993600.0f / 2.0f);
    int cyc = (int)(c->ssg_env_t / ramp);
    float pos = c->ssg_env_t / ramp - (float)cyc;
    int up = (sh & 4) != 0;
    int lvl;

    if (!(sh & 8)) {
        return cyc == 0 ? (up ? (int)(pos * 16) : 15 - (int)(pos * 16)) : 0;
    }
    if (sh & 1) {   /* hold */
        if (cyc == 0) {
            return up ? (int)(pos * 16) : 15 - (int)(pos * 16);
        }
        return (up ^ ((sh & 2) != 0)) ? 15 : 0;
    }
    if ((sh & 2) && (cyc & 1)) {
        up = !up;
    }
    lvl = up ? (int)(pos * 16) : 15 - (int)(pos * 16);
    return lvl;
}

static float db_amp(float db)
{
    return powf(10.0f, -db / 20.0f);
}

static void sources(Chip *c, float dt)
{
    int ch;
    int r7 = c->reg[7];
    int np = c->reg[6] & 31;

    for (ch = 0; ch < 3; ch++) {
        int fnum = ((c->reg[0xA4 + ch] & 7) << 8) | c->reg[0xA0 + ch];
        int blk = (c->reg[0xA4 + ch] >> 3) & 7;
        int alg = c->reg[0xB0 + ch] & 7;
        int tl = 127;
        int k;

        fm_env_step(c, ch, dt);
        for (k = 0; k < 4; k++) {
            int car = k == 3 || (alg == 7) || (alg >= 5 && k >= 1) || (alg == 4 && k == 1);
            int t = c->reg[0x40 + k_op[k] + ch] & 127;

            if (car && t < tl) {
                tl = t;
            }
        }
        {
            /* The loudest carrier's MUL sets the heard pitch. */
            int best = 3, k2, mul;

            for (k2 = 0; k2 < 4; k2++) {
                int car = k2 == 3 || (alg == 7) || (alg >= 5 && k2 >= 1) || (alg == 4 && k2 == 1);

                if (car && (c->reg[0x40 + k_op[k2] + ch] & 127) < (c->reg[0x40 + k_op[best] + ch] & 127)) {
                    best = k2;
                }
            }
            mul = c->reg[0x30 + k_op[best] + ch] & 15;
            c->sf[ch] = (float)fnum * ldexpf(1.0f, blk) * FM_FS / 1048576.0f
                        * (mul ? (float)mul : 0.5f);
        }
        {
            /* Heavy feedback or loud modulators on a stacked algorithm
             * give the distorted "guitar" patches: those always get a
             * voice and a harder timbre. */
            int fb = (c->reg[0xB0 + ch] >> 3) & 7;
            int modmin = 127;

            for (k = 0; k < 4; k++) {
                int car = k == 3 || (alg == 7) || (alg >= 5 && k >= 1) || (alg == 4 && k == 1);
                int t = c->reg[0x40 + k_op[k] + ch] & 127;

                if (!car && t < modmin) {
                    modmin = t;
                }
            }
            c->bright[ch] = alg <= 3 && fb >= 6 && modmin >= 8;
        }
        if (c->kon_age[ch] < 1000) {
            c->kon_age[ch]++;
        }
        {
            /* FM loudness: the carrier TL plus envelope, with a gentler
             * curve so a TL 18-30 lead is not buried under the SSG. */
            float raw = c->fm[ch].stage ? db_amp(c->fm[ch].att + 0.75f * (float)tl) : 0.0f;

            c->sa[ch] = raw > 0.0005f ? fminf(1.0f, 1.6f * sqrtf(raw)) : 0.0f;
        }
        if (c->sf[ch] < 20.0f || c->sf[ch] > 12000.0f) {
            c->sa[ch] = 0.0f;
        }
    }
    {
        float b = 0.0f;
        int k3;

        for (k3 = 0; k3 < 3; k3++) {
            float age = (float)c->kon_age[k3] * dt;

            if (c->bright[k3] && c->fm[k3].stage && age < 0.03f) {
                b = fmaxf(b, 0.55f * (1.0f - age / 0.03f));
            }
        }
        c->sf[9] = 7000.0f;
        c->sa[9] = b;
    }
    c->ssg_env_t += dt;
    for (ch = 0; ch < 3; ch++) {
        int per = ((c->reg[1 + 2 * ch] & 15) << 8) | c->reg[2 * ch];
        int lv = c->reg[8 + ch];
        float a;

        lv = (lv & 16) ? ssg_env_level(c) : (lv & 15);
        a = lv ? db_amp((float)(15 - lv) * 3.0f) * 0.8f : 0.0f;
        c->sf[3 + ch] = SSG_CLK / (float)(per ? per : 1);
        c->sa[3 + ch] = (!(r7 & (1 << ch)) && c->sf[3 + ch] < 12000.0f) ? a : 0.0f;
        c->sf[6 + ch] = SSG_CLK / (float)(np ? np : 1);
        c->sa[6 + ch] = !(r7 & (8 << ch)) ? a : 0.0f;
    }
}

/* Pick up to nt tonal sources (FM weighted over SSG) and the loudest
 * noise source. Voices that keep their source keep their slot. */
static float g_dbl_ratio = 1.5f;
static void double_free(Chip *c, int nt);

static void allocate(Chip *c, int nt, int noise_slot, int shared)
{
    int pick[MAXV];
    int np = 0;
    int used[9] = {0};
    int i, k;
    int nsrc = -1;
    float nbest = 0.02f;

    for (i = 0; i < MAXV; i++) {
        if (c->v[i].dbl) {
            c->v[i].dbl = 0;
            c->v[i].src = SRC_NONE;
        }
    }

    for (k = 6; k < 9; k++) {
        if (c->sa[k] > nbest) {
            nbest = c->sa[k];
            nsrc = k;
        }
    }
    /* SID: noise only steals a voice when it is loud. */
    if (shared && nsrc >= 0 && nbest < 0.15f) {
        nsrc = -1;
    }
    if (shared && nsrc >= 0) {
        nt--;
    }
    while (np < nt) {
        int best = -1;
        float bs = 0.004f;

        for (k = 0; k < 6; k++) {
            float s = c->sa[k] * (k < 3 ? (c->bright[k] ? 8.0f : 2.0f) : 1.0f);

            if (!used[k] && s > bs) {
                bs = s;
                best = k;
            }
        }
        if (best < 0) {
            break;
        }
        used[best] = 1;
        pick[np++] = best;
    }
    /* keep sticky slots */
    for (i = 0; i < nt; i++) {
        int s = c->v[i].src;

        if (s >= 0 && s < 6 && used[s] == 1 && !c->v[i].dbl) {
            used[s] = 2;
        } else {
            c->v[i].src = SRC_NONE;
        }
    }
    for (k = 0; k < np; k++) {
        if (used[pick[k]] == 2) {
            continue;
        }
        for (i = 0; i < nt; i++) {
            if (c->v[i].src == SRC_NONE) {
                c->v[i].src = pick[k];
                break;
            }
        }
    }
    if (noise_slot >= 0) {
        int ns = shared ? (nt < MAXV ? nt : MAXV - 1) : noise_slot;

        c->v[ns].src = nsrc >= 0 ? nsrc : SRC_NONE;
        /* Pick attack: a short noise burst on a guitar key-on. */
        if (!shared && c->v[ns].src == SRC_NONE && c->sa[9] > 0.0f) {
            c->v[ns].src = 9;
        }
    }
    double_free(c, nt);
}

/* Free tonal slots double a guitar voice (fifth or detune). */

static void double_free(Chip *c, int nt)
{
    int i, k;
    int done[3] = {0, 0, 0};

    for (i = 0; i < nt; i++) {
        if (c->v[i].src != SRC_NONE || c->v[i].dbl) {
            continue;
        }
        for (k = 0; k < nt; k++) {
            int s = c->v[k].src;

            if (k != i && !c->v[k].dbl && s >= 0 && s < 3 && c->bright[s] &&
                c->sa[s] > 0.0f && !done[s]) {
                c->v[i].src = s;
                c->v[i].dbl = 1;
                done[s] = 1;
                break;
            }
        }
    }
}

static float q(float v, float steps)
{
    return floorf(v * steps + 0.5f) / steps;
}

static void bind_voices(Chip *c)
{
    int i;

    for (i = 0; i < MAXV; i++) {
        Voice *v = &c->v[i];

        if (v->src < 0) {
            v->amp = 0.0f;
            continue;
        }
        v->bright = v->src < 3 && c->bright[v->src];
        v->ratio = !v->dbl ? 1.0f : g_dbl_ratio;
        v->freq = c->sf[v->src] * v->ratio;
        v->amp = c->sa[v->src] * (v->dbl ? 0.6f : 1.0f);
        if (v->src < 3 && !c->bright[v->src]) {
            v->amp *= g_mix_kb[c->mode + 1];
        } else if (v->src >= 3 && v->src < 9) {
            v->amp *= g_mix_kd[c->mode + 1];
        }
        else if (gtr_chips(c->mode) < 2 && c->mode != CHIPX_ORIGINAL) {
            /* a guitar part without its own chip plays on the main one */
            v->amp *= GTR_ON_REST;
        }
    }
}

static float noise_step(Voice *v, float rate, int short_mode)
{
    v->nacc += rate / RATE;
    while (v->nacc >= 1.0f) {
        uint32_t b = (v->lfsr ^ (v->lfsr >> 1)) & 1u;

        v->nacc -= 1.0f;
        v->lfsr = (v->lfsr >> 1) | (b << 14);
        if (short_mode) {
            v->lfsr = (v->lfsr & ~0x40u) | (b << 6);
        }
        if (!v->lfsr) {
            v->lfsr = 1;
        }
        v->nbit = (int)(v->lfsr & 1u);
    }
    return v->nbit ? 1.0f : -1.0f;
}

static float adv(Voice *v, float f)
{
    v->phase += f / RATE;
    v->phase -= floorf(v->phase);
    return v->phase;
}

/* ---- SID ---------------------------------------------------------- */
/* ---- shared distorted-guitar chain (SID and PC Engine) ----------- */
typedef struct { float b0, b1, b2, a1, a2; } BqC;

static BqC g_cab[3];
static int g_cab_ready;

static void bq_make(BqC *q_, int type, float fc, float qv, float gain_db)
{
    float w = 6.2831853f * fc / RATE;
    float cw = cosf(w), sw = sinf(w);
    float al = sw / (2.0f * qv);
    float A = powf(10.0f, gain_db / 40.0f);
    float b0, b1, b2, a0, a1, a2;

    if (type == 0) {            /* high-pass */
        b0 = (1.0f + cw) / 2.0f; b1 = -(1.0f + cw); b2 = b0;
        a0 = 1.0f + al; a1 = -2.0f * cw; a2 = 1.0f - al;
    } else if (type == 1) {     /* low-pass */
        b0 = (1.0f - cw) / 2.0f; b1 = 1.0f - cw; b2 = b0;
        a0 = 1.0f + al; a1 = -2.0f * cw; a2 = 1.0f - al;
    } else {                    /* peaking */
        b0 = 1.0f + al * A; b1 = -2.0f * cw; b2 = 1.0f - al * A;
        a0 = 1.0f + al / A; a1 = -2.0f * cw; a2 = 1.0f - al / A;
    }
    q_->b0 = b0 / a0; q_->b1 = b1 / a0; q_->b2 = b2 / a0;
    q_->a1 = a1 / a0; q_->a2 = a2 / a0;
}

static float bq_run(const BqC *q_, float *z, float x)
{
    float y = q_->b0 * x + z[0];

    z[0] = q_->b1 * x - q_->a1 * y + z[1];
    z[1] = q_->b2 * x - q_->a2 * y;
    return y;
}

/* Note envelope: < 3 ms attack, a fast palm-mute decay to a plateau. */
static float gtr_env(float age)
{
    if (age < 0.003f) {
        return age / 0.003f;
    }
    return 0.55f + 0.45f * expf(-(age - 0.003f) / 0.06f);
}

/* Pick transient: a click plus 8 ms of noise. */
static float gtr_pick(Voice *v, float age)
{
    float k = 0.0f;

    if (age < 0.0008f) {
        k = 0.9f;
    }
    if (age < 0.008f) {
        k += (1.0f - age / 0.008f) * 0.7f * noise_step(v, 11000.0f, 0);
    }
    return k;
}

/* Overdrive (asymmetric hard clip) and a 1x12 cabinet: cut < 100 Hz
 * and > 5 kHz, bump 1.8 kHz. */
static float gtr_ampz(float *z, float x, float drive);
static float gtr_amp(Chip *c, float x, float drive)
{
    return gtr_ampz(c->cab, x, drive);
}

static float gtr_ampz(float *z, float x, float drive)
{
    if (!g_cab_ready) {
        bq_make(&g_cab[0], 0, 100.0f, 0.707f, 0.0f);
        bq_make(&g_cab[1], 2, 1800.0f, 0.9f, 6.0f);
        bq_make(&g_cab[2], 1, 5000.0f, 0.707f, 0.0f);
        g_cab_ready = 1;
    }
    x = x * drive + 0.15f;
    x = x > 0.9f ? 0.9f : x < -0.7f ? -0.7f : x;     /* asymmetric clip */
    x = tanhf(1.5f * x) - 0.149f;
    x = bq_run(&g_cab[0], &z[0], x);
    x = bq_run(&g_cab[1], &z[2], x);
    x = bq_run(&g_cab[2], &z[4], x);
    return x;
}

/* ---- SID guitar instrument ----------------------------------------
 * One SID (3 voices + its filter) plays one guitar part, driven like a
 * 50 Hz music player would drive real registers:
 *   V1 main oscillator (articulation), V2 harmonic generator (hard
 *   sync or ring with V1), V3 pick transient (noise, first frame) then
 *   reinforcement (fifth or octave).
 * Oscillators are 24-bit accumulators at the PAL clock with 12-bit
 * waveforms; ADSR uses the datasheet rate times; the filter is one
 * 12 dB state-variable filter with the 11-bit cutoff and 4-bit
 * resonance. Hard restart: on each new note all three envelopes and
 * oscillators are reset (test bit) before the gate. */
#define SID_CLK 985248.0f

typedef struct {
    uint8_t wav[3];         /* control byte without gate, frame 1.. */
    uint8_t wav3_first;     /* V3 control byte on frame 0 (pick) */
    float ratio[3];         /* V1 = 1; V2/V3 frequency ratios */
    float v2_sweep;         /* V2 ratio added on frame 0, decays/frame */
    uint8_t ad[3], sr[3];   /* ADSR bytes as written to $05/$06 */
    uint16_t pw0, pw_lo, pw_hi;
    int16_t pw_step;        /* PWM table step per frame */
    uint8_t res_filt;       /* $D417: resonance << 4 | voice routing */
    uint8_t mode_vol;       /* $D418: filter mode | volume */
    uint16_t cut0, cut1;    /* cutoff sweep start -> end */
    int cut_frames;
    float vib_cents;        /* vibrato depth after 300 ms */
    float drive;            /* post amp (not SID hardware) */
} SidPatch;

static const SidPatch k_sid_patch[3] = {
    /* A: sync lead. V1 pulse PWM, V2 saw hard-synced to V1 sweeping
     * 3x -> 2x, V3 noise pick then pulse fifth. BP+LP, res 12. */
    {{0x40, 0x22, 0x40}, 0x80, {1.0f, 2.0f, 1.4983f}, 1.0f,
     {0x06, 0x06, 0x04}, {0xC1, 0xA1, 0x71},
     0x0800, 0x0500, 0x0B00, 0x40, 0xC7, 0x3F, 0x0500, 0x02C0, 8, 9.0f, 2.4f},
    /* B: power chord. V1 saw, V2 pulse on the fifth, V3 noise pick then
     * saw an octave up. LP, res 8. */
    {{0x20, 0x40, 0x20}, 0x80, {1.0f, 1.4983f, 2.0f}, 0.0f,
     {0x07, 0x07, 0x05}, {0xB1, 0x91, 0x81},
     0x0600, 0x0400, 0x0900, 0x30, 0x87, 0x1F, 0x0600, 0x0380, 10, 7.0f, 2.6f},
    /* C: ring grit. V1 pulse fixed 44 %, V2 triangle ring-modulated by
     * V1 a few cents sharp (growl), V3 noise pick then pulse an octave
     * up. BP, res 15, cutoff falling. */
    {{0x40, 0x14, 0x40}, 0x80, {1.0f, 1.0035f, 2.0f}, 0.0f,
     {0x06, 0x06, 0x04}, {0xC0, 0xC0, 0x80},
     0x0700, 0x0700, 0x0700, 0, 0xF3, 0x2F, 0x0480, 0x0280, 12, 6.0f, 2.8f},
};
#ifndef SID_PATCH
#define SID_PATCH 1
#endif
static const int g_sid_patch = SID_PATCH;

/* Datasheet attack times (ms) per rate nibble; decay/release are 3x. */
static const float k_sid_att_ms[16] = {
    2, 8, 16, 24, 38, 56, 68, 80, 100, 250, 500, 800, 1000, 3000, 5000, 8000
};

static uint16_t sid_freq_reg(float hz)
{
    float r = hz * 16777216.0f / SID_CLK;

    return (uint16_t)(r > 65535.0f ? 65535.0f : r < 0.0f ? 0.0f : r);
}

static void sidg_frame(Chip *c, struct SidGtr *g)
{
    const SidPatch *P = &k_sid_patch[g_sid_patch];
    float f = c->sf[g->src];
    float vib = 1.0f;
    int k;

    if (g->frame >= 15) {
        vib = powf(2.0f, P->vib_cents / 1200.0f *
                         sinf(6.2831853f * 5.5f * (float)(g->frame - 15) / 50.0f));
    }
    for (k = 0; k < 3; k++) {
        float r = P->ratio[k];

        if (k == 1 && P->v2_sweep != 0.0f) {
            r += P->v2_sweep * powf(0.6f, (float)g->frame);
        }
        g->freq[k] = sid_freq_reg(f * r * vib);
        g->ctrl[k] = (uint8_t)(P->wav[k] | 1);
    }
    if (g->frame == 0) {
        g->ctrl[2] = (uint8_t)(P->wav3_first | 1);
        g->freq[2] = 0xC000;            /* bright noise for the pick */
        for (k = 0; k < 3; k++) {
            g->pw[k] = P->pw0;
        }
        g->pwd = 1;
    } else if (P->pw_step) {
        for (k = 0; k < 3; k++) {
            /* PWM table: sweep and bounce between pw_lo and pw_hi */
            int w = (int)g->pw[k] + g->pwd * (k == 1 ? -P->pw_step : P->pw_step);

            if (w > P->pw_hi || w < P->pw_lo) {
                w = w > P->pw_hi ? P->pw_hi : P->pw_lo;
                if (k == 2) {
                    g->pwd = -g->pwd;
                }
            }
            g->pw[k] = (uint16_t)w;
        }
    }
    g->cut = g->frame >= P->cut_frames ? P->cut1 :
             (uint16_t)(P->cut0 + (int)(P->cut1 - P->cut0) * g->frame / P->cut_frames);
    if (!c->fm[g->src].keyed) {
        for (k = 0; k < 3; k++) {
            g->ctrl[k] &= 0xFE;
        }
    }
    g->frame++;
}

static float sidg_sample(Chip *c, struct SidGtr *g)
{
    const SidPatch *P = &k_sid_patch[g_sid_patch];
    float out[3];
    float filt = 0.0f, dry = 0.0f, hp, fc;
    uint32_t prev1 = g->acc[0];
    int k;

    if (g->src < 0) {
        return 0.0f;
    }
    if (--g->fcnt <= 0) {
        g->fcnt = (int)(RATE / 50.0f);
        sidg_frame(c, g);
    }
    for (k = 0; k < 3; k++) {
        uint32_t step = (uint32_t)((float)g->freq[k] * SID_CLK / RATE);
        uint32_t a;
        int w = g->ctrl[k];
        int v = 0, n = 0;

        g->acc[k] = (g->acc[k] + step) & 0xFFFFFFu;
        /* hard sync: V2 restarts when V1's accumulator wraps */
        if (k == 1 && (w & 0x02) && g->acc[0] < prev1) {
            g->acc[1] = 0;
        }
        a = g->acc[k];
        if (w & 0x80) {
            if ((a ^ g->nlast) & 0x080000u) {
                uint32_t b = ((g->lfsr >> 22) ^ (g->lfsr >> 17)) & 1u;

                g->lfsr = ((g->lfsr << 1) | b) & 0x7FFFFFu;
            }
            g->nlast = a;
            v = (int)((g->lfsr >> 11) & 0xFFFu);
            n = 1;
        } else {
            int saw = (int)(a >> 12);
            int pul = (int)(a >> 12) >= g->pw[k] ? 0xFFF : 0;
            uint32_t msb = a & 0x800000u;
            int tri;

            if ((w & 0x04) && (g->acc[0] & 0x800000u)) {
                msb ^= 0x800000u;           /* ring: V1's MSB flips V2 */
            }
            tri = (int)(((msb ? ~a : a) >> 11) & 0xFFFu);
            v = 0xFFF;
            if (w & 0x10) { v &= tri; n++; }
            if (w & 0x20) { v &= saw; n++; }
            if (w & 0x40) { v &= pul; n++; }
        }
        /* envelope (linear attack, exponential decay/release) */
        {
            int gate = w & 1;
            float att = k_sid_att_ms[P->ad[k] >> 4] * 0.001f;
            float dec = k_sid_att_ms[P->ad[k] & 15] * 0.003f;
            float sus = (float)((P->sr[k] >> 4) * 17);
            float rel = k_sid_att_ms[P->sr[k] & 15] * 0.003f;

            if (gate && g->est[k] == 0) {
                g->est[k] = 1;
            }
            if (!gate && g->est[k]) {
                g->est[k] = 3;
            }
            if (g->est[k] == 1) {
                g->env[k] += 255.0f / (att * RATE);
                if (g->env[k] >= 255.0f) {
                    g->env[k] = 255.0f;
                    g->est[k] = 2;
                }
            } else if (g->est[k] == 2) {
                g->env[k] = sus + (g->env[k] - sus) * expf(-4.0f / (dec * RATE));
            } else if (g->est[k] == 3) {
                g->env[k] *= expf(-4.0f / (rel * RATE));
                if (g->env[k] < 0.5f) {
                    g->env[k] = 0.0f;
                    g->est[k] = 0;
                }
            }
        }
        out[k] = n ? ((float)v - 2048.0f) / 2048.0f * floorf(g->env[k]) / 255.0f : 0.0f;
    }
    for (k = 0; k < 3; k++) {
        if (P->res_filt & (1 << k)) {
            filt += out[k];
        } else {
            dry += out[k];
        }
    }
    /* 12 dB SVF; cutoff ~ 30 Hz + 5.8 Hz per step (8580-like) */
    fc = 2.0f * sinf(3.14159265f * fminf(30.0f + 5.8f * (float)g->cut, 12000.0f) / RATE);
    {
        float qd = 1.4f - (float)(P->res_filt >> 4) / 15.0f * 1.2f;
        float y = 0.0f;

        g->lp += fc * g->bp;
        hp = filt - g->lp - qd * g->bp;
        g->bp += fc * hp;
        if (P->mode_vol & 0x10) y += g->lp;
        if (P->mode_vol & 0x20) y += g->bp;
        if (P->mode_vol & 0x40) y += hp;
        return (y + dry) * (float)(P->mode_vol & 15) / 15.0f;
    }
}

/* Give each guitar channel (bright, sounding) a whole SID, at most 2;
 * a new key-on is a hard restart. */
static void sidg_assign(Chip *c)
{
    int s, ch;

    for (s = 0; s < 2; s++) {
        struct SidGtr *g = &c->sg[s];

        if (g->src >= 0 && (!c->bright[g->src] || c->fm[g->src].stage == 0)) {
            g->src = -1;
        }
    }
    for (ch = 0; ch < 3; ch++) {
        int have = 0;

        if (!c->bright[ch] || c->sa[ch] <= 0.0f) {
            continue;
        }
        for (s = 0; s < 2; s++) {
            have |= c->sg[s].src == ch;
        }
        for (s = 0; s < gtr_chips(c->mode) && !have; s++) {
            if (c->sg[s].src < 0) {
                c->sg[s].src = ch;
                c->sg[s].kon = c->kon_cnt[ch] - 1;
                have = 1;
            }
        }
    }
    for (s = 0; s < 2; s++) {
        struct SidGtr *g = &c->sg[s];

        if (g->src >= 0 && g->kon != c->kon_cnt[g->src]) {
            int k;

            g->kon = c->kon_cnt[g->src];
            g->frame = 0;
            g->fcnt = 0;
            if (!g->lfsr) {
                g->lfsr = 0x7FFFF8u;
            }
            for (k = 0; k < 3; k++) {
                g->acc[k] = 0;
                g->env[k] = 0.0f;
                g->est[k] = 0;
            }
            for (k = 0; k < 6; k++) {
                g->ph[k] = 0.0f;
            }
        }
    }
}


/* Advance an extra layer oscillator at f Hz. */
static float lay(Voice *v, int k, float f)
{
    v->ph[k] += f / RATE;
    v->ph[k] -= floorf(v->ph[k]);
    return v->ph[k];
}

/* Guitar bus high-pass (~160 Hz) to leave room for the bass. */
static float gtr_hp(Chip *c, float x)
{
    float y = x - c->ghp;

    c->ghp += 0.0228f * y;
    return y;
}

static float sid_pulse(float p, float w)
{
    return p < w ? 1.0f : -1.0f;
}

static float sid_sample(Chip *c)
{
    float sum = 0.0f;
    float gsum = 0.0f;
    float gsum2 = 0.0f;
    float gsum3 = 0.0f;
    float lowest = 1e9f;
    int i, lo = -1;
    float hp;

    c->lfo += 0.6f / RATE;
    c->lfo -= floorf(c->lfo);
    for (i = 0; i < c->sid_nv; i++) {
        if (c->v[i].src >= 0 && c->v[i].src < 6 && c->v[i].amp > 0 && c->v[i].freq < lowest) {
            lowest = c->v[i].freq;
            lo = i;
        }
    }
    for (i = 0; i < c->sid_nv; i++) {
        Voice *v = &c->v[i];
        float a = q(v->amp, 255.0f);
        float p, s;

        if (a <= 0.0f || v->src < 0) {
            continue;
        }
        if (v->src >= 6) {
            s = noise_step(v, v->freq * 4.0f, 0);
        } else if (v->bright) {
            /* Guitar: a 40-50 % pulse (slight PWM) with a pulse
             * hard-synced at 2x and ring-modulated for grit, an instant
             * attack, a palm-mute decay and a pick. The second SID plays
             * the fifth and the octave (power chord). */
            float age = (float)c->kon_age[v->src] * (float)TICK / RATE;
            float w = 0.45f + 0.04f * sinf(6.2831853f * (c->lfo * 4.0f + 0.3f * (float)i));
            float ps, pu, sy, e;

            p = adv(v, v->freq);
            ps = p * 2.0f;
            ps -= floorf(ps);
            pu = sid_pulse(p, w);
            sy = sid_pulse(ps, 0.35f);
            s = pu + 0.35f * sy + 0.25f * pu * sy;
            e = gtr_env(age);
            gsum += (q(s, 2048.0f) * e + gtr_pick(v, age)) * a;
            if (!v->dbl) {
                float p5, p8;

                v->phase2 += v->freq * 1.4983f / RATE;
                v->phase2 -= floorf(v->phase2);
                v->phase3 += v->freq * 2.0035f / RATE;
                v->phase3 -= floorf(v->phase3);
                p5 = sid_pulse(v->phase2, w);
                p8 = sid_pulse(v->phase3, 0.5f);
                gsum2 += (0.6f * p5 + 0.4f * p8) * e * a;
                /* third SID: the root double-tracked a few cents off
                 * each way (hard-synced pulses), the chorus of two
                 * guitars, plus the pick on its noise voice. */
                {
                    float d1 = lay(v, 0, v->freq * 1.0041f);
                    float d2 = lay(v, 1, v->freq * 0.9962f);
                    float d1s = lay(v, 2, v->freq * 1.0041f * 2.0f);

                    gsum3 += (0.55f * sid_pulse(d1, 0.42f) + 0.25f * sid_pulse(d1s, 0.3f) +
                              0.5f * sid_pulse(d2, 0.47f)) * e * a;
                    gsum3 += 0.6f * gtr_pick(v, age) * a;
                }
            }
            continue;
        } else {
            p = adv(v, v->freq);
            if (i == lo && v->src < 3) {
                s = 2.0f * p - 1.0f;                       /* saw bass */
            } else if (v->src < 3) {
                float w = 0.3f + 0.18f * sinf(6.2831853f * (c->lfo + 0.33f * (float)i));

                s = p < w ? 1.0f : -1.0f;                  /* PWM pulse */
            } else {
                s = p < 0.5f ? 4.0f * p - 1.0f : 3.0f - 4.0f * p;   /* triangle */
            }
        }
        sum += q(s, 2048.0f) * a;                          /* 12-bit osc */
    }
    /* Chamberlin state-variable low-pass, about 2.4 kHz, some resonance. */
    {
        const float f = 0.34f;
        const float qd = 1.1f;

        c->lp += f * c->bp;
        hp = sum - c->lp - qd * c->bp;
        c->bp += f * hp;
    }
    /* Guitar bus: each SID's resonant filter around 1.2 - 2 kHz
     * (band-pass plus the dry signal), then overdrive and cabinet. */
    {
        float f = 0.11f + 0.04f * (0.5f + 0.5f * sinf(6.2831853f * c->lfo * 2.0f));
        float h, h2;

        c->glp += f * c->gbp;
        h = gsum - c->glp - 0.25f * c->gbp;
        c->gbp += f * h;
        c->g2lp += 0.13f * c->g2bp;
        h2 = gsum2 - c->g2lp - 0.3f * c->g2bp;
        c->g2bp += 0.13f * h2;
        c->g3lp += 0.12f * c->g3bp;
        h2 = gsum3 - c->g3lp - 0.35f * c->g3bp;
        c->g3bp += 0.12f * h2;
    }
    {
        /* guitar SIDs, then the amp/cabinet stage (post, not SID) */
        float gs = sidg_sample(c, &c->sg[0]) + sidg_sample(c, &c->sg[1]);

        return MIX_REST(CHIPX_SID) * (c->lp * 0.6f + sum * 0.45f) +
               GTR_SID * gtr_ampz(c->sg[0].cab, 0.55f * gtr_hp(c, gs), k_sid_patch[g_sid_patch].drive) +
               1.0f * gtr_amp(c, 0.5f * (gsum + gsum2 + gsum3) + 0.45f * (c->gbp + c->g2bp + c->g3bp), 2.6f);
    }
}


/* ---- GAME BOY ------------------------------------------------------ */
static const uint8_t k_gb_wave[32] = {
    0, 2, 4, 6, 8, 10, 12, 14, 15, 15, 15, 14, 13, 12, 11, 10,
    9, 8, 7, 6, 5, 4, 3, 2, 1, 1, 0, 0, 0, 0, 0, 0
};

static float gb_pitch(float f, float base)
{
    float x = 2048.0f - base / (f > 1.0f ? f : 1.0f);

    if (x < 0.0f) {
        x = 0.0f;
    }
    x = floorf(x + 0.5f);
    if (x > 2047.0f) {
        x = 2047.0f;
    }
    return base / (2048.0f - x);
}

static float gb_sample(Chip *c)
{
    /* slots 0,1 pulse, 2 wave, 3 noise. Wave takes the lowest note. */
    float sum = 0.0f;
    int i;

    for (i = 0; i < 4; i++) {
        Voice *v = &c->v[i];
        int vol = (int)floorf(v->amp * 15.0f + 0.5f);
        int out;
        float p;

        if (v->src < 0 || vol <= 0) {
            continue;
        }
        if (i == 3) {
            out = noise_step(v, v->freq * 2.0f, v->freq > 4000.0f) > 0 ? vol : 0;
        } else if (i == 2) {
            int wv = vol >= 12 ? 0 : vol >= 6 ? 1 : 2;   /* 100/50/25 % */

            p = adv(v, gb_pitch(v->freq, 65536.0f));
            out = k_gb_wave[(int)(p * 32.0f) & 31] >> wv;
        } else {
            static const float duty[2] = {0.5f, 0.25f};

            p = adv(v, gb_pitch(v->freq, 131072.0f));
            out = p < duty[i] ? vol : 0;
            if (v->bright) {
                out = p < (i == 0 ? 0.125f : 0.25f) ? vol : 0;   /* guitar */
            } else if (c->v[i].src >= 3 && c->v[i].src < 6) {
                out = p < 0.125f ? vol : 0;
            }
        }
        if (i == 3) {
            /* noise: gentle high shelf, about -2.5 dB above 6 kHz */
            float x = (float)out / 15.0f - 0.5f * (float)vol / 15.0f;

            c->gbn += 0.575f * (x - c->gbn);
            sum += c->gbn + 0.75f * (x - c->gbn);
            continue;
        }
        sum += (float)out / 15.0f - 0.5f * (float)vol / 15.0f;
    }
    return sum * 1.1f;
}

/* ---- SN76489 PSG --------------------------------------------------- */
static float psg_sample(Chip *c)
{
    float sum = 0.0f;
    int i;

    for (i = 0; i < 4; i++) {
        Voice *v = &c->v[i];
        float a;
        int att;

        if (v->src < 0 || v->amp <= 0.0f) {
            continue;
        }
        att = (int)floorf(-20.0f * log10f(v->amp) / 2.0f + 0.5f);
        if (v->bright && att > 2) {
            att = (v->dbl ? 3 : 1) + (gtr_chips(c->mode) < 2 ? GTR_REST_ATT_PSG : 0);   /* guitar: square near full volume */
        }
        if (att >= 15) {
            continue;
        }
        a = db_amp((float)att * 2.0f);
        if (i == 3) {
            sum += a * noise_step(v, v->freq * 2.0f, 0);
        } else {
            float n = floorf(3579545.0f / (32.0f * v->freq) + 0.5f);

            if (n > 1023.0f) {
                n = 1023.0f;   /* the PSG cannot go lower than ~109 Hz */
            }
            if (n < 1.0f) {
                n = 1.0f;
            }
            sum += a * (adv(v, 3579545.0f / (32.0f * n)) < 0.5f ? 1.0f : -1.0f);
            if (v->bright) {
                /* second PSG: the same square, detuned by one period
                 * step and 4 dB down, for a thicker guitar */
                float n2 = n > 2.0f ? n - 1.0f : n;

                v->phase3 += 3579545.0f / (32.0f * n2) / RATE;
                v->phase3 -= floorf(v->phase3);
                sum += db_amp((float)(att + 2) * 2.0f) * (v->phase3 < 0.5f ? 1.0f : -1.0f);
            }
        }
    }
    if (c->sa[9] > 0.0f && c->v[3].src != 9) {
        /* second PSG's noise channel: the pick, when the first one is
         * busy with drums */
        Voice *nv = &c->v[MAXV - 1];

        sum += c->sa[9] * 0.8f * noise_step(nv, 14000.0f, 1);
    }
    return sum * 0.7f;
}

/* ---- ZX SPECTRUM 128K: AY-3-8912 ---------------------------------
 * 1.7734 MHz, three squares (12-bit period, clock/16/N), one shared
 * 17-bit noise (5-bit period), 16 logarithmic volume steps and the
 * envelope generator (clock/256/EP), mono. */
#define AY_CLK 1773400.0f
static const float k_ay_vol[16] = {0.0f, 0.0137f, 0.0205f, 0.0291f, 0.0423f, 0.0618f, 0.0847f, 0.1369f,
                                   0.1691f, 0.2647f, 0.3527f, 0.4499f, 0.5704f, 0.6873f, 0.8482f, 1.0f};

static float ay_vq(float x)
{
    int k, b = 0;
    float bd = 9.0f;

    for (k = 0; k < 16; k++) {
        float d = fabsf(k_ay_vol[k] - x);

        if (d < bd) {
            bd = d;
            b = k;
        }
    }
    return k_ay_vol[b];
}

static float ay_f(float f)
{
    float n = floorf(AY_CLK / (16.0f * f) + 0.5f);

    n = n > 4095.0f ? 4095.0f : n < 1.0f ? 1.0f : n;
    return AY_CLK / (16.0f * n);
}

static float ay_noise(Voice *v, float rate)
{
    float np = floorf(AY_CLK / (16.0f * rate) + 0.5f);

    np = np < 1.0f ? 1.0f : np > 31.0f ? 31.0f : np;
    v->nacc += AY_CLK / (16.0f * np) / RATE;
    while (v->nacc >= 1.0f) {
        uint32_t b = (v->lfsr ^ (v->lfsr >> 3)) & 1u;

        v->nacc -= 1.0f;
        v->lfsr = (v->lfsr >> 1) | (b << 16);
        if (!v->lfsr) {
            v->lfsr = 1;
        }
        v->nbit = (int)(v->lfsr & 1u);
    }
    return v->nbit ? 1.0f : -1.0f;
}

/* envelope shape 8 (repeating saw down) as a waveform: the "buzzer" */
static float ay_saw(float p)
{
    return k_ay_vol[15 - ((int)(p * 16.0f) & 15)];
}

static float zx_sample(Chip *c)
{
    float sum = 0.0f;
    int i;

    for (i = 0; i < 4; i++) {
        Voice *v = &c->v[i];
        float a;

        if (v->src < 0 || v->amp <= 0.0f) {
            continue;
        }
        a = ay_vq(v->amp);
        if (a <= 0.0f) {
            continue;
        }
        if (i == 3) {
            sum += a * ay_noise(v, v->freq * 2.0f);
        } else {
            sum += a * (adv(v, ay_f(v->freq)) < 0.5f ? 1.0f : -1.0f);
        }
    }
    if (c->sa[9] > 0.0f && c->v[3].src != 9) {
        sum += ay_vq(c->sa[9] * 0.8f) * ay_noise(&c->v[MAXV - 1], 40000.0f);
    }
    return sum * 0.7f;
}

/* ---- MSX2: MSX-MUSIC (YM2413 OPLL) + PSG + Konami SCC ------------
 * Our own model of the three chips an MSX2 game with an FM-PAC and an
 * SCC cartridge would drive:
 *   OPLL   2-operator FM, 9 channels (or 6 + the 5 rhythm sounds),
 *          49.7 kHz internal rate, 9-bit F-number + 3-bit block pitch
 *          grid, log-sine / exponent tables, half-wave rectified
 *          waveforms (DM / DC), modulator feedback, total level in
 *          0.75 dB steps, channel volume in 3 dB steps. Only the user
 *          instrument is programmable; the voices below are patches we
 *          designed (no data from any real chip ROM or the PC-98 FM).
 *   PSG    AY-3-8910 at 1.7897725 MHz: squares, noise, 16 log levels.
 *   SCC    5 channels, 32-byte signed wavetables, 12-bit period,
 *          4-bit volume; channels 4 and 5 share one wave.
 * Arrangement: guitar = OPLL user patch (distorted) on the root and
 * the fifth, layered with an SCC saw; bass = OPLL; other FM parts =
 * OPLL brass; SSG parts = SCC (harmony / arps) and PSG; drums = OPLL
 * rhythm (BD / SD / HH) plus PSG noise. */
#define OPLL_FS (3579545.0f / 72.0f)
#define MSX_PSG_CLK 1789772.5f

typedef struct {
    uint8_t mul;    /* 0..15 */
    uint8_t tl;     /* modulator total level, 0.75 dB steps (0..63) */
    uint8_t fb;     /* modulator feedback 0..7 */
    uint8_t dm, dc; /* half-wave rectified modulator / carrier */
    uint8_t cmul;
} OpllPatch;

/* our user instrument: hard, bright distortion lead */
static const OpllPatch k_op_lead = {1, 11, 6, 1, 0, 1};
/* our bass: round, a little bite on the attack */
static const OpllPatch k_op_bass = {0, 20, 4, 0, 0, 1};
/* our brass for the other FM parts */
static const OpllPatch k_op_brass = {1, 22, 5, 0, 0, 1};

static int16_t g_opll_logsin[256];
static int16_t g_opll_exp[256];
static int g_opll_ready;
static int8_t g_scc_wave[4][32];

static void opll_init(void)
{
    int i;

    for (i = 0; i < 256; i++) {
        double s = sin((i + 0.5) * 3.14159265358979 / 512.0);

        g_opll_logsin[i] = (int16_t)floor(-log(s) / log(2.0) * 256.0 + 0.5);
        g_opll_exp[i] = (int16_t)floor((pow(2.0, i / 256.0) - 1.0) * 1024.0 + 0.5);
    }
    for (i = 0; i < 32; i++) {
        g_scc_wave[0][i] = (int8_t)(127 - i * 8);                         /* saw */
        g_scc_wave[1][i] = (int8_t)(i < 8 ? 112 : -112);                  /* 25 % pulse */
        g_scc_wave[2][i] = (int8_t)(i < 16 ? -120 + i * 15 : 120 - (i - 16) * 15);  /* triangle */
        g_scc_wave[3][i] = (int8_t)(i < 12 ? 100 : i < 16 ? -20 : -90);   /* brassy (ch4/5) */
    }
    g_opll_ready = 1;
}

/* one operator: 10-bit phase, attenuation in 1/256 log2 units;
 * returns about -4096..4096 */
static int opll_op(int ph10, int att, int half)
{
    int neg = ph10 & 512, i = ph10 & 255, l, v;

    if (ph10 & 256) {
        i = 255 - i;
    }
    if (neg && half) {
        return 0;
    }
    l = g_opll_logsin[i] + att;
    if (l >= 256 * 13) {
        return 0;
    }
    v = ((g_opll_exp[255 - (l & 255)] + 1024) << 2) >> (l >> 8);
    return neg ? -v : v;
}

/* the nearest pitch on the OPLL F-number / block grid */
static float opll_f(float f)
{
    int blk = 0;
    float fn;

    while (blk < 7 && f * 524288.0f / (OPLL_FS * (float)(1 << blk)) > 511.0f) {
        blk++;
    }
    fn = floorf(f * 524288.0f / (OPLL_FS * (float)(1 << blk)) + 0.5f);
    fn = fn < 1.0f ? 1.0f : fn > 511.0f ? 511.0f : fn;
    return fn * OPLL_FS * (float)(1 << blk) / 524288.0f;
}

static const float k_opll_mul[16] = {0.5f, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 12, 12, 15, 15};

/* channel volume: 4 bits, 3 dB steps */
static int opll_vol(float a)
{
    int v;

    if (a <= 0.0f) {
        return 15;
    }
    v = (int)floorf(-20.0f * log10f(a) / 3.0f + 0.5f);
    return v < 0 ? 0 : v > 15 ? 15 : v;
}

/* one 2-op channel. st: [0] mod phase, [1] car phase, [2],[3] feedback */
static float opll_ch(float *st, const OpllPatch *p, float f, int vol, float bright)
{
    float fq = opll_f(f);
    int m, cph, mtl;

    if (vol >= 15) {
        return 0.0f;
    }
    st[0] += fq * k_opll_mul[p->mul] / RATE;
    st[0] -= floorf(st[0]);
    st[1] += fq * k_opll_mul[p->cmul] / RATE;
    st[1] -= floorf(st[1]);
    mtl = p->tl * 32 - (int)(bright * 96.0f);    /* 0.75 dB = 32 units */
    mtl = mtl < 0 ? 0 : mtl;
    {
        int fbk = p->fb ? (int)((st[2] + st[3]) / (float)(1 << (9 - p->fb))) : 0;

        m = opll_op(((int)(st[0] * 1024.0f) + fbk) & 1023, mtl, p->dm);
    }
    st[3] = st[2];
    st[2] = (float)m;
    cph = ((int)(st[1] * 1024.0f) + (m >> 1)) & 1023;
    return (float)opll_op(cph, vol * 128, p->dc) / 4096.0f;
}

static float scc_f(float f)
{
    float n = floorf(3579545.0f / (32.0f * f) - 1.0f + 0.5f);

    n = n < 8.0f ? 8.0f : n > 4095.0f ? 4095.0f : n;
    return 3579545.0f / (32.0f * (n + 1.0f));
}

static float scc_ch(float *ph, int wave, float f, float a)
{
    int v = (int)floorf(a * 15.0f + 0.5f);

    if (v <= 0) {
        return 0.0f;
    }
    *ph += scc_f(f) / RATE;
    *ph -= floorf(*ph);
    return (float)g_scc_wave[wave][(int)(*ph * 32.0f) & 31] * (float)(v > 15 ? 15 : v) / (127.0f * 15.0f);
}

static float msx_psg_sq(float *ph, float f, float a)
{
    float n = floorf(MSX_PSG_CLK / (16.0f * f) + 0.5f);

    if (a <= 0.0f) {
        return 0.0f;
    }
    n = n > 4095.0f ? 4095.0f : n < 1.0f ? 1.0f : n;
    *ph += MSX_PSG_CLK / (16.0f * n) / RATE;
    *ph -= floorf(*ph);
    return ay_vq(a) * (*ph < 0.5f ? 1.0f : -1.0f);
}

/* rhythm section state per chip */
typedef struct {
    float bd_t, sd_t, hh_t;     /* seconds since trigger, <0 idle */
    float bd_ph, sd_ph;
    float bd_a, sd_a, hh_a;
    float prev_n[3];
    float prev_t[3];
    uint32_t lfsr;
    float st[MAXV][4];          /* OPLL channel state per voice */
    float scc_ph[MAXV];
    float lp, lp2;
} MsxState;
static MsxState g_mx[2];

static float msx_noise(uint32_t *l)
{
    uint32_t b = (*l ^ (*l >> 14)) & 1u;

    *l = (*l >> 1) | (b << 22);
    if (!*l) {
        *l = 1;
    }
    return (*l & 1u) ? 1.0f : -1.0f;
}

/* Watch the SSG drum parts and fire the OPLL rhythm sounds:
 * tone + noise low = BD, loud noise = SD, soft noise = HH. */
static void msx_rhythm_tick(Chip *c, int ci)
{
    int ch;

    for (ch = 0; ch < 3; ch++) {
        float n = c->sa[6 + ch], t = c->sa[3 + ch];

        if (n > 0.05f && n > 1.6f * g_mx[ci].prev_n[ch] + 0.02f) {
            if (t > 0.05f && c->sf[3 + ch] < 250.0f) {
                g_mx[ci].bd_t = 0.0f;
                g_mx[ci].bd_a = n;
            } else if (n > 0.35f) {
                g_mx[ci].sd_t = 0.0f;
                g_mx[ci].sd_a = n;
            } else {
                g_mx[ci].hh_t = 0.0f;
                g_mx[ci].hh_a = n;
            }
        }
        g_mx[ci].prev_n[ch] = n;
        g_mx[ci].prev_t[ch] = t;
    }
}

static float msx_rhythm(int ci)
{
    float x = 0.0f, dt = 1.0f / RATE;
    MsxState *r = &g_mx[ci];

#ifdef MSX_PSCC
    /* PSG only: kick = a falling square plus a click of noise, snare =
     * noise over a 190 Hz square, hats = noise, all on 4-bit volumes */
    if (r->bd_t >= 0.0f && r->bd_t < 0.3f) {
        float f = 50.0f + 130.0f * expf(-r->bd_t / 0.02f);

        x += 1.3f * msx_psg_sq(&r->bd_ph, f, r->bd_a * expf(-r->bd_t / 0.09f));
        if (r->bd_t < 0.012f) {
            x += 0.5f * msx_noise(&r->lfsr) * ay_vq(r->bd_a * 0.6f);
        }
        r->bd_t += dt;
    }
    if (r->sd_t >= 0.0f && r->sd_t < 0.25f) {
        float a = r->sd_a * expf(-r->sd_t / 0.06f);

        x += 0.75f * msx_noise(&r->lfsr) * ay_vq(a);
        x += 0.45f * msx_psg_sq(&r->sd_ph, 190.0f, a * expf(-r->sd_t / 0.03f));
        r->sd_t += dt;
    }
    if (r->hh_t >= 0.0f && r->hh_t < 0.1f) {
        x += 0.45f * msx_noise(&r->lfsr) * ay_vq(r->hh_a * expf(-r->hh_t / 0.025f));
        r->hh_t += dt;
    }
    return x;
#endif
    if (r->bd_t >= 0.0f && r->bd_t < 0.4f) {
        float f = 55.0f + 110.0f * expf(-r->bd_t / 0.025f);
        int vol = opll_vol(r->bd_a * expf(-r->bd_t / 0.11f));

        r->bd_ph += opll_f(f) / RATE;
        r->bd_ph -= floorf(r->bd_ph);
        x += 1.1f * (float)opll_op((int)(r->bd_ph * 1024.0f) & 1023, vol * 128, 0) / 4096.0f;
        r->bd_t += dt;
    }
    if (r->sd_t >= 0.0f && r->sd_t < 0.3f) {
        int vol = opll_vol(r->sd_a * expf(-r->sd_t / 0.07f));
        float nz = msx_noise(&r->lfsr);

        r->sd_ph += opll_f(190.0f) / RATE;
        r->sd_ph -= floorf(r->sd_ph);
        /* the OPLL snare: the phase's top bit gated by noise */
        x += 0.8f * (float)opll_op(((int)(r->sd_ph * 1024.0f) ^ (nz > 0 ? 256 : 0)) & 1023, vol * 128, 0) / 4096.0f;
        r->sd_t += dt;
    }
    if (r->hh_t >= 0.0f && r->hh_t < 0.12f) {
        int vol = opll_vol(r->hh_a * 1.2f * expf(-r->hh_t / 0.03f));

        x += 0.5f * msx_noise(&r->lfsr) * (vol >= 15 ? 0.0f : powf(10.0f, -(float)vol * 0.15f));
        r->hh_t += dt;
    }
    return x;
}

static void msx2_reset(int ci)
{
    memset(&g_mx[ci], 0, sizeof g_mx[ci]);
    g_mx[ci].bd_t = g_mx[ci].sd_t = g_mx[ci].hh_t = -1.0f;
    g_mx[ci].lfsr = 0x1234u;
}

static float msx2_sample(Chip *c)
{
    int ci = (int)(c - g_c), i, lo = -1;
    float sum = 0.0f, lf = 1e9f;

    if (!g_opll_ready) {
        opll_init();
    }
    /* lowest FM part = bass */
    for (i = 0; i < MAXV - 1; i++) {
        Voice *v = &c->v[i];

        if (v->src >= 0 && v->src < 3 && v->amp > 0.0f && !v->bright && v->freq < lf) {
            lf = v->freq;
            lo = i;
        }
    }
    for (i = 0; i < MAXV - 1; i++) {
        Voice *v = &c->v[i];

        if (v->src < 0 || v->amp <= 0.0f || v->src >= 6) {
            continue;
        }
#ifdef MSX_PSCC
        if (v->src < 3) {
            /* no FM: bass on an SCC "brassy" wave, the rest on SCC saw */
            sum += PSCC_KM * (i == lo ? PSCC_BASS : 0.6f) * scc_ch(&g_mx[ci].scc_ph[i], i == lo ? 3 : 0, v->freq, v->amp);
        } else
#endif
        if (v->src < 3) {
            /* FM parts: OPLL */
            sum += (i == lo ? 0.5f : 0.9f) * opll_ch(g_mx[ci].st[i], i == lo ? &k_op_bass : &k_op_brass, v->freq,
                                  opll_vol(v->amp), 0.0f);
        } else if (v->src == 3 || v->src == 5) {
            /* SSG parts: SCC harmony / arps (triangle and pulse) */
            sum += PSCC_KM * 0.8f * scc_ch(&g_mx[ci].scc_ph[i], v->src == 3 ? 1 : 2, v->freq, v->amp);
        } else {
            /* and one on the PSG */
            sum += PSCC_KM * 0.55f * msx_psg_sq(&g_mx[ci].scc_ph[i], v->freq, v->amp);
        }
    }
    {
        /* PSG noise under the rhythm sounds */
        Voice *nv = &c->v[MAXV - 1];

        if (nv->src >= 6 && nv->src < 9 && nv->amp > 0.0f) {
            float np = floorf(MSX_PSG_CLK / (16.0f * nv->freq * 2.0f) + 0.5f);

            np = np < 1.0f ? 1.0f : np > 31.0f ? 31.0f : np;
            sum += PSCC_KD * 0.47f * ay_vq(nv->amp) * noise_step(nv, MSX_PSG_CLK / (16.0f * np), 0);
        }
    }
    {
        /* the cartridge's analogue output filter (~6 kHz) on the drums */
        float d = msx_rhythm(ci);

        g_mx[ci].lp += 0.58f * (d - g_mx[ci].lp);
        g_mx[ci].lp2 += 0.58f * (g_mx[ci].lp - g_mx[ci].lp2);
        sum += PSCC_KD * PSCC_DRUM * g_mx[ci].lp2;
    }
    return sum * 0.7f;
}

/* guitar: OPLL user patch on the root and the fifth (two channels),
 * an SCC saw a few cents off on top, PSG noise for the pick */
static float msx2_gtr(Chip *c, struct SidGtr *g, float f, float e, int fr)
{
    int vol = opll_vol(e), vol5 = opll_vol(e * 0.7f);
    float x = 0.0f, br = fr < 4 ? 1.0f - 0.25f * (float)fr : 0.0f;
    float *st = g->ph;     /* ph[0..3] OPLL root, cab-free */
    static float st5[2][2][4];
    int gi = (int)(g - c->sg), ci = (int)(c - g_c);

    if (!g_opll_ready) {
        opll_init();
    }
#ifdef MSX_PSCC
    /* no FM: two SCC saws (root, fifth), a PSG square an octave down */
    (void)vol; (void)vol5; (void)st; (void)br;
    x += PSCC_GSAW * scc_ch(&g->ph[4], 0, f * 1.0035f, e);
    x += PSCC_GSAW * 0.7f * scc_ch(&st5[ci][gi][0], 0, f * 1.4983f, e * 0.8f);
    x += PSCC_GSQ * msx_psg_sq(&st5[ci][gi][1], f * 0.5f, e);
#else
    x += opll_ch(st, &k_op_lead, f, vol, br);
    x += opll_ch(st5[ci][gi], &k_op_lead, f * 1.4983f, vol5, br);
    x += 0.55f * scc_ch(&g->ph[4], 0, f * 1.0035f, e);
#endif
    if (fr < 2) {
        x += msx_noise(&g_mx[ci].lfsr) * ay_vq(fr ? 0.35f : 0.7f);
    }
    return x;
}

/* ---- PC ENGINE ----------------------------------------------------- */
static uint8_t g_pce_wave[5][32];
static int g_pce_ready;

static void pce_init(void)
{
    int i;

    for (i = 0; i < 32; i++) {
        float t = (float)i / 32.0f;
        float s0 = 2.0f * t - 1.0f;                                         /* saw */
        float s1 = sinf(6.2831853f * t) * 0.7f + sinf(12.566371f * t) * 0.3f; /* organ */
        float s2 = sinf(6.2831853f * t) + 0.35f * sinf(18.849556f * t);      /* brass */
        float s3 = t < 0.5f ? 1.0f : -1.0f;                                 /* square */

        g_pce_wave[0][i] = (uint8_t)floorf((s0 * 0.5f + 0.5f) * 31.0f + 0.5f);
        g_pce_wave[1][i] = (uint8_t)floorf((s1 * 0.5f + 0.5f) * 31.0f + 0.5f);
        g_pce_wave[2][i] = (uint8_t)floorf((s2 / 1.35f * 0.5f + 0.5f) * 31.0f + 0.5f);
        g_pce_wave[3][i] = (uint8_t)floorf((s3 * 0.5f + 0.5f) * 31.0f + 0.5f);
        {
            /* guitar: clipped saw + 25 % pulse */
            /* clipped square, steep edges, with a harmonic ripple */
            float s4 = 1.4f * ((t < 0.5f ? 1.0f : -1.0f) + 0.3f * sinf(6.2831853f * 6.0f * t) +
                               0.15f * sinf(6.2831853f * 3.0f * t));

            s4 = s4 > 1.0f ? 1.0f : s4 < -1.0f ? -1.0f : s4;
            g_pce_wave[4][i] = (uint8_t)floorf((s4 * 0.5f + 0.5f) * 31.0f + 0.5f);
        }
    }
    g_pce_ready = 1;
}

static float pce_sample(Chip *c)
{
    float sum = 0.0f;
    float gsum = 0.0f;
    int i;

    if (!g_pce_ready) {
        pce_init();
    }
    for (i = 0; i < 6; i++) {
        Voice *v = &c->v[i];
        int vol;
        float a;

        if (v->src < 0 || v->amp <= 0.0f) {
            continue;
        }
        /* 5-bit channel volume, 1.5 dB steps */
        vol = 31 - (int)floorf(-20.0f * log10f(v->amp) / 1.5f + 0.5f);
        if (v->bright) {
            vol = (v->dbl ? 29 : 31) - (gtr_chips(c->mode) < 2 ? GTR_REST_ATT_PCE : 0);   /* guitar: full volume */
        }
        if (vol <= 0) {
            continue;
        }
        a = db_amp((float)(31 - vol) * 1.5f);
        if (v->src >= 6) {
            sum += a * noise_step(v, v->freq * 3.0f, 0) * 0.8f;
        } else {
            float n = floorf(3579545.0f / (32.0f * v->freq) + 0.5f);
            int w = v->bright ? 4 : v->src < 3 ? v->src : 3;
            float p;

            if (n > 4095.0f) {
                n = 4095.0f;
            }
            if (n < 1.0f) {
                n = 1.0f;
            }
            p = adv(v, 3579545.0f / (32.0f * n));
            if (v->bright) {
                /* Guitar: per-note 5-bit volume envelope (fast attack,
                 * palm-mute decay), the pick on the noise mode, and the
                 * second PCE doubling detuned plus the fifth. */
                float age = (float)c->kon_age[v->src] * (float)TICK / RATE;
                float e = floorf(gtr_env(age) * 31.0f + 0.5f) / 31.0f;
                float g = a * e;

                gsum += g * ((float)g_pce_wave[4][(int)(p * 32.0f) & 31] / 15.5f - 1.0f);
                gsum += a * gtr_pick(v, age);
                if (!v->dbl) {
                    v->phase2 += 3579545.0f / (32.0f * n) * 1.0046f / RATE;
                    v->phase2 -= floorf(v->phase2);
                    v->phase3 += 3579545.0f / (32.0f * n) * 1.4983f / RATE;
                    v->phase3 -= floorf(v->phase3);
                    gsum += g * 0.8f * ((float)g_pce_wave[4][(int)(v->phase2 * 32.0f) & 31] / 15.5f - 1.0f);
                    gsum += g * 0.6f * ((float)g_pce_wave[4][(int)(v->phase3 * 32.0f) & 31] / 15.5f - 1.0f);
                }
            } else {
                sum += a * ((float)g_pce_wave[w][(int)(p * 32.0f) & 31] / 15.5f - 1.0f);
            }
        }
    }
    return sum * 0.75f + 1.1f * gtr_amp(c, 0.5f * gsum, 2.0f);
}


/* ---- VIRTUAL BOY (VSU) ----------------------------------------------
 * 5 wavetable channels (32 x 6-bit samples from 5 shared wave RAM
 * banks), channel 5 with sweep/modulation, channel 6 noise (15-bit
 * LFSR, selectable tap). 5 MHz clock: wave output is
 * 5e6 / ((2048 - F) * 32) with an 11-bit F, so nothing below ~76 Hz.
 * Each channel has 4-bit L/R levels and a 4-bit envelope; the output
 * step is (level * envelope >> 3) + 1. The game output is mono, so
 * the VSU runs with L = R. */
#define VSU_CLK 5000000.0f
static uint8_t g_vb_wave[5][32];
static int g_vb_ready;

static void vb_init(void)
{
    int i;

    for (i = 0; i < 32; i++) {
        float t = (float)i / 32.0f * 6.2831853f;
        float tri = i < 16 ? (float)i / 8.0f - 1.0f : 3.0f - (float)i / 8.0f;
        float sq = i < 16 ? 1.0f : -1.0f;
        float saw = (float)i / 16.0f - 1.0f;
        float cs = tanhf(2.5f * (sinf(t) + 0.3f * sinf(3.0f * t)));
        float bass = 0.75f * sinf(t) + 0.25f * tri;

        g_vb_wave[0][i] = (uint8_t)floorf((bass * 0.5f + 0.5f) * 63.0f + 0.5f);
        g_vb_wave[1][i] = (uint8_t)floorf((0.5f * sinf(t) + 0.25f * sinf(2.0f * t) + 0.5f) * 63.0f + 0.5f);
        g_vb_wave[2][i] = (uint8_t)floorf((sq * 0.45f + 0.5f) * 63.0f + 0.5f);
        g_vb_wave[3][i] = (uint8_t)floorf((cs / 1.05f * 0.5f + 0.5f) * 63.0f + 0.5f);   /* clipped square */
        g_vb_wave[4][i] = (uint8_t)floorf((tanhf(1.8f * saw) / 0.95f * 0.5f + 0.5f) * 63.0f + 0.5f); /* clipped saw */
    }
    g_vb_ready = 1;
}

static float vb_f(float f)
{
    float n;

    while (f > 0.0f && f < 77.0f) {
        f *= 2.0f;
    }
    n = floorf(2048.0f - VSU_CLK / (32.0f * f) + 0.5f);
    n = n < 0.0f ? 0.0f : n > 2047.0f ? 2047.0f : n;
    return VSU_CLK / ((2048.0f - n) * 32.0f);
}

/* (4-bit level * 4-bit envelope >> 3) + 1, out of 29 */
static float vb_lv(float amp)
{
    int l = (int)floorf(amp * 15.0f + 0.5f), e = 15;

    if (l <= 0) {
        return 0.0f;
    }
    l = l > 15 ? 15 : l;
    return (float)(((l * e) >> 3) + 1) / 29.0f;
}

static float vb_wav(int bank, float p)
{
    return (float)g_vb_wave[bank][(int)(p * 32.0f) & 31] / 31.5f - 1.0f;
}

static float vb_noise(Voice *v, float rate, int tap)
{
    static const int k_tap[8] = {14, 10, 13, 4, 8, 6, 9, 11};

    v->nacc += rate / RATE;
    while (v->nacc >= 1.0f) {
        uint32_t b = ((v->lfsr >> 7) ^ (v->lfsr >> k_tap[tap & 7])) & 1u;

        v->nacc -= 1.0f;
        v->lfsr = ((v->lfsr << 1) | b) & 0x7FFFu;
        if (!v->lfsr) {
            v->lfsr = 1;
        }
        v->nbit = (int)(b ^ 1u);
    }
    return v->nbit ? 1.0f : -1.0f;
}

static float vb_sample(Chip *c)
{
    float sum = 0.0f;
    int i;

    if (!g_vb_ready) {
        vb_init();
    }
    for (i = 0; i < 6; i++) {
        Voice *v = &c->v[i];
        float a;

        if (v->src < 0 || v->amp <= 0.0f) {
            continue;
        }
        a = vb_lv(v->amp);
        if (a <= 0.0f) {
            continue;
        }
        if (i == 5) {
            /* ch6: noise; rate from the 11-bit period (500 kHz base) */
            sum += a * vb_noise(v, fminf(v->freq * 3.0f, 250000.0f), 0) * 0.8f;
        } else {
            int w = v->src == 0 ? 3 : v->src < 3 ? (v->src == 2 ? 0 : 1) : 2;

            if (v->src < 3 && v->freq < 220.0f) {
                w = 0;          /* bass bank */
            }
            sum += a * vb_wav(w, adv(v, vb_f(v->freq)));
        }
    }
    return sum * 0.75f;
}

/* ch1-3 FM, ch4-5 the two loudest SSG tones, ch6 the loudest noise. */
static void vb_allocate(Chip *c)
{
    int i, t1 = -1, t2 = -1, nz = -1;

    for (i = 0; i < 3; i++) {
        c->v[i].src = i;
        c->v[i].dbl = 0;
    }
    for (i = 3; i < 6; i++) {
        if (c->sa[i] <= 0.0f) {
            continue;
        }
        if (t1 < 0 || c->sa[i] > c->sa[t1]) {
            t2 = t1;
            t1 = i;
        } else if (t2 < 0 || c->sa[i] > c->sa[t2]) {
            t2 = i;
        }
    }
    for (i = 6; i < 9; i++) {
        if (c->sa[i] > 0.0f && (nz < 0 || c->sa[i] > c->sa[nz])) {
            nz = i;
        }
    }
    c->v[3].src = t1 >= 0 ? t1 : SRC_NONE;
    c->v[4].src = t2 >= 0 ? t2 : SRC_NONE;
    c->v[5].src = nz >= 0 ? nz : SRC_NONE;
    for (i = 3; i < 6; i++) {
        c->v[i].dbl = 0;
    }
}

/* ---- GB / PSG / PCE guitar instruments -----------------------------
 * Same idea as the SID: one whole chip per guitar part, driven by a
 * 60 Hz frame table like a game's sound driver, hard restart (phase
 * and volume reset) on every note.
 *   GAME BOY  pulse 1 root (duty table 12.5 -> 25 %), pulse 2 the fifth
 *             (25 %), wave channel the octave below with a clipped
 *             4-bit wave, noise channel the pick (frames 0-1).
 *   PSG       tone 1 root, tone 2 the fifth, tone 3 the root one period
 *             step off (beating), noise the pick (frames 0-1).
 *   PC ENGINE ch1 root (clipped square), ch2 root a few cents flat
 *             (clipped saw), ch3 fifth, ch4 octave, ch5 noise pick.
 * Pitches go through each chip's own period grid; volumes through its
 * own volume steps. */
static const uint8_t k_gb_gwave[32] = {
    15, 15, 15, 15, 15, 14, 12, 10, 8, 6, 4, 2, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 3, 5, 7, 9, 11, 13, 15, 15, 15, 15
};

/* per-frame volume table (palm-mute: full, then a plateau) */
static float cg_vol(int frame)
{
    static const float t[6] = {1.0f, 1.0f, 0.93f, 0.86f, 0.8f, 0.76f};

    return frame < 6 ? t[frame] : 0.74f;
}

static float cg_phase(float *ph, float f)
{
    *ph += f / RATE;
    *ph -= floorf(*ph);
    return *ph;
}

static float gb_vq(float x)   /* 4-bit volume */
{
    return floorf(x * 15.0f + 0.5f) / 15.0f;
}

static float psg_vq(float x)  /* 2 dB steps */
{
    int att;

    if (x <= 0.0f) {
        return 0.0f;
    }
    att = (int)floorf(-20.0f * log10f(x) / 2.0f + 0.5f);
    return att >= 15 ? 0.0f : db_amp((float)att * 2.0f);
}

static float psg_f(float f)
{
    float n = floorf(3579545.0f / (32.0f * f) + 0.5f);

    n = n > 1023.0f ? 1023.0f : n < 1.0f ? 1.0f : n;
    return 3579545.0f / (32.0f * n);
}


/* ---- MEGA DRIVE (Model 1): YM2612 + SN76489 ------------------------
 * YM2612: 7.67 MHz / 144 = 53.27 kHz, six 4-operator FM channels,
 * 11-bit F-number + 3-bit block, 0.75 dB total-level steps, 9-bit DAC
 * (the Model 1 output grit). Channel 6 stays FM here (no DAC
 * samples). SN76489: three squares (10-bit period, 3.58 MHz / 32),
 * one noise, 2 dB volume steps. The patches are our own. */
#define MD_FS (7670453.0f / 144.0f)
typedef struct {
    int alg;            /* 0: 1>2>3>4 chain, 4: (1>2)+(3>4) */
    float mul[4];
    float idx[4];       /* modulation depth of ops 1..3 (radians) */
    float fb;           /* op1 self feedback */
    float dt;           /* op4 detune ratio */
} MdPatch;
static const MdPatch k_md_brass = {4, {1, 1, 2, 1}, {1.3f, 0, 0.9f, 0}, 0.6f, 1.0021f};
static const MdPatch k_md_bass = {4, {1, 1, 1, 0.5f}, {1.7f, 0, 0.7f, 0}, 0.9f, 1.0f};
/* the guitar runs on a YM2612 core: md_fm.cpp */
static float g_md_st[2][MAXV][6];
#ifndef MDFM_LV
#define MDFM_LV 5.4f
#endif
static float g_md_lp[2];

static float md_f(float f)
{
    int blk = 0;
    float fn;

    while (blk < 7 && f * 2097152.0f / (MD_FS * (float)(1 << blk)) > 2047.0f) {
        blk++;
    }
    fn = floorf(f * 2097152.0f / (MD_FS * (float)(1 << blk)) + 0.5f);
    fn = fn < 1.0f ? 1.0f : fn > 2047.0f ? 2047.0f : fn;
    return fn * MD_FS * (float)(1 << blk) / 2097152.0f;
}

/* carrier level: 7-bit TL, 0.75 dB per step */
static float md_tl(float a)
{
    int t;

    if (a <= 0.0f) {
        return 0.0f;
    }
    t = (int)floorf(-20.0f * log10f(a) / 0.75f + 0.5f);
    if (t < 0) {
        t = 0;
    }
    return t >= 127 ? 0.0f : db_amp((float)t * 0.75f);
}

/* one channel; st: op phases [0..3], op1 feedback history [4],[5] */
static float md_ch(float *st, const MdPatch *p, float f, float a, float bright)
{
    float fq = md_f(f), o1, o2, o3, o4, k = 6.2831853f, lv = md_tl(a);
    int i;

    if (lv <= 0.0f) {
        return 0.0f;
    }
    for (i = 0; i < 4; i++) {
        st[i] += fq * p->mul[i] * (i == 3 ? p->dt : 1.0f) / RATE;
        st[i] -= floorf(st[i]);
    }
    o1 = sinf(k * st[0] + p->fb * 0.5f * (st[4] + st[5]));
    st[5] = st[4];
    st[4] = o1;
    o2 = sinf(k * st[1] + (p->idx[0] + bright) * o1);
    if (p->alg == 0) {
        o3 = sinf(k * st[2] + p->idx[1] * o2);
        o4 = sinf(k * st[3] + p->idx[2] * o3);
        return lv * o4;
    }
    o3 = sinf(k * st[2]);
    o4 = sinf(k * st[3] + p->idx[2] * o3);
    return lv * 0.5f * (o2 + o4);
}

/* the 9-bit DAC */
static float md_dac(float x)
{
    x = x > 1.0f ? 1.0f : x < -1.0f ? -1.0f : x;
    return floorf(x * 255.0f + 0.5f) / 255.0f;
}

static float md_psg_sq(float *ph, float f, float a)
{
    float n = floorf(3579545.0f / (32.0f * f) + 0.5f);
    int att;

    if (a <= 0.0f) {
        return 0.0f;
    }
    att = (int)floorf(-20.0f * log10f(a) / 2.0f + 0.5f);
    if (att >= 15) {
        return 0.0f;
    }
    att = att < 0 ? 0 : att;
    n = n > 1023.0f ? 1023.0f : n < 1.0f ? 1.0f : n;
    *ph += 3579545.0f / (32.0f * n) / RATE;
    *ph -= floorf(*ph);
    return db_amp((float)att * 2.0f) * (*ph < 0.5f ? 1.0f : -1.0f);
}

static float md_sample(Chip *c)
{
    int ci = (int)(c - g_c), i, lo = -1;
    float fm = 0.0f, ps = 0.0f, lf = 1e9f, s;

    for (i = 0; i < MAXV - 1; i++) {
        Voice *v = &c->v[i];

        if (v->src >= 0 && v->src < 3 && v->amp > 0.0f && !v->bright && v->freq < lf) {
            lf = v->freq;
            lo = i;
        }
    }
    for (i = 0; i < MAXV - 1; i++) {
        Voice *v = &c->v[i];

        if (v->src < 0 || v->amp <= 0.0f || v->src >= 6) {
            continue;
        }
        if (v->src < 3) {
            fm += MD_KM * (i == lo ? 0.75f : 0.6f) * md_ch(g_md_st[ci][i], i == lo ? &k_md_bass : &k_md_brass, v->freq, v->amp, 0.0f);
        } else {
            ps += MD_KM * 0.55f * md_psg_sq(&g_md_st[ci][i][0], v->freq, v->amp);
        }
    }
    {
        /* SN76489 noise: the drums */
        Voice *nv = &c->v[MAXV - 1];

        if (nv->src >= 6 && nv->src < 9 && nv->amp > 0.0f) {
            int att = (int)floorf(-20.0f * log10f(nv->amp) / 2.0f + 0.5f);

            if (att < 15) {
                att = att < 0 ? 0 : att;
                ps += MD_DRUM * 0.6f * db_amp((float)att * 2.0f) * noise_step(nv, nv->freq * 2.0f, 0);
            }
        }
    }
    /* Model 1 output: FM through its 9-bit DAC, the PSG mixed in after,
     * then the board's soft low-pass */
    s = md_dac(fm * 0.5f) * 2.0f + ps;
    g_md_lp[ci] += 0.6f * (s - g_md_lp[ci]);
    return g_md_lp[ci];
}

/* guitar: a distorted YM2612 patch (four-op chain, heavy op1
 * feedback) on the root and the fifth, two FM channels, the 9-bit
 * DAC; PSG noise for the pick */
float mdfm_gtr(int idx, float f, float e, int fr, int keyed, float rate);

static float md_gtr(Chip *c, struct SidGtr *g, float f, float e, int fr)
{
    int gi = (int)(g - c->sg), ci = (int)(c - g_c);
    float x;

    x = mdfm_gtr(ci * 2 + gi, f, e, fr < 0 ? 0 : fr, c->fm[g->src].keyed, RATE) * MDFM_LV;
    if (fr < 2) {   /* pick */
        x += noise_step(&c->v[MAXV - 1], 14000.0f, 0) * db_amp(fr ? 8.0f : 2.0f) * 0.25f;
    }
    return x;
}

static float cg_sample(Chip *c, struct SidGtr *g)
{
    float f, x = 0.0f, e, vol;
    int fr;

    if (g->src < 0) {
        return 0.0f;
    }
    if (--g->fcnt <= 0) {
        g->fcnt = (int)(RATE / 60.0f);
        g->frame++;
    }
    fr = g->frame - 1;
    f = c->sf[g->src];
    vol = c->fm[g->src].keyed ? 1.0f : 0.0f;
    if (!vol) {
        g->env[0] *= 0.995f;    /* short release */
    } else {
        g->env[0] = cg_vol(fr < 0 ? 0 : fr);
    }
    e = g->env[0] * fminf(1.0f, c->sa[g->src] * 1.3f);
    if (e < 0.002f) {
        return 0.0f;
    }
    switch (c->mode) {
    case CHIPX_GAMEBOY: {
        float v = gb_vq(e);
        float d1 = fr < 3 ? 0.125f : 0.25f;
        float p;

        p = cg_phase(&g->ph[0], gb_pitch(f, 131072.0f));
        x += (p < d1 ? 1.0f : 0.0f) * v;
        p = cg_phase(&g->ph[1], gb_pitch(f * 1.4983f, 131072.0f));
        x += (p < 0.25f ? 1.0f : 0.0f) * gb_vq(e * 0.8f);
        p = cg_phase(&g->ph[2], gb_pitch(f * 0.5f, 65536.0f));
        x += (float)k_gb_gwave[(int)(p * 32.0f) & 31] / 15.0f * 0.9f;
        x -= 0.5f * (v + gb_vq(e * 0.8f) + 0.9f);
        if (fr < 2) {
            x += 0.6f * noise_step(&c->v[MAXV - 1], 9000.0f, 0) * gb_vq(1.0f - 0.5f * (float)fr);
        }
        break;
    }
    case CHIPX_PSG: {
        float p;
        float n0 = floorf(3579545.0f / (32.0f * f) + 0.5f);

        p = cg_phase(&g->ph[0], psg_f(f));
        x += (p < 0.5f ? 1.0f : -1.0f) * psg_vq(e);
        p = cg_phase(&g->ph[1], psg_f(f * 1.4983f));
        x += (p < 0.5f ? 1.0f : -1.0f) * psg_vq(e * 0.75f);
        n0 = n0 > 1022.0f ? 1022.0f : n0 < 2.0f ? 2.0f : n0;
        p = cg_phase(&g->ph[2], 3579545.0f / (32.0f * (n0 + 1.0f)));
        x += (p < 0.5f ? 1.0f : -1.0f) * psg_vq(e * 0.7f);
        if (fr < 2) {
            x += noise_step(&c->v[MAXV - 1], 14000.0f, 0) * psg_vq(fr ? 0.4f : 0.8f);
        }
        break;
    }
    case CHIPX_ZX: {
        /* AY guitar: A = envelope saw at the note (buzzer, ANDed with
         * its square), B = fifth, C = octave; noise pick. */
        float a = ay_vq(e), p, ep, en;

        en = floorf(AY_CLK / (256.0f * f) + 0.5f);
        en = en < 1.0f ? 1.0f : en;
        ep = AY_CLK / (256.0f * en);
        p = cg_phase(&g->ph[0], ep);
        x += a * (ay_saw(p) * 2.0f - 1.0f);
        p = cg_phase(&g->ph[1], ay_f(f * 1.4983f));
        x += (p < 0.5f ? 1.0f : -1.0f) * ay_vq(e * 0.75f);
        p = cg_phase(&g->ph[2], ay_f(f * 2.0f));
        x += (p < 0.5f ? 1.0f : -1.0f) * ay_vq(e * 0.45f);
        if (fr < 2) {
            x += ay_noise(&c->v[MAXV - 1], 60000.0f) * ay_vq(fr ? 0.4f : 0.8f);
        }
        break;
    }
    case CHIPX_MSX2:
        x = msx2_gtr(c, g, f, e, fr);
        break;
    case CHIPX_MD:
        x = md_gtr(c, g, f, e, fr);
        break;
    case CHIPX_PCENGINE: {
        float n = floorf(3579545.0f / (32.0f * f) + 0.5f), pf, p;
        float a = floorf(e * 31.0f + 0.5f) / 31.0f;

        n = n > 4095.0f ? 4095.0f : n < 1.0f ? 1.0f : n;
        pf = 3579545.0f / (32.0f * n);
        p = cg_phase(&g->ph[0], pf);
        x += a * ((float)g_pce_wave[4][(int)(p * 32.0f) & 31] / 15.5f - 1.0f);
        p = cg_phase(&g->ph[1], pf * 0.9958f);
        x += a * 0.8f * ((float)g_pce_wave[0][(int)(p * 32.0f) & 31] / 15.5f - 1.0f);
        p = cg_phase(&g->ph[2], pf * 1.4983f);
        x += a * 0.6f * ((float)g_pce_wave[4][(int)(p * 32.0f) & 31] / 15.5f - 1.0f);
        p = cg_phase(&g->ph[3], pf * 2.0f);
        x += a * 0.4f * ((float)g_pce_wave[4][(int)(p * 32.0f) & 31] / 15.5f - 1.0f);
        if (fr < 2) {
            x += (fr ? 0.35f : 0.7f) * noise_step(&c->v[MAXV - 1], 12000.0f, 0);
        }
        break;
    }
    case CHIPX_VBOY: {
        /* ch1 root clipped square, ch2 root -7 cents clipped saw, ch3
         * fifth, ch4 octave, ch5 root with the sweep unit bending the
         * attack down into pitch and then the modulator's vibrato, ch6
         * noise pick (tap 0 = long, hissy). */
        float a = vb_lv(e) * 1.1f, p, fs;

        if (!g_vb_ready) {
            vb_init();
        }
        p = cg_phase(&g->ph[0], vb_f(f));
        x += a * vb_wav(3, p);
        p = cg_phase(&g->ph[1], vb_f(f * 0.9960f));
        x += a * 0.8f * vb_wav(4, p);
        p = cg_phase(&g->ph[2], vb_f(f * 1.4983f));
        x += a * 0.6f * vb_wav(3, p);
        p = cg_phase(&g->ph[3], vb_f(f * 2.0f));
        x += a * 0.35f * vb_wav(4, p);
        fs = fr < 2 ? f * (1.03f - 0.015f * (float)fr) :
             fr > 15 ? f * (1.0f + 0.004f * sinf((float)fr * 0.9f)) : f;
        p = cg_phase(&g->ph[4], vb_f(fs));
        x += a * 0.4f * vb_wav(4, p);
        if (fr < 2) {
            x += (fr ? 0.35f : 0.7f) * vb_noise(&c->v[MAXV - 1], 120000.0f, 0);
        }
        break;
    }
    default:
        break;
    }
    return x;
}

/* PCE maps each source to its own channel (6 voices, 6 sources). */
static void pce_allocate(Chip *c)
{
    int i;

    for (i = 0; i < 6; i++) {
        int t = i;

        if (i >= 3 && c->sa[i] <= 0.0f && c->sa[i + 3] > 0.0f) {
            t = i + 3;   /* SSG channel playing only noise */
        }
        c->v[i].src = t;
        c->v[i].dbl = 0;
    }
    /* idle SSG channels double the guitar a fifth up */
    for (i = 3; i < 6; i++) {
        int k;

        if (c->sa[c->v[i].src] > 0.0f) {
            continue;
        }
        for (k = 0; k < 3; k++) {
            int used = 0, j;

            for (j = 3; j < 6; j++) {
                used |= c->v[j].dbl && c->v[j].src == k;
            }
            if (c->bright[k] && c->sa[k] > 0.0f && !used) {
                c->v[i].src = k;
                c->v[i].dbl = 1;
                break;
            }
        }
    }
}

int16_t chipx_next(int inst)
{
    Chip *c = &g_c[inst & 1];
    float s = 0.0f;
    float out;

    if (c->tick <= 0) {
        c->tick = TICK;
        sources(c, (float)TICK / RATE);
        g_dbl_ratio = c->mode == CHIPX_GAMEBOY ? 1.006f : 1.5f;
        if (c->mode == CHIPX_MSX2) {
            msx_rhythm_tick(c, (int)(c - g_c));
        }
        switch (c->mode) {
        case CHIPX_SID:
        case CHIPX_GAMEBOY:
        case CHIPX_PSG:
        case CHIPX_PCENGINE:
        case CHIPX_ZX:
        case CHIPX_MSX2:
        case CHIPX_MD:
        case CHIPX_VBOY: {
            /* Up to three chips: each guitar part owns a whole chip,
             * the rest of the song shares the others. */
            float keep[3];
            int s2, ng = 0, i2;

            sidg_assign(c);
            for (s2 = 0; s2 < 3; s2++) {
                keep[s2] = c->sa[s2];
            }
            for (s2 = 0; s2 < 2; s2++) {
                if (c->sg[s2].src >= 0) {
                    c->sa[c->sg[s2].src] = 0.0f;
                    ng++;
                }
            }
            if (c->mode == CHIPX_SID) {
                c->sid_nv = 3 * (3 - ng) > MAXV ? MAXV : 3 * (3 - ng);
                allocate(c, c->sid_nv, c->sid_nv - 1, 1);
            } else if (c->mode == CHIPX_PCENGINE) {
                pce_allocate(c);
            } else if (c->mode == CHIPX_VBOY) {
                vb_allocate(c);
            } else if (c->mode == CHIPX_MSX2 || c->mode == CHIPX_MD) {
                allocate(c, 5, 5, 0);
            } else {
                allocate(c, 3, 3, 0);
            }
            for (i2 = 0; i2 < MAXV; i2++) {
                for (s2 = 0; s2 < 2; s2++) {
                    if (c->sg[s2].src >= 0 && c->v[i2].src == c->sg[s2].src) {
                        c->v[i2].src = SRC_NONE;
                        c->v[i2].dbl = 0;
                    }
                }
            }
            for (s2 = 0; s2 < 3; s2++) {
                c->sa[s2] = keep[s2];
            }
            break;
        }
        default: break;
        }
        if (c->mode == CHIPX_GAMEBOY) {
            /* the wave channel (slot 2) gets the lowest note */
            int i, lo = -1;
            float lf = 1e9f;

            for (i = 0; i < 3; i++) {
                if (c->v[i].src >= 0 && c->sf[c->v[i].src] < lf && c->sa[c->v[i].src] > 0.0f) {
                    lf = c->sf[c->v[i].src];
                    lo = i;
                }
            }
            if (lo >= 0 && lo != 2 && !c->v[lo].bright && !(c->v[lo].src < 3 && c->v[lo].src >= 0 && c->bright[c->v[lo].src])) {
                Voice t = c->v[2];

                c->v[2] = c->v[lo];
                c->v[lo] = t;
            }
        }
        bind_voices(c);
    }
    c->tick--;
    switch (c->mode) {
    case CHIPX_SID: s = sid_sample(c) * 0.42f; break;
    case CHIPX_GAMEBOY: s = gb_sample(c) * 0.62f * MIX_REST(CHIPX_GAMEBOY); break;
    case CHIPX_PSG: s = psg_sample(c) * 0.30f * MIX_REST(CHIPX_PSG); break;
    case CHIPX_PCENGINE: s = pce_sample(c) * 0.28f * MIX_REST(CHIPX_PCENGINE); break;
    case CHIPX_VBOY: s = vb_sample(c) * 0.28f * MIX_REST(CHIPX_VBOY); break;
    case CHIPX_ZX: s = zx_sample(c) * 0.30f * MIX_REST(CHIPX_ZX); break;
    case CHIPX_MSX2: s = msx2_sample(c) * 0.30f * MIX_REST(CHIPX_MSX2); break;
    case CHIPX_MD: s = md_sample(c) * 0.30f * MIX_REST(CHIPX_MD); break;
    default: break;
    }
    if (c->mode == CHIPX_GAMEBOY || c->mode == CHIPX_PSG || c->mode == CHIPX_PCENGINE ||
        c->mode == CHIPX_VBOY || c->mode == CHIPX_ZX || c->mode == CHIPX_MSX2 || c->mode == CHIPX_MD) {
        /* guitar chips, then amp and cabinet */
        static const float drive[CHIPX_COUNT] = {0, 0, 2.4f, 2.4f, 2.4f, 2.4f, 2.4f, GTR_DRV_MSX2, MD_DRV};
        static const float lev[CHIPX_COUNT] = {0, 0, GTR_GB, GTR_PSG, GTR_PCE, GTR_VB, GTR_ZX, GTR_MSX2, MD_GTR};
        float gs = cg_sample(c, &c->sg[0]) + cg_sample(c, &c->sg[1]);

        if (c->mode == CHIPX_MD) {
            /* MEGA DRIVE: the distortion is all in the FM patch, straight
             * out of the YM2612 with no amp or cabinet */
            s += lev[c->mode] * gtr_hp(c, gs);
        } else {
            s += lev[c->mode] * gtr_ampz(c->sg[0].cab, 0.35f * gtr_hp(c, gs), drive[c->mode]);
        }
    }
    {
        /* presence: a first-order high shelf for the FM "bite" */
        float d = s - c->hs;

        c->hs = s;
        s += g_mix_pres[c->mode + 1] * d;
    }
    out = s * 9000.0f * g_mix_out[c->mode + 1];
    {
        int ji = (int)(c - g_c);
        float want = g_jingle_left[ji] > 0 ? JINGLE_GAIN : 1.0f;

        if (g_jingle_left[ji] > 0) {
            g_jingle_left[ji]--;
        }
        g_jingle_g[ji] += (want - g_jingle_g[ji]) * (1.0f / (0.01f * RATE));
        out *= g_jingle_g[ji];
    }
    {
        /* Bus limiter: instant attack, ~120 ms release, ceiling
         * -1 dBFS, so loudness matching never hard clips. */
        float a = fabsf(out), want = a > 29200.0f ? 29200.0f / a : 1.0f;

        if (c->lim <= 0.0f) {
            c->lim = 1.0f;
        }

        if (want < c->lim) {
            c->lim = want;
        } else {
            c->lim += (1.0f - c->lim) * 0.00019f;
            if (c->lim > want) {
                c->lim = want;
            }
        }
        out *= c->lim;
    }
    if (out > 32767.0f) {
        out = 32767.0f;
    } else if (out < -32768.0f) {
        out = -32768.0f;
    }
    return (int16_t)out;
}
