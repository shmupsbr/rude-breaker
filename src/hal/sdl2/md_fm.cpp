/* Mega Drive guitar on a real YM2612 core (ymfm::ym2612, Aaron Giles,
 * BSD-3-Clause, vendored under third_party/ymfm).
 *
 * One chip per guitar voice (two per synth instance), three FM
 * channels each, all sounding the written note: a four-operator chain
 * with heavy feedback, and two layers whose carriers sound an octave
 * below (one about 11 cents flat, one an octave lower still, panned
 * left).  A changed note slides in from below for one frame; on release
 * the pitch drops away.  No LFO.  The chip runs at its own rate
 * (7670453 / 144 Hz); the output goes through a console-like output
 * stage (one-pole low-pass, slight low cut) and is resampled. */
#include "ymfm_opn.h"
#include <cmath>
#include <cstring>

namespace {

class MdIfc : public ymfm::ymfm_interface {};

struct GtrChan {
    int b0, b4;
    double cents;
    int op[4][7];
};
#include "md_fm_patch.h"

const double CHIP_RATE = 7670453.0 / 144.0;

struct Voice {
    MdIfc ifc;
    ymfm::ym2612 chip{ifc};
    bool init = false, keyed = false;
    int last_fr = -1, tl_add = -1, rel = 0;
    double acc = 0.0, prev = 0.0, cur = 0.0, lp = 0.0, lo = 0.0, dc = 0.0, last_f = 0.0;
    bool scoop = false;
    int fnum[3] = {-1, -1, -1};
    void w(int r, int d) { chip.write(0, (uint8_t)r); chip.write(1, (uint8_t)d); }
};

Voice g_v[4];
const int k_slot[4] = {0, 8, 4, 12};

bool is_carrier(int alg, int o)
{
    if (o == 3 || alg == 7) return true;
    if (alg == 4) return o == 1;
    if (alg == 5 || alg == 6) return o > 0;
    return false;
}

void set_tl(Voice &v, int add)
{
    for (int ch = 0; ch < 3; ch++) {
        const GtrChan &p = k_md_gtr_patch[ch];

        for (int o = 0; o < 4; o++) {
            int tl = p.op[o][1] + (is_carrier(p.b0 & 7, o) ? add : 0);

            v.w(0x40 + k_slot[o] + ch, tl > 127 ? 127 : tl);
        }
    }
    v.tl_add = add;
}

void setup(Voice &v)
{
    v.chip.reset();
    v.w(0x22, 0);
    v.w(0x27, 0);
    v.w(0x2B, 0);
    for (int ch = 0; ch < 3; ch++) {
        const GtrChan &p = k_md_gtr_patch[ch];

        v.w(0xB0 + ch, p.b0);
        v.w(0xB4 + ch, p.b4);
        for (int o = 0; o < 4; o++) {
            const int s = k_slot[o] + ch;

            v.w(0x30 + s, p.op[o][0]);
            v.w(0x50 + s, p.op[o][2]);
            v.w(0x60 + s, p.op[o][3]);
            v.w(0x70 + s, p.op[o][4]);
            v.w(0x80 + s, p.op[o][5]);
            v.w(0x90 + s, p.op[o][6]);
        }
    }
    set_tl(v, 0);
    v.init = true;
}

void set_freq(Voice &v, int ch, double hz)
{
    int blk = 0, n;
    double k = 144.0 * 2097152.0 / 7670453.0;

    while (blk < 7 && hz * k / (double)(1 << blk) > 2047.0) {
        blk++;
    }
    n = (int)lround(hz * k / (double)(1 << blk));
    n = n > 2047 ? 2047 : n < 1 ? 1 : n;
    n |= blk << 11;
    if (n != v.fnum[ch]) {
        v.w(0xA4 + ch, n >> 8);
        v.w(0xA0 + ch, n & 255);
        v.fnum[ch] = n;
    }
}

}  /* namespace */

/* one output sample at `rate` for guitar voice `idx` (0..3); f = note
 * in Hz, e = level 0..1, fr = 60 Hz frames since the pick, keyed =
 * string still held */
extern "C" float mdfm_gtr(int idx, float f, float e, int fr, int keyed, float rate)
{
    Voice &v = g_v[idx & 3];
    double hz = f, fps = (double)rate / 60.0;
    int add;

    if (!v.init) {
        setup(v);
    }
    if (fr == 0 && v.last_fr != 0) {          /* new pick: retrigger */
        v.w(0x28, 0x00); v.w(0x28, 0x01); v.w(0x28, 0x02);
        v.keyed = false;
        v.scoop = v.last_f > 0.0 && fabs((double)f / v.last_f - 1.0) > 0.03;
        v.last_f = f;
    }
    if (fr == 0 && v.scoop) {
        hz *= 0.68;                           /* one frame below, then the note */
    }
    if (!keyed) {                             /* release: the pitch drops away */
        v.rel++;
        hz *= v.rel < fps ? 0.49 : 0.33;
    } else {
        v.rel = 0;
    }
    for (int ch = 0; ch < 3; ch++) {
        set_freq(v, ch, hz * pow(2.0, k_md_gtr_patch[ch].cents / 1200.0));
    }
    if (keyed && !v.keyed) {
        v.w(0x28, 0xF0); v.w(0x28, 0xF1); v.w(0x28, 0xF2);
    } else if (!keyed && v.keyed) {
        v.w(0x28, 0x00); v.w(0x28, 0x01); v.w(0x28, 0x02);
    }
    v.keyed = keyed != 0;
    v.last_fr = fr;
    add = e > 0.0f ? (int)lround(-20.0 * log10((double)e) / 0.75) : 127;
    add = add > 127 ? 127 : add;
    if (add != v.tl_add) {
        set_tl(v, add);
    }
    v.acc += CHIP_RATE / (double)rate;
    while (v.acc >= 1.0) {
        ymfm::ym2612::output_data out;
        double s;

        v.chip.generate(&out);
        s = (double)(out.data[0] + out.data[1]) * (0.5 / 8192.0);
        v.dc += 0.0024 * (s - v.dc);          /* ~20 Hz: ymfm's mix carries DC */
        s -= v.dc;
        v.lp += 0.5 * (s - v.lp);             /* console output stage: ~6 kHz one-pole */
        v.lo += 0.01 * (v.lp - v.lo);         /* and a slight cut below ~85 Hz */
        v.prev = v.cur;
        v.cur = v.lp - 0.3 * v.lo;
        v.acc -= 1.0;
    }
    return (float)(v.prev + (v.cur - v.prev) * (1.0 - v.acc));
}
