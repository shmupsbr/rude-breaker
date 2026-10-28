/**
 * ymfm::ym2203 (OPN) at the PC-98 sound clock, 3993600 Hz, medium
 * fidelity. FPLAY.COM only drives the YM2203 part of the board: three
 * FM channels and the SSG. Nearest-neighbor down to 44100 Hz.
 * BSD-3-Clause, Copyright (c) 2021, Aaron Giles. See third_party/ymfm/LICENSE.
 */
#include "opna.h"

#include "ymfm_opn.h"

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

class OpnIfc : public ymfm::ymfm_interface {
};

static OpnIfc g_ifc;
static ymfm::ym2203 g_chip(g_ifc);

static const uint32_t OPN_CLOCK = 3993600u;
static const uint32_t OUT_RATE = 44100u;

static uint32_t g_chip_rate;
static uint32_t g_accum;
static int32_t g_hold_l;
static int32_t g_hold_r;
static int g_live;
/* 20 Hz one-pole high-pass. ymfm's mix carries a DC offset; this
 * removes it and leaves the chip's bass alone. */
static float g_hp_l;
static float g_hp_r;
static float g_x1_l;
static float g_x1_r;
static const float HP_R = 0.997152f;

static int16_t sat16(int32_t v)
{
    if (v > 32767) {
        return 32767;
    }
    if (v < -32768) {
        return (int16_t)-32768;
    }
    return (int16_t)v;
}

/* Linear through 24000, then a soft knee. Quiet material is unchanged. */
static int16_t limit16(float v)
{
    float a = v < 0.0f ? -v : v;
    float s = v < 0.0f ? -1.0f : 1.0f;

    if (a > 24000.0f) {
        float over = (a - 24000.0f) / 8767.0f;

        if (over > 8.0f) {
            over = 8.0f;
        }
        a = 24000.0f + 8767.0f * (1.0f - expf(-over));
    }
    return sat16((int32_t)(s * a));
}

extern "C" void opna_reset(void)
{
    g_chip.set_fidelity(ymfm::OPN_FIDELITY_MED);
    g_chip.reset();
    g_chip_rate = g_chip.sample_rate(OPN_CLOCK);
    if (g_chip_rate == 0) {
        g_chip_rate = OUT_RATE;
    }
    g_accum = 0;
    g_hold_l = 0;
    g_hold_r = 0;
    g_hp_l = 0;
    g_hp_r = 0;
    g_x1_l = 0;
    g_x1_r = 0;
    g_live = 1;
}

extern "C" void opna_write(uint16_t port, uint8_t value)
{
    if (!g_live) {
        opna_reset();
    }
    /* The YM2203 has one address/data pair. 0x18C/0x18E belong to the
     * YM2608 extension, which FPLAY.COM never uses. */
    if (port == 0x188u) {
        g_chip.write(0, value);
    } else if (port == 0x18Au) {
        g_chip.write(1, value);
    }
}

extern "C" void opna_sample(int16_t *stereo)
{
    int32_t left;
    int32_t right;

    if (!stereo) {
        return;
    }
    if (!g_live) {
        stereo[0] = 0;
        stereo[1] = 0;
        return;
    }
    g_accum += g_chip_rate;
    left = g_hold_l;
    right = g_hold_r;
    while (g_accum >= OUT_RATE) {
        ymfm::ym2203::output_data out;
        int32_t fm;
        int32_t ssg;

        g_chip.generate(&out, 1);
        /* data[0] is the mono FM mix (full 14-bit OPN scale, twice
         * ymfm's OPNA FM scale), data[1..3] are the SSG channels. FM at
         * 1.5x and the SSG sum at 2/3 keep the FM/SSG balance and the
         * loudness of the earlier mix: the FM sits under the SSG in the
         * raw sum, and option select, stage 1 and the final boss need
         * the lift to match the SSG-led stages. */
        fm = out.data[0] * 3 / 2;
        ssg = (out.data[1] + out.data[2] + out.data[3]) * 2 / 3;
        left = fm + ssg;
        right = left;
        g_accum -= OUT_RATE;
    }
    g_hold_l = left;
    g_hold_r = right;
    /* The hold stays raw so a repeated output sample is not filtered twice. */
    {
        float xl = (float)left;
        float xr = (float)right;
        float hpl = xl - g_x1_l + HP_R * g_hp_l;
        float hpr = xr - g_x1_r + HP_R * g_hp_r;

        g_x1_l = xl;
        g_x1_r = xr;
        g_hp_l = hpl;
        g_hp_r = hpr;
        stereo[0] = limit16(hpl);
        stereo[1] = limit16(hpr);
    }
}
