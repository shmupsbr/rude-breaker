/*
 * crt.c - CRT look for the playfield only (SCANLINES, CRT MASK,
 * CURVATURE, BLOOM, VIGNETTE, BAD RF CABLE). Integer maths, tables
 * built once, one scratch buffer: cheap enough for a Pi 4.
 */
#include "crt.h"

#include <stdlib.h>
#include <string.h>

static const char *const k_name[CRT_COUNT] = {
    "SCANLINES", "CRT MASK", "CURVATURE", "BLOOM", "VIGNETTE", "BAD RF CABLE"
};
static const char *const k_key[CRT_COUNT] = {
    "crt_scanlines", "crt_mask", "crt_curvature", "crt_bloom", "crt_vignette", "crt_badrf"
};

const char *crt_name(int fx) { return fx >= 0 && fx < CRT_COUNT ? k_name[fx] : ""; }
const char *crt_key(int fx) { return fx >= 0 && fx < CRT_COUNT ? k_key[fx] : ""; }

#define R_(p) ((int)((p) >> 24))
#define G_(p) ((int)(((p) >> 16) & 0xFF))
#define B_(p) ((int)(((p) >> 8) & 0xFF))
#define PK(r, g, b) (((uint32_t)(r) << 24) | ((uint32_t)(g) << 16) | ((uint32_t)(b) << 8) | 0xFFu)

static int clamp8(int v) { return v < 0 ? 0 : v > 255 ? 255 : v; }

static uint32_t g_rng = 0x1234567u;
static uint32_t rnd(void)
{
    g_rng ^= g_rng << 13;
    g_rng ^= g_rng >> 17;
    g_rng ^= g_rng << 5;
    return g_rng;
}

static uint32_t *g_tmp;
static int g_tw, g_th;
static int *g_curve;      /* source index per pixel, -1 = outside */
static uint8_t *g_vig;    /* 0..256 / 2 */
static int g_cw, g_ch, g_vw, g_vh;

static void copy_out(uint32_t *px, int pitch, int x0, int y0, int w, int h)
{
    int y;
    for (y = 0; y < h; y++) memcpy(g_tmp + (size_t)y * w, px + (size_t)(y0 + y) * pitch + x0, (size_t)w * 4);
}

static void bad_rf(uint32_t *px, int pitch, int x0, int y0, int w, int h, uint32_t frame)
{
    int x, y;
    int roll = (int)((frame * 3u) % (unsigned)(h + 120)) - 60;

    g_rng ^= frame * 2654435761u;
    copy_out(px, pitch, x0, y0, w, h);
    for (y = 0; y < h; y++) {
        uint32_t *d = px + (size_t)(y0 + y) * pitch + x0;
        const uint32_t *s = g_tmp + (size_t)y * w;
        /* line jitter: slow wobble plus the odd tear */
        int j = (int)((rnd() >> 29) & 1) - (int)((frame / 2 + (unsigned)y / 24) % 3 == 0);
        int band = y - roll;
        int lift = band >= 0 && band < 40 ? 28 - (band > 20 ? band - 20 : 20 - band) : 0;
        int ar = 0, ab = 0;

        if ((rnd() & 255) < 3) j += (int)(rnd() % 7) - 3;
        for (x = 0; x < w; x++) {
            int sx = x - j;
            int gx = sx - 6;
            uint32_t p = s[sx < 0 ? 0 : sx >= w ? w - 1 : sx];
            uint32_t q = s[gx < 0 ? 0 : gx >= w ? w - 1 : gx];
            int r = R_(p), g = G_(p), b = B_(p);
            int n = (int)(rnd() & 31) - 14;

            /* colour bleed: chroma smeared to the right */
            ar = (ar * 3 + r) >> 2;
            ab = (ab * 3 + b) >> 2;
            r = (r + ar) >> 1;
            b = (b + ab) >> 1;
            /* ghost 6 px to the left */
            r += R_(q) * 3 >> 3; g += G_(q) * 3 >> 3; b += B_(q) * 3 >> 3;
            if ((rnd() & 1023) < 6) n += 140; /* snow */
            d[x] = PK(clamp8(r + n + lift), clamp8(g + n + lift), clamp8(b + n + lift));
        }
    }
}

static void bloom(uint32_t *px, int pitch, int x0, int y0, int w, int h)
{
    int x, y;
    /* bright part, blurred: horizontal box 7 into g_tmp, then vertical box 5 added */
    for (y = 0; y < h; y++) {
        const uint32_t *s = px + (size_t)(y0 + y) * pitch + x0;
        uint32_t *t = g_tmp + (size_t)y * w;
        int sr = 0, sg = 0, sb = 0;
        for (x = -3; x < w; x++) {
            int a = x + 3, o = x - 4;
            if (a < w) { uint32_t p = s[a]; sr += R_(p) > 96 ? R_(p) - 96 : 0; sg += G_(p) > 96 ? G_(p) - 96 : 0; sb += B_(p) > 96 ? B_(p) - 96 : 0; }
            if (o >= 0) { uint32_t p = s[o]; sr -= R_(p) > 96 ? R_(p) - 96 : 0; sg -= G_(p) > 96 ? G_(p) - 96 : 0; sb -= B_(p) > 96 ? B_(p) - 96 : 0; }
            if (x >= 0) t[x] = PK(sr / 7, sg / 7, sb / 7);
        }
    }
    for (x = 0; x < w; x++) {
        int sr = 0, sg = 0, sb = 0;
        for (y = -2; y < h; y++) {
            int a = y + 2, o = y - 3;
            if (a < h) { uint32_t p = g_tmp[(size_t)a * w + x]; sr += R_(p); sg += G_(p); sb += B_(p); }
            if (o >= 0) { uint32_t p = g_tmp[(size_t)o * w + x]; sr -= R_(p); sg -= G_(p); sb -= B_(p); }
            if (y >= 0) {
                uint32_t *d = px + (size_t)(y0 + y) * pitch + x0 + x;
                uint32_t p = *d;
                *d = PK(clamp8(R_(p) + sr * 3 / 10), clamp8(G_(p) + sg * 3 / 10), clamp8(B_(p) + sb * 3 / 10));
            }
        }
    }
}

static void curvature(uint32_t *px, int pitch, int x0, int y0, int w, int h)
{
    int x, y;
    if (!g_curve || g_cw != w || g_ch != h) {
        free(g_curve);
        g_curve = (int *)malloc((size_t)w * h * sizeof(int));
        if (!g_curve) return;
        g_cw = w; g_ch = h;
        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                /* barrel: u' = u * (1 + k r^2), u,v in -1..1 */
                double u = (2.0 * x + 1 - w) / w, v = (2.0 * y + 1 - h) / h;
                double r2 = u * u + v * v, f = 1.0 + 0.045 * r2 * 2.0;
                double su = u * f * 0.985, sv = v * f * 0.985;
                int sx = (int)((su * w + w) / 2), sy = (int)((sv * h + h) / 2);
                g_curve[y * w + x] = sx < 0 || sy < 0 || sx >= w || sy >= h ? -1 : sy * w + sx;
            }
        }
    }
    copy_out(px, pitch, x0, y0, w, h);
    for (y = 0; y < h; y++) {
        uint32_t *d = px + (size_t)(y0 + y) * pitch + x0;
        const int *m = g_curve + (size_t)y * w;
        for (x = 0; x < w; x++) d[x] = m[x] < 0 ? 0x000000FFu : g_tmp[m[x]];
    }
}

static void scanlines(uint32_t *px, int pitch, int x0, int y0, int w, int h)
{
    int x, y;
    for (y = 1; y < h; y += 2) {
        uint32_t *d = px + (size_t)(y0 + y) * pitch + x0;
        for (x = 0; x < w; x++) {
            uint32_t p = d[x];
            d[x] = PK(R_(p) * 9 >> 4, G_(p) * 9 >> 4, B_(p) * 9 >> 4);
        }
    }
}

static void phosphor(uint32_t *px, int pitch, int x0, int y0, int w, int h)
{
    int x, y;
    /* slot mask: RGB triads, every other 2-row group offset by half a slot */
    for (y = 0; y < h; y++) {
        uint32_t *d = px + (size_t)(y0 + y) * pitch + x0;
        int off = ((y >> 1) & 1) ? 1 : 0;
        int edge = (y & 1) == 0 && ((y >> 1) & 1) == 0;
        for (x = 0; x < w; x++) {
            uint32_t p = d[x];
            int c = (x + off) % 3;
            int r = R_(p) * (c == 0 ? 20 : 13) >> 4;
            int g = G_(p) * (c == 1 ? 20 : 13) >> 4;
            int b = B_(p) * (c == 2 ? 20 : 13) >> 4;
            if (edge && c == 2) { r = r * 3 >> 2; g = g * 3 >> 2; b = b * 3 >> 2; }
            d[x] = PK(clamp8(r), clamp8(g), clamp8(b));
        }
    }
}

static void vignette(uint32_t *px, int pitch, int x0, int y0, int w, int h)
{
    int x, y;
    if (!g_vig || g_vw != w || g_vh != h) {
        free(g_vig);
        g_vig = (uint8_t *)malloc((size_t)w * h);
        if (!g_vig) return;
        g_vw = w; g_vh = h;
        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                double u = (2.0 * x + 1 - w) / w, v = (2.0 * y + 1 - h) / h;
                double f = 1.0 - 0.45 * (u * u * u * u + v * v * v * v) - 0.12 * (u * u + v * v);
                g_vig[y * w + x] = (uint8_t)(f < 0 ? 0 : f * 255);
            }
        }
    }
    for (y = 0; y < h; y++) {
        uint32_t *d = px + (size_t)(y0 + y) * pitch + x0;
        const uint8_t *m = g_vig + (size_t)y * w;
        for (x = 0; x < w; x++) {
            uint32_t p = d[x];
            int k = m[x];
            d[x] = PK(R_(p) * k >> 8, G_(p) * k >> 8, B_(p) * k >> 8);
        }
    }
}

void crt_apply(uint32_t *px, int pitch, int x0, int y0, int w, int h,
               unsigned mask, uint32_t frame)
{
    if (!mask || !px || w <= 0 || h <= 0) return;
    if (!g_tmp || g_tw * g_th < w * h) {
        free(g_tmp);
        g_tmp = (uint32_t *)malloc((size_t)w * h * 4);
        if (!g_tmp) return;
        g_tw = w; g_th = h;
    }
    if (mask & (1u << CRT_BADRF)) bad_rf(px, pitch, x0, y0, w, h, frame);
    if (mask & (1u << CRT_BLOOM)) bloom(px, pitch, x0, y0, w, h);
    if (mask & (1u << CRT_CURVATURE)) curvature(px, pitch, x0, y0, w, h);
    if (mask & (1u << CRT_SCANLINES)) scanlines(px, pitch, x0, y0, w, h);
    if (mask & (1u << CRT_MASK)) phosphor(px, pitch, x0, y0, w, h);
    if (mask & (1u << CRT_VIGNETTE)) vignette(px, pitch, x0, y0, w, h);
}
