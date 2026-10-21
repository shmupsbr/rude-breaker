/**
 * ymfm::ym2608 at the PC-98 clock, 7987200 Hz, medium fidelity
 * (output rate clock/24). Nearest-neighbor down to 44100 Hz.
 * BSD-3-Clause, Copyright (c) 2021, Aaron Giles. See third_party/ymfm/LICENSE.
 */
#include "opna.h"

#include "exepath.h"
#include "ymfm_opn.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

enum { RHYTHM_N = 8192 };

static uint8_t g_rom[RHYTHM_N];
static int g_rom_ok;
static char g_override[1024];
static uint32_t g_reads_a;
static uint32_t g_reads_b;
static uint32_t g_rhythm_writes;
static uint8_t g_addr_latch[2];

class OpnaIfc : public ymfm::ymfm_interface {
public:
    uint8_t ymfm_external_read(ymfm::access_class type, uint32_t address) override
    {
        if (type == ymfm::ACCESS_ADPCM_A) {
            g_reads_a++;
            if (g_rom_ok && address < RHYTHM_N) {
                return g_rom[address];
            }
            return 0;
        }
        if (type == ymfm::ACCESS_ADPCM_B) {
            g_reads_b++;
        }
        return 0;
    }
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

static uint32_t crc32_ieee(const uint8_t *p, size_t n)
{
    uint32_t c = 0xffffffffu;
    size_t i;

    for (i = 0; i < n; i++) {
        int b;

        c ^= p[i];
        for (b = 0; b < 8; b++) {
            uint32_t mask = (uint32_t)-(int)(c & 1u);

            c = (c >> 1) ^ (0xedb88320u & mask);
        }
    }
    return ~c;
}

/* 1 = loaded, 0 = absent (no message) or rejected. */
static int load_rhythm_file(const char *path)
{
    FILE *fp;
    long n;
    uint8_t buf[RHYTHM_N];
    uint32_t crc;

    if (!path || !path[0]) {
        return 0;
    }
    fp = fopen(path, "rb");
    if (!fp) {
        return 0;
    }
    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return 0;
    }
    n = ftell(fp);
    if (n != RHYTHM_N || fseek(fp, 0, SEEK_SET) != 0 ||
        fread(buf, 1, RHYTHM_N, fp) != RHYTHM_N) {
        fprintf(stderr, "opna: rhythm ROM rejected (%s)\n", path);
        fclose(fp);
        g_rom_ok = 0;
        return 0;
    }
    fclose(fp);
    crc = crc32_ieee(buf, RHYTHM_N);
    if (crc != 0x23c9e0d8u) {
        fprintf(stderr, "opna: rhythm ROM rejected (%s)\n", path);
        g_rom_ok = 0;
        return 0;
    }
    memcpy(g_rom, buf, RHYTHM_N);
    g_rom_ok = 1;
    fprintf(stderr, "opna: ADPCM-A rhythm ROM loaded\n");
    return 1;
}

static int rom_beside_exe(char *out, size_t n)
{
    char dir[1024];

    if (rb_exe_dir(dir, sizeof dir) != 0) {
        return -1;
    }
    if (snprintf(out, n, "%s/ym2608_adpcm_rom.bin", dir) >= (int)n) {
        return -1;
    }
    return 0;
}

extern "C" void opna_set_rhythm_override(const char *path)
{
    if (!path) {
        g_override[0] = 0;
        return;
    }
    snprintf(g_override, sizeof g_override, "%s", path);
}

extern "C" void opna_autoload_rhythm(const char *data_dir)
{
    const char *env;
    char beside[1024];
    char in_data[1024];

    if (g_override[0]) {
        load_rhythm_file(g_override);
        return;
    }
    env = getenv("RUDEBR_YM2608_ROM");
    if (env && env[0]) {
        load_rhythm_file(env);
        return;
    }
    if (rom_beside_exe(beside, sizeof beside) == 0 && load_rhythm_file(beside)) {
        return;
    }
    if (data_dir && data_dir[0] &&
        snprintf(in_data, sizeof in_data, "%s/ym2608_adpcm_rom.bin", data_dir) <
            (int)sizeof in_data) {
        load_rhythm_file(in_data);
    }
}

extern "C" int opna_rhythm_loaded(void)
{
    return g_rom_ok;
}

extern "C" uint32_t opna_adpcm_a_reads(void)
{
    return g_reads_a;
}

extern "C" uint32_t opna_adpcm_b_reads(void)
{
    return g_reads_b;
}

extern "C" uint32_t opna_rhythm_port_writes(void)
{
    return g_rhythm_writes;
}

extern "C" void opna_adpcm_reads_reset(void)
{
    g_reads_a = 0;
    g_reads_b = 0;
    g_rhythm_writes = 0;
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
        g_addr_latch[0] = value;
    } else if (port == 0x18Au) {
        off = 1;
        /* YM2608 rhythm (ADPCM-A) lives at low-port addresses 0x10-0x1F. */
        if (g_addr_latch[0] >= 0x10u && g_addr_latch[0] <= 0x1Fu) {
            g_rhythm_writes++;
        }
    } else if (port == 0x18Cu) {
        off = 2;
        g_addr_latch[1] = value;
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
