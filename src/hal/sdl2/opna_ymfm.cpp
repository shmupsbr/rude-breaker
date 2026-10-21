/**
 * ymfm::ym2608 at the PC-98 clock, 7987200 Hz, medium fidelity
 * (output rate clock/24). Nearest-neighbor down to 44100 Hz.
 * BSD-3-Clause, Copyright (c) 2021, Aaron Giles. See third_party/ymfm/LICENSE.
 */
#include "opna.h"

#include "ymfm_opn.h"

#include <stdint.h>

class OpnaIfc : public ymfm::ymfm_interface {
};

static OpnaIfc g_ifc;
static ymfm::ym2608 g_chip(g_ifc);

static const uint32_t OPNA_CLOCK = 7987200u;
static const uint32_t OUT_RATE = 44100u;

static uint32_t g_chip_rate;
static uint32_t g_accum;
static int32_t g_hold_l;
static int32_t g_hold_r;
static int g_live;

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

extern "C" void opna_reset(void)
{
    g_chip.set_fidelity(ymfm::OPN_FIDELITY_MED);
    g_chip.reset();
    g_chip_rate = g_chip.sample_rate(OPNA_CLOCK);
    if (g_chip_rate == 0) {
        g_chip_rate = OUT_RATE;
    }
    g_accum = 0;
    g_hold_l = 0;
    g_hold_r = 0;
    g_live = 1;
}

extern "C" void opna_write(uint16_t port, uint8_t value)
{
    uint32_t off;

    if (!g_live) {
        opna_reset();
    }
    if (port == 0x188u) {
        off = 0;
    } else if (port == 0x18Au) {
        off = 1;
    } else if (port == 0x18Cu) {
        off = 2;
    } else if (port == 0x18Eu) {
        off = 3;
    } else {
        return;
    }
    g_chip.write(off, value);
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
        ymfm::ym2608::output_data out;

        g_chip.generate(&out, 1);
        /* FM stereo plus the mono SSG, which ymfm leaves in slot 2. */
        left = out.data[0] + out.data[2];
        right = out.data[1] + out.data[2];
        g_accum -= OUT_RATE;
    }
    g_hold_l = left;
    g_hold_r = right;
    stereo[0] = sat16(left);
    stereo[1] = sat16(right);
}
