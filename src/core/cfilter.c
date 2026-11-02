#include "cfilter.h"

#include <stdlib.h>
#include <string.h>

/* GAME BOY and VIRTUAL BOY: six-shade luminance ramps. GAME GEAR: 4 bits
 * per channel through a backlit LCD. PC ENGINE: 3 bits per channel (512
 * colours), vivid. These are 4096-entry tables keyed on the 12-bit
 * colour. COMMODORE 64 also keeps the machine's colours-per-cell limit
 * (see clash()). Distances are "redmean" weighted RGB. */

static const char *const k_name[CF_COUNT] = {
    "ORIGINAL", "GAME BOY", "VIRTUAL BOY", "COMMODORE 64", "GAME GEAR",
    "PC ENGINE"
};
static const char *const k_key[CF_COUNT] = {
    "original", "gameboy", "virtualboy", "c64", "gamegear", "pcengine"
};

/* DMG LCD greens, darkest first: the four classic shades with two
 * in-between steps. */
static const uint32_t k_gb[6] = {0x0F380Fu, 0x1F4D1Fu, 0x306230u, 0x5E8720u,
                                 0x8BAC0Fu, 0x9BBC0Fu};
/* Virtual Boy red LED on black, darkest first. */
static const uint32_t k_vb[6] = {0x000000u, 0x330000u, 0x660000u, 0x990000u,
                                 0xCC0000u, 0xFF0000u};
/* Commodore 64, Pepto's palette. */
static const uint32_t k_c64[16] = {
    0x000000u, 0xFFFFFFu, 0x68372Bu, 0x70A4B2u, 0x6F3D86u, 0x588D43u,
    0x352879u, 0xB8C76Fu, 0x6F4F25u, 0x433900u, 0x9A6759u, 0x444444u,
    0x6C6C6Cu, 0x9AD284u, 0x6C5EB5u, 0x959595u
};


static uint32_t g_lut[CF_COUNT][4096];
static int g_ready[CF_COUNT];

const char *cfilter_name(int mode)
{
    return mode >= 0 && mode < CF_COUNT ? k_name[mode] : k_name[0];
}

const char *cfilter_key(int mode)
{
    return mode >= 0 && mode < CF_COUNT ? k_key[mode] : k_key[0];
}

int cfilter_from_key(const char *s)
{
    int i;

    static const char *const k_gone[] = {"msx2", "spectrum", "msx1", "nes", "mastersystem", "megadrive"};

    if (!s) {
        return -1;
    }
    for (i = 0; i < (int)(sizeof k_gone / sizeof k_gone[0]); i++) {
        size_t n = strlen(k_gone[i]);

        if (strncmp(s, k_gone[i], n) == 0 && (s[n] == 0 || s[n] == '\n' || s[n] == '\r')) {
            return CF_ORIGINAL;  /* removed filters load as OFF */
        }
    }
    for (i = 0; i < CF_COUNT; i++) {
        size_t n = strlen(k_key[i]);

        if (strncmp(s, k_key[i], n) == 0 &&
            (s[n] == 0 || s[n] == '\n' || s[n] == '\r')) {
            return i;
        }
    }
    return -1;
}

static long redmean(int r1, int g1, int b1, uint32_t c)
{
    int r2 = (int)(c >> 16) & 255, g2 = (int)(c >> 8) & 255, b2 = (int)c & 255;
    long rm = (r1 + r2) / 2;
    long dr = r1 - r2, dg = g1 - g2, db = b1 - b2;

    return ((512 + rm) * dr * dr >> 8) + 4 * dg * dg + ((767 - rm) * db * db >> 8);
}

static uint32_t nearest(int r, int g, int b, const uint32_t *pal, int n)
{
    long best = -1;
    uint32_t out = 0;
    int i;

    for (i = 0; i < n; i++) {
        long d = redmean(r, g, b, pal[i]);

        if (best < 0 || d < best) {
            best = d;
            out = pal[i];
        }
    }
    return out;
}

/* Luminance ramp: the game is mostly dark, so the cuts sit low enough
 * that the star field and the ship's mid tones keep their own shades. */
static uint32_t ramp(int r, int g, int b, const uint32_t *pal)
{
    int y = (299 * r + 587 * g + 114 * b) / 1000;

    return pal[y < 12 ? 0 : y < 36 ? 1 : y < 64 ? 2 : y < 100 ? 3 : y < 150 ? 4 : 5];
}

/* Game Gear: 4 bits per channel (4096 colours) seen through the backlit
 * LCD, so blacks sit a little lifted and the colours a little washed. */
static uint32_t gamegear(int r, int g, int b)
{
    int y = (r * 77 + g * 150 + b * 29) >> 8;
    int v[3];
    int i;

    v[0] = r; v[1] = g; v[2] = b;
    for (i = 0; i < 3; i++) {
        int c = (v[i] * 7 + y) >> 3;            /* 12 % toward grey */
        c = 22 + c * 213 / 255;                 /* lifted black, soft white */
        c = (c * 15 + 127) / 255;               /* 4 bits */
        v[i] = c * 17;
    }
    return ((uint32_t)v[0] << 16) | ((uint32_t)v[1] << 8) | (uint32_t)v[2];
}

static int c8s(int v) { return v < 0 ? 0 : v > 255 ? 255 : v; }

static uint32_t pce9(int r, int g, int b)
{
    /* 3 bits per channel, 512 colours. */
    r = (r * 7 + 127) / 255; g = (g * 7 + 127) / 255; b = (b * 7 + 127) / 255;
    return (uint32_t)((r * 255 / 7) << 16 | (g * 255 / 7) << 8 | (b * 255 / 7));
}

/* Push a colour away from (k > 256) or toward (k < 256) its grey; k in
 * 1/256, then add `lift`. */
static void sat_adj(int *r, int *g, int *b, int k, int lift)
{
    int y = (*r * 77 + *g * 150 + *b * 29) >> 8;
    *r = c8s(y + (*r - y) * k / 256 + lift);
    *g = c8s(y + (*g - y) * k / 256 + lift);
    *b = c8s(y + (*b - y) * k / 256 + lift);
}

static void build(int mode)
{
    int k;

    for (k = 0; k < 4096; k++) {
        int r = ((k >> 8) & 15) * 17, g = ((k >> 4) & 15) * 17, b = (k & 15) * 17;
        uint32_t c;

        if (mode == CF_PCENGINE) sat_adj(&r, &g, &b, 440, 14);  /* strong, bright */
        switch (mode) {
        case CF_GAMEBOY:    c = ramp(r, g, b, k_gb); break;
        case CF_GAMEGEAR:   c = gamegear(r, g, b); break;
        case CF_VIRTUALBOY: c = ramp(r, g, b, k_vb); break;
        case CF_C64:        c = nearest(r, g, b, k_c64, 16); break;
        default:            c = pce9(r, g, b); break;  /* PC ENGINE: 512 colours */
        }
        g_lut[mode][k] = c << 8;
    }
    g_ready[mode] = 1;
}

#define KEY(p) ((((p) >> 20) & 0xF00u) | (((p) >> 16) & 0x0F0u) | (((p) >> 12) & 0x00Fu))

void cfilter_apply_flat(uint32_t *px, size_t n, int mode)
{
    const uint32_t *lut;
    size_t i;

    if (!px || mode <= CF_ORIGINAL || mode >= CF_COUNT) {
        return;
    }
    if (!g_ready[mode]) {
        build(mode);
    }
    lut = g_lut[mode];
    for (i = 0; i < n; i++) {
        uint32_t p = px[i];

        px[i] = lut[KEY(p)] | (p & 0xFFu);
    }
}

/* ---- Colour clash ------------------------------------------------------
 * The frame is read as a grid of "machine pixels" (bw x bh host pixels,
 * averaged, which is the blur), grouped into cells of cw x ch machine
 * pixels. COMMODORE 64: 2x2 blocks, 8x8 cells, shared black background
 * + 3 colours; sprites keep their own colours (sprite layer).
 * Index tables map a 12-bit colour to the nearest entry of a palette. */

static uint8_t g_ix_c64[4096];
static int g_ix_ready;

static void build_ix(void)
{
    int k, i;

    for (k = 0; k < 4096; k++) {
        int r = ((k >> 8) & 15) * 17, g = ((k >> 4) & 15) * 17, b = (k & 15) * 17;
        uint32_t c = nearest(r, g, b, k_c64, 16);

        for (i = 0; i < 16 && k_c64[i] != c; i++) {}
        g_ix_c64[k] = (uint8_t)i;
    }
    g_ix_ready = 1;
}

/* Up to 64 machine pixels per cell (8x8). */
typedef struct { int r, g, b; uint16_t key; } MPix;

static int top_n(const int *cnt, int n, int *out, int want, int skip)
{
    int got = 0, j;

    while (got < want) {
        int best = -1;

        for (j = 0; j < n; j++) {
            int used = j == skip, q;

            for (q = 0; q < got; q++) {
                used |= out[q] == j;
            }
            if (!used && cnt[j] > 0 && (best < 0 || cnt[j] > cnt[best])) {
                best = j;
            }
        }
        if (best < 0) {
            break;
        }
        out[got++] = best;
    }
    return got;
}

static const uint8_t *g_layer;

void cfilter_set_layer(const uint8_t *mask)
{
    g_layer = mask;
}

static void clash(uint32_t *px, int w, int h, int mode)
{
    int bw, bh, cw, ch;
    int cx, cy;
    MPix mp[64];

    (void)mode;  /* COMMODORE 64 */
    bw = 2; bh = 2; cw = 8; ch = 8;
    if (!g_ix_ready) {
        build_ix();
    }
    for (cy = 0; cy < h; cy += bh * ch) {
        for (cx = 0; cx < w; cx += bw * cw) {
            int n = 0, mx, my, i;
            uint32_t pal[4] = {0u, 0u, 0u, 0u};
            int np = 0;

            for (my = 0; my < ch; my++) {
                for (mx = 0; mx < cw; mx++) {
                    int x0 = cx + mx * bw, y0 = cy + my * bh, x, y, c = 0;
                    MPix *m = &mp[n];

                    m->r = m->g = m->b = 0;
                    for (y = y0; y < y0 + bh && y < h; y++) {
                        for (x = x0; x < x0 + bw && x < w; x++) {
                            uint32_t p = px[(size_t)y * w + x];
                            m->r += (int)(p >> 24); m->g += (int)(p >> 16) & 255;
                            m->b += (int)(p >> 8) & 255; c++;
                        }
                    }
                    if (c) {
                        m->r /= c; m->g /= c; m->b /= c;
                    }
                    m->key = (uint16_t)((m->r >> 4) << 8 | (m->g >> 4) << 4 | (m->b >> 4));
                    n++;
                }
            }
            {
                int cnt[16] = {0}, sel[3], k;

                for (i = 0; i < n; i++) {
                    cnt[g_ix_c64[mp[i].key]]++;
                }
                pal[0] = k_c64[0];  /* shared background colour */
                k = top_n(cnt, 16, sel, 3, 0);
                np = 1;
                for (i = 0; i < k; i++) {
                    pal[np++] = k_c64[sel[i]];
                }
            }
            /* Write each machine pixel as the closest allowed colour. */
            n = 0;
            for (my = 0; my < ch; my++) {
                for (mx = 0; mx < cw; mx++, n++) {
                    int x0 = cx + mx * bw, y0 = cy + my * bh, x, y, q;
                    uint32_t c = pal[0];
                    long bd = redmean(mp[n].r, mp[n].g, mp[n].b, pal[0]);

                    for (q = 1; q < np; q++) {
                        long d = redmean(mp[n].r, mp[n].g, mp[n].b, pal[q]);
                        if (d < bd) {
                            bd = d; c = pal[q];
                        }
                    }
                    if (mode == CF_C64 && g_layer && y0 < h && x0 < w &&
                        (g_layer[(size_t)y0 * w + x0] || (x0 + 1 < w && g_layer[(size_t)y0 * w + x0 + 1]))) {
                        /* Hardware sprites: free colour, no cell clash. */
                        c = nearest(mp[n].r, mp[n].g, mp[n].b, k_c64, 16);
                        if (redmean((int)(c >> 16), (int)(c >> 8) & 255, (int)c & 255, k_c64[0]) < 12000) {
                            c = k_c64[1];
                        }
                    }
                    for (y = y0; y < y0 + bh && y < h; y++) {
                        for (x = x0; x < x0 + bw && x < w; x++) {
                            uint32_t *p = &px[(size_t)y * w + x];
                            *p = (c << 8) | (*p & 0xFFu);
                        }
                    }
                }
            }
        }
    }
}

/* ---- the machines' flaws, after the palette -------------------------- */

#define CR(p) ((int)((p) >> 24))
#define CG(p) ((int)(((p) >> 16) & 0xFF))
#define CB(p) ((int)(((p) >> 8) & 0xFF))
#define CPK(r, g, b) (((uint32_t)(r) << 24) | ((uint32_t)(g) << 16) | ((uint32_t)(b) << 8) | 0xFFu)

static uint32_t *g_prev;      /* last output, for LCD / LED persistence */
static int g_prev_mode = -1;
static uint32_t g_frame_no;


/* Keep only the `keep` most used colours of a frame already reduced to
 * a 2^(3*bits) colour space (bits per channel); the rest go to the
 * nearest kept one. Region x0..x0+rw, y0..y0+rh of a w-wide frame. */
static void limit_colours(uint32_t *px, int w, int x0, int y0, int rw, int rh,
                          int bits, int keep)
{
    static int cnt[4096];
    static uint32_t col[4096];
    static int16_t map[4096];
    int lv = 1 << bits, n = lv * lv * lv, sh = 8 - bits;
    int x, y, i, k, used = 0, kept[64], nk = 0;

    memset(cnt, 0, sizeof(int) * (size_t)n);
    for (y = y0; y < y0 + rh; y++) {
        for (x = x0; x < x0 + rw; x++) {
            uint32_t p = px[(size_t)y * w + x];
            int ix = ((CR(p) >> sh) * lv + (CG(p) >> sh)) * lv + (CB(p) >> sh);
            if (!cnt[ix]++) { col[ix] = p; used++; }
        }
    }
    if (used <= keep) return;
    if (keep > 64) keep = 64;
    for (k = 0; k < keep; k++) {
        int best = -1;
        for (i = 0; i < n; i++) if (cnt[i] > 0 && (best < 0 || cnt[i] > cnt[best])) best = i;
        if (best < 0) break;
        kept[nk++] = best;
        cnt[best] = -cnt[best];
    }
    for (i = 0; i < n; i++) {
        long bd = -1;
        map[i] = (int16_t)i;
        if (cnt[i] <= 0) continue;
        for (k = 0; k < nk; k++) {
            uint32_t q = col[kept[k]], p = col[i];
            long dr = CR(p) - CR(q), dg = CG(p) - CG(q), db = CB(p) - CB(q);
            long d = 2 * dr * dr + 4 * dg * dg + 3 * db * db;
            if (bd < 0 || d < bd) { bd = d; map[i] = (int16_t)kept[k]; }
        }
    }
    for (y = y0; y < y0 + rh; y++) {
        for (x = x0; x < x0 + rw; x++) {
            uint32_t *p = &px[(size_t)y * w + x];
            int ix = ((CR(*p) >> sh) * lv + (CG(*p) >> sh)) * lv + (CB(*p) >> sh);
            if (map[ix] != ix) *p = col[map[ix]];
        }
    }
}

/* Composite: each pixel mixed with its right neighbour (a/16), which
 * melts dither columns into the in-between shade (the "waterfall"). */
static void composite_blend(uint32_t *px, int w, int h, int a)
{
    int x, y;
    for (y = 0; y < h; y++) {
        uint32_t *r = px + (size_t)y * w;
        for (x = 0; x < w - 1; x++) {
            uint32_t p = r[x], q = r[x + 1];
            r[x] = CPK((CR(p) * (16 - a) + CR(q) * a) >> 4, (CG(p) * (16 - a) + CG(q) * a) >> 4,
                       (CB(p) * (16 - a) + CB(q) * a) >> 4);
        }
    }
}

/* Slow pixels: out = now + (prev - now) * keep / 16 (LCD smear, LED glow). */
static void persistence(uint32_t *px, size_t n, int mode, int keep)
{
    size_t i;
    if (!g_prev || g_prev_mode != mode) {
        free(g_prev);
        g_prev = (uint32_t *)malloc(n * 4);
        if (!g_prev) return;
        memcpy(g_prev, px, n * 4);
        g_prev_mode = mode;
    }
    for (i = 0; i < n; i++) {
        uint32_t p = px[i], q = g_prev[i];
        uint32_t o = CPK(CR(p) + (CR(q) - CR(p)) * keep / 16, CG(p) + (CG(q) - CG(p)) * keep / 16,
                         CB(p) + (CB(q) - CB(p)) * keep / 16);
        px[i] = o;
        g_prev[i] = o;
    }
}


static void flaws(uint32_t *px, int w, int h, int mode)
{
    int x, y;
    g_frame_no++;

    switch (mode) {
    case CF_GAMEBOY:
        /* DMG LCD: slow liquid crystal, then the dot grid (lighter gaps
         * between the 2x2 dots, pale green like the bare glass). */
        persistence(px, (size_t)w * h, mode, 9);
        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                if ((x & 1) && (y & 1)) {
                    uint32_t *p = &px[(size_t)y * w + x];
                    *p = CPK((CR(*p) * 3 + 0x8B) >> 2, (CG(*p) * 3 + 0xAC) >> 2, (CB(*p) * 3 + 0x0F) >> 2);
                }
            }
        }
        break;
    case CF_GAMEGEAR: {
        /* Game Gear LCD: 3x3 machine pixels (about 160x133 on the
         * playfield), at most 32 colours on screen (16 background + 16
         * sprite, from 4096), slow liquid crystal, then a faint dark
         * grid between the dots of the backlit panel. */
        int bx, by, i, j;
        for (by = 0; by < h; by += 3) {
            for (bx = 0; bx < w; bx += 3) {
                uint32_t c = px[(size_t)(by + 1 < h ? by + 1 : by) * w + (bx + 1 < w ? bx + 1 : bx)];
                for (j = by; j < by + 3 && j < h; j++) {
                    for (i = bx; i < bx + 3 && i < w; i++) {
                        px[(size_t)j * w + i] = c;
                    }
                }
            }
        }
        limit_colours(px, w, 0, 0, w, h, 4, 32);
        persistence(px, (size_t)w * h, mode, 8);
        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                if (x % 3 == 2 || y % 3 == 2) {
                    uint32_t *p = &px[(size_t)y * w + x];
                    *p = CPK(CR(*p) * 13 >> 4, CG(*p) * 13 >> 4, CB(*p) * 13 >> 4);
                }
            }
        }
        break;
    }
    case CF_VIRTUALBOY:
        /* Red LED scanning mirror: a short glow that fades, and the
         * column gaps of the LED line. */
        persistence(px, (size_t)w * h, mode, 6);
        for (y = 0; y < h; y++) {
            for (x = 2; x < w; x += 3) {
                uint32_t *p = &px[(size_t)y * w + x];
                *p = CPK(CR(*p) * 3 >> 2, 0, 0);
            }
        }
        break;
    case CF_PCENGINE: {
        /* 9-bit colour, each 8x8 tile limited to one 16-colour
         * sub-palette; soft composite. */
        int tx, ty;
        for (ty = 0; ty < h; ty += 16) {
            for (tx = 0; tx < w; tx += 16) {
                limit_colours(px, w, tx, ty, tx + 16 <= w ? 16 : w - tx, ty + 16 <= h ? 16 : h - ty, 3, 16);
            }
        }
        composite_blend(px, w, h, 2);
        break;
    }
    case CF_C64:
        /* PAL delay line: each line's colour averaged with the one above. */
        for (y = h - 1; y > 0; y--) {
            uint32_t *r = px + (size_t)y * w, *u = r - w;
            for (x = 0; x < w; x++) {
                uint32_t p = r[x], q = u[x];
                r[x] = CPK((CR(p) * 6 + CR(q) * 2) >> 3, (CG(p) * 6 + CG(q) * 2) >> 3,
                           (CB(p) * 6 + CB(q) * 2) >> 3);
            }
        }
        composite_blend(px, w, h, 2);
        break;
    default:
        break;
    }
}

void cfilter_apply(uint32_t *px, int w, int h, int mode)
{
    if (!px || w <= 0 || h <= 0) {
        return;
    }
    if (mode == CF_ORIGINAL) {
        g_prev_mode = -1;
        return;
    }
    if (mode == CF_C64) {
        clash(px, w, h, mode);
    } else {
        cfilter_apply_flat(px, (size_t)w * h, mode);
    }
    flaws(px, w, h, mode);
}
