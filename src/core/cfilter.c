#include "cfilter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CR(p) ((int)((p) >> 24))
#define CG(p) ((int)(((p) >> 16) & 0xFF))
#define CB(p) ((int)(((p) >> 8) & 0xFF))
#define CPK(r, g, b) (((uint32_t)(r) << 24) | ((uint32_t)(g) << 16) | ((uint32_t)(b) << 8) | 0xFFu)

/* GAME BOY and VIRTUAL BOY: six-shade luminance ramps. GAME GEAR: 4 bits
 * per channel through a backlit LCD. PC ENGINE: 3 bits per channel (512
 * colours), vivid. These are 4096-entry tables keyed on the 12-bit
 * colour. COMMODORE 64 also keeps the machine's colours-per-cell limit
 * (see clash()). Distances are "redmean" weighted RGB. */

static const char *const k_name[CF_COUNT] = {
    "ORIGINAL", "GAME BOY", "VIRTUAL BOY", "COMMODORE 64", "GAME GEAR",
    "PC ENGINE", "ZX SPECTRUM", "MSX2", "MEGA DRIVE"
};
static const char *const k_key[CF_COUNT] = {
    "original", "gameboy", "virtualboy", "c64", "gamegear", "pcengine", "zx", "msx2", "megadrive"
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

    static const char *const k_gone[] = {"spectrum", "msx1", "nes", "mastersystem"};

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
        /* a little more saturation and contrast (the GG panel is
         * small and backlit), then 4 bits per channel, full range */
        int c = v[i] + (v[i] - y) / 4;
        c = 128 + (c - 128) * 9 / 8;
        c = c < 0 ? 0 : c > 255 ? 255 : c;
        c = (c * 15 + 127) / 255;
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

        if (mode == CF_PCENGINE) sat_adj(&r, &g, &b, 360, 8);  /* strong, bright */
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

    if (!px || mode <= CF_ORIGINAL || mode >= CF_COUNT || mode == CF_ZX || mode == CF_MSX2 || mode == CF_MD) {
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
    /* pr9zj look (restored in 0.10.1): 2x2 host pixels per machine
     * pixel, 8x8 cells of them, sprites free of the cell clash. */
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


static uint32_t *g_prev;      /* last output, for LCD / LED persistence */
static int g_prev_mode = -1;
static uint32_t g_frame_no;


/* Keep only the `keep` most used colours of a frame already reduced to
 * a 2^(3*bits) colour space (bits per channel); the rest go to the
 * nearest kept one. Region x0..x0+rw, y0..y0+rh of a w-wide frame. */
#define RB_W_LAYER 640
static const uint8_t *g_lc_mask;   /* NULL, or only pixels with mask == g_lc_want */
static int g_lc_want;
static int g_lc_div;   /* 1: pick a spread set (GAME GEAR) */

static void limit_colours(uint32_t *px, int w, int x0, int y0, int rw, int rh,
                          int bits, int keep)
{
    static int cnt[4096];
    static uint32_t col[4096];
    static int16_t map[4096];
    int lv = 1 << bits, n = lv * lv * lv, sh = 8 - bits;
    int x, y, i, k, used = 0, kept[64], nk = 0, nu, ui;
    static int16_t ul[4096];

    memset(cnt, 0, sizeof(int) * (size_t)n);
    for (y = y0; y < y0 + rh; y++) {
        for (x = x0; x < x0 + rw; x++) {
            uint32_t p = px[(size_t)y * w + x];
            int ix = ((CR(p) >> sh) * lv + (CG(p) >> sh)) * lv + (CB(p) >> sh);
            if (g_lc_mask && (g_lc_mask[(size_t)y * w + x] != 0) != g_lc_want) continue;
            if (!cnt[ix]++) { col[ix] = p; used++; }
        }
    }
    if (used <= keep) return;
    if (keep > 64) keep = 64;
    for (i = 0, nu = 0; i < n; i++) if (cnt[i] > 0) ul[nu++] = (int16_t)i;
    for (k = 0; k < keep; k++) {
        int best = -1;
        long bs = -1;
        for (ui = 0; ui < nu; ui++) {
            long sc, dm = 1L << 30;
            int q;
            i = ul[ui];
            if (cnt[i] <= 0) continue;
            if (!g_lc_div || !nk) { if (best < 0 || cnt[i] > cnt[best]) best = i; continue; }
            /* spread: count weighted by distance to the kept ones, so a
             * busy hue cannot take all the slots */
            for (q = 0; q < nk; q++) {
                uint32_t u = col[kept[q]], p = col[i];
                long dr = CR(p) - CR(u), dg = CG(p) - CG(u), db = CB(p) - CB(u);
                long d = 2 * dr * dr + 4 * dg * dg + 3 * db * db;
                if (d < dm) dm = d;
            }
            sc = (long)cnt[i] * (dm < 6000 ? dm : 6000);
            if (sc > bs) { bs = sc; best = i; }
        }
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
            if (g_lc_mask && (g_lc_mask[(size_t)y * w + x] != 0) != g_lc_want) continue;
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


#define GGB 2   /* GAME GEAR dot size in host pixels */
#define LUMP(p) ((int)CR(p) * 77 + (int)CG(p) * 150 + (int)CB(p) * 29)
static void flaws(uint32_t *px, int w, int h, int mode)
{
    int x, y;
    g_frame_no++;

    switch (mode) {
    case CF_GAMEBOY:
        /* DMG LCD: the picture keeps its own pixels (no blocks), on the
         * DMG greens, with slow liquid crystal; over it a fixed grid of
         * square dots at the real LCD pitch (160 dots across the 480 px
         * playfield: one every 3 px), its dark lines clearly visible. */
        persistence(px, (size_t)w * h, mode, 9);
        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                if ((x + 3 - 80 % 3) % 3 == 2 || y % 3 == 2) {
                    uint32_t *p = &px[(size_t)y * w + x];
                    *p = CPK(CR(*p) * 140 >> 8, CG(*p) * 140 >> 8, CB(*p) * 140 >> 8);
                }
            }
        }
        break;
    case CF_GAMEGEAR: {
        /* Game Gear LCD: 2x2 machine pixels (about 240x200 on the
         * playfield, between the real 160x144 and the full picture), at most 32 colours on screen (16 background + 16
         * sprite, from 4096), slow liquid crystal, then a faint dark
         * grid between the dots of the backlit panel. */
        int bx, by, i, j;
        for (by = 0; by < h; by += GGB) {
            for (bx = 0; bx < w; bx += GGB) {
                uint32_t c;
                /* average of the block: crisp but no lost detail */
                {
                    int sr = 0, sg = 0, sb = 0, nn = 0;
                    for (j = by; j < by + GGB && j < h; j++) {
                        for (i = bx; i < bx + GGB && i < w; i++) {
                            uint32_t q = px[(size_t)j * w + i];
                            sr += CR(q); sg += CG(q); sb += CB(q); nn++;
                        }
                    }
                    /* back on the 12-bit grid (4 bits per channel) */
                    c = CPK((sr / nn + 8) / 17 * 17, (sg / nn + 8) / 17 * 17, (sb / nn + 8) / 17 * 17);
                }
                for (j = by; j < by + GGB && j < h; j++) {
                    for (i = bx; i < bx + GGB && i < w; i++) {
                        px[(size_t)j * w + i] = c;
                    }
                }
            }
        }
        /* 16 background + 16 sprite colours, counted apart so the
         * ships and bullets keep their own colours; no LCD smear (it
         * muddied the busy base sections), only a faint dot grid */
        if (g_layer && w == RB_W_LAYER) {
            g_lc_mask = g_layer;
            g_lc_want = 0;
            g_lc_div = 1;
            limit_colours(px, w, 0, 0, w, h, 4, 16);
            g_lc_want = 1;
            limit_colours(px, w, 0, 0, w, h, 4, 15);   /* sprite entry 0 is transparent */
            g_lc_mask = NULL;
            g_lc_div = 0;
        } else {
            limit_colours(px, w, 0, 0, w, h, 4, 31);
        }
        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                if (x % GGB == GGB - 1 || y % GGB == GGB - 1) {
                    uint32_t *p = &px[(size_t)y * w + x];
                    *p = CPK(CR(*p) * 237 >> 8, CG(*p) * 237 >> 8, CB(*p) * 237 >> 8);   /* gaps 7.4 % down (was 6.25 %: +20 %) */
                }
            }
        }
        break;
    }
    case CF_VIRTUALBOY:
        /* Red LED scanning mirror: a short glow that fades, then the
         * same clean square dot grid as GAME BOY / GAME GEAR (the last
         * column and row of every 2x2 dot a little darker). */
        persistence(px, (size_t)w * h, mode, 6);
        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                if (x % GGB == GGB - 1 || y % GGB == GGB - 1) {
                    uint32_t *p = &px[(size_t)y * w + x];
                    *p = CPK(CR(*p) * 5 >> 3, 0, 0);
                }
            }
        }
        break;
    case CF_PCENGINE: {
        /* 9-bit colour, each 8x8 tile limited to one 16-colour
         * sub-palette; soft composite. */
        /* HuC6260 VCE: background tiles are 8x8 machine pixels with one
         * of 16 sub-palettes of 16 (entry 0 shared), sprites 16x16 with
         * one sub-palette of 15 + transparent. On the game's own pixels
         * (2 host pixels wide) a tile is 16 and a sprite cell 32 host
         * pixels, from the playfield's left edge (x 80). */
        int tx, ty, t = 16, sc = 32, x0 = w >= 560 ? 80 : 0, x1 = w >= 560 ? 560 : w;
        size_t i, n = (size_t)w * h;

        /* same grid as MEGA DRIVE: 240 columns of two host pixels over
         * the playfield, all 400 lines (no resampling, no wobble) */
        for (y = 0; y < h; y++) {
            uint32_t *r = px + (size_t)y * w;

            for (x = x0; x + 1 < x1; x += 2) {
                uint32_t p = r[x], q = r[x + 1];
                uint32_t m = CPK((CR(p) + CR(q) + 1) >> 1, (CG(p) + CG(q) + 1) >> 1, (CB(p) + CB(q) + 1) >> 1);

                r[x] = r[x + 1] = m;
            }
        }

        g_lc_mask = g_layer;
        g_lc_want = 0;
        for (ty = 0; ty < h; ty += t) {
            for (tx = x0; tx < x1; tx += t) {
                limit_colours(px, w, tx, ty, tx + t <= x1 ? t : x1 - tx, ty + t <= h ? t : h - ty, 3, 16);
            }
        }
        if (g_layer) {
            g_lc_want = 1;
            for (ty = 0; ty < h; ty += sc) {
                for (tx = x0; tx < x1; tx += sc) {
                    limit_colours(px, w, tx, ty, tx + sc <= x1 ? sc : x1 - tx, ty + sc <= h ? sc : h - ty, 3, 15);
                }
            }
        }
        g_lc_mask = NULL;
        composite_blend(px, w, h, 2);
        /* the blend mixes neighbours; put every pixel back on the
         * 512-colour grid (3 bits per channel, v * 255 / 7) */
        for (i = 0; i < n; i++) {
            uint32_t p = px[i];
            int r = ((int)CR(p) * 7 + 127) / 255, g = ((int)CG(p) * 7 + 127) / 255,
                b = ((int)CB(p) * 7 + 127) / 255;

            px[i] = CPK(r * 255 / 7, g * 255 / 7, b * 255 / 7);
        }
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

/* ---- ZX SPECTRUM ---------------------------------------------------
 * Modelled on R-Type for the 128K Spectrum: a black screen, scenery in
 * the ZX attribute colours (one ink per 8x8 cell, from the cell's own
 * main colour) and every object drawn solid in black plus ONE bright
 * ZX colour, its own predominant colour, with the shading kept as black
 * detail lines. Sprites are opaque: nothing of the background shows
 * through them. Objects are found as connected sprite pixels. */
static const uint32_t k_zx[15] = {
    0x000000u, 0x0000D7u, 0xD70000u, 0xD700D7u, 0x00D700u, 0x00D7D7u, 0xD7D700u, 0xD7D7D7u,
    0x0000FFu, 0xFF0000u, 0xFF00FFu, 0x00FF00u, 0x00FFFFu, 0xFFFF00u, 0xFFFFFFu};
static const uint8_t *g_bgl;
static int32_t g_zx_lab[480 * 400];
static int32_t g_zx_stk[480 * 400];
static uint8_t g_zx_obj[480 * 400];
static uint8_t g_zx_l[480 * 400];   /* luma of this frame */

void cfilter_set_bg(const uint8_t *luma)
{
    g_bgl = luma;   /* kept for the API; the ZX look no longer needs it */
}

/* nearest ZX colour (1..14, never black); "avoid" is skipped */
static int zx_near(int r, int g, int b, int avoid)
{
    int k, best = 14;
    long bd = -1;

    for (k = 1; k < 15; k++) {
        long d = redmean(r, g, b, k_zx[k]);

        if (k >= 8) {
            d -= d / 8;   /* bright set wins ties: R-Type is all BRIGHT */
        }
        if (k == avoid || (avoid > 0 && (k_zx[k] == k_zx[avoid]))) {
            continue;
        }
        if (bd < 0 || d < bd) {
            bd = d;
            best = k;
        }
    }
    return best;
}

/* ZX colour of one pixel's hue (min removed, max stretched); grey -> white */
static int zx_hue(int r, int g, int b, int *chroma)
{
    int mx = r > g ? (r > b ? r : b) : (g > b ? g : b);
    int mn = r < g ? (r < b ? r : b) : (g < b ? g : b);

    *chroma = mx - mn;
    if (mx - mn < 20) {
        return 14;
    }
    return zx_near((r - mn) * 255 / (mx - mn), (g - mn) * 255 / (mx - mn),
                   (b - mn) * 255 / (mx - mn), 0);
}

/* 12-bit colour -> ZX hue index and chroma, built once */
static uint8_t g_zx_hl[4096], g_zx_cl[4096];
static int g_zx_hl_ok;

static void zx_hl_build(void)
{
    int k, ch;

    for (k = 0; k < 4096; k++) {
        g_zx_hl[k] = (uint8_t)zx_hue(((k >> 8) & 15) * 17, ((k >> 4) & 15) * 17, (k & 15) * 17, &ch);
        g_zx_cl[k] = (uint8_t)ch;
    }
    g_zx_hl_ok = 1;
}

#define ZX_K12(p) ((int)(((p) >> 20) & 0xF00u) | (int)(((p) >> 16) & 0xF0u) | (int)(((p) >> 12) & 0xFu))

static int zx_lum(uint32_t p)
{
    return ((int)(p >> 24) * 3 + (int)((p >> 16) & 0xFF) * 6 + (int)((p >> 8) & 0xFF)) / 10;
}

/* Otsu threshold over a 32-bin luma histogram */
static int zx_otsu(const long *hist, long n)
{
    long sum = 0, wb = 0, sb = 0;
    int k, t = 64;
    double best = -1;

    for (k = 0; k < 32; k++) {
        sum += hist[k] * k;
    }
    for (k = 0; k < 31; k++) {
        double mb, mf, v;

        wb += hist[k];
        sb += hist[k] * k;
        if (!wb || wb == n) {
            continue;
        }
        mb = (double)sb / wb;
        mf = (double)(sum - sb) / (n - wb);
        v = (double)wb * (n - wb) * (mb - mf) * (mb - mf);
        if (v > best) {
            best = v;
            t = k * 8 + 8;
        }
    }
    return t;
}

static const uint8_t *g_kind;
void cfilter_set_kind(const uint8_t *kind) { g_kind = kind; }
static int g_cf_stage = 1;
void cfilter_set_stage(int stage) { g_cf_stage = stage; }

static __attribute__((unused)) void zx_apply(uint32_t *px, int w, int h)
{
    static const int8_t b4[16] = {0, 8, 2, 10, 12, 4, 14, 6, 3, 11, 1, 9, 15, 7, 13, 5};
    int x, y, X0 = 80, W = w - 160, bgc[60 * 50];
    int nlab = 0, H = h > 400 ? 400 : h;

    if (W > 480) {
        W = 480;
    }
    if (W <= 0) {
        return;
    }
    if (!g_zx_hl_ok) {
        zx_hl_build();
    }
    for (y = 0; y < H; y++) {
        for (x = 0; x < W; x++) {
            g_zx_l[y * 480 + x] = (uint8_t)zx_lum(px[(size_t)y * w + x + X0]);
        }
    }
    /* background: one ink per 8x8 attribute cell */
    for (y = 0; y < H; y += 8) {
        for (x = 0; x < W; x += 8) {
            long n = 0, hc[15] = {0};
            int yy, xx, k, bk = 0;

            for (yy = y; yy < y + 8 && yy < H; yy++) {
                for (xx = x; xx < x + 8 && xx < W; xx++) {
                    size_t i = (size_t)yy * w + xx + X0;
                    uint32_t p = px[i];

                    if ((g_layer && g_layer[i]) || g_zx_l[yy * 480 + xx] < 24) {
                        continue;
                    }
                    {
                        int k12 = ZX_K12(p), hk = g_zx_hl[k12];

                        hc[hk] += hk == 14 ? 4 : 4 + g_zx_cl[k12];
                    }
                    n++;
                }
            }
            for (k = 1; k < 15; k++) {
                if (hc[k] > hc[bk]) {
                    bk = k;
                }
            }
            bgc[(y / 8) * 60 + x / 8] = n ? bk : 0;
        }
    }
    /* label objects (4-connected sprite pixels) */
    memset(g_zx_lab, 0, sizeof g_zx_lab);
    for (y = 0; y < H; y++) {
        for (x = 0; x < W; x++) {
            size_t i = (size_t)y * w + x + X0;
            int sp, cnt = 0;
            long hist[32], hc[15];
            int thr, col, best, k;

            if (!g_layer || !g_layer[i] || g_zx_lab[y * 480 + x]) {
                continue;
            }
            memset(hist, 0, sizeof hist);
            memset(hc, 0, sizeof hc);
            nlab++;
            sp = 0;
            g_zx_stk[sp++] = y * 480 + x;
            g_zx_lab[y * 480 + x] = nlab;
            while (sp) {
                int q = g_zx_stk[--sp], qy = q / 480, qx = q % 480, d;
                uint32_t p = px[(size_t)qy * w + qx + X0];
                int l = g_zx_l[q];

                g_zx_stk[480 * 400 - 1 - cnt] = q;   /* member list from the top */
                cnt++;
                hist[l >> 3]++;
                if (l >= 40) {
                    int r = (int)(p >> 24), g = (int)((p >> 16) & 0xFF), b = (int)((p >> 8) & 0xFF);
                    int k12 = ZX_K12(p), hk = g_zx_hl[k12];

                    (void)r; (void)g; (void)b;
                    hc[hk] += hk == 14 ? 4 : 4 + g_zx_cl[k12];   /* coloured pixels weigh more */
                }
                for (d = 0; d < 4; d++) {
                    int nx = qx + (d == 0) - (d == 1), ny = qy + (d == 2) - (d == 3);

                    if (nx < 0 || ny < 0 || nx >= W || ny >= H) {
                        continue;
                    }
                    if (g_layer[(size_t)ny * w + nx + X0] && !g_zx_lab[ny * 480 + nx] &&
                        sp + cnt < 480 * 400 - 1) {
                        g_zx_lab[ny * 480 + nx] = nlab;
                        g_zx_stk[sp++] = ny * 480 + nx;
                    }
                }
            }
            /* predominant colour, by pixel count */
            for (k = 1, best = 14, col = 0; k < 15; k++) {
                if (hc[k] > col) {
                    col = (int)hc[k];
                    best = k;
                }
            }
            /* readable against the scenery under it */
            {
                int cy = y / 8, cx = x / 8;

                if (bgc[cy * 60 + cx] && (k_zx[bgc[cy * 60 + cx]] == k_zx[best] ||
                                          (best % 7) == (bgc[cy * 60 + cx] % 7))) {
                    int r = (int)(k_zx[best] >> 16), g = (int)((k_zx[best] >> 8) & 0xFF),
                        b = (int)(k_zx[best] & 0xFF);

                    best = zx_near(r, g, b, best);
                    if ((best % 7) == (bgc[cy * 60 + cx] % 7)) {
                        best = zx_near(r, g, b, best);
                    }
                }
            }
            thr = zx_otsu(hist, cnt);
            if (thr < 24) {
                thr = 24;
            }
            for (k = 0; k < cnt; k++) {
                int q = g_zx_stk[480 * 400 - 1 - k], qy = q / 480, qx = q % 480;
                int l = g_zx_l[q];
                /* edge-aware: a pixel darker than its neighbours is a line */
                int ln = l, cnt2 = 0, d;
                long acc = 0;

                for (d = 0; d < 4; d++) {
                    int nx = qx + (d == 0) - (d == 1), ny = qy + (d == 2) - (d == 3);

                    if (nx >= 0 && ny >= 0 && nx < W && ny < H) {
                        acc += g_zx_l[ny * 480 + nx];
                        cnt2++;
                    }
                }
                if (cnt2) {
                    ln = (int)(acc / cnt2);
                }
                if (l < 16 || l + 28 < ln) {
                    g_zx_obj[q] = 0;          /* outline / dark line */
                } else if (l >= thr + 24) {
                    g_zx_obj[q] = (uint8_t)best;
                } else if (l < thr - 24) {
                    g_zx_obj[q] = 0;
                } else {
                    /* mid-tone: ordered dither, inside the sprite only */
                    int t = thr + (b4[(qy & 3) * 4 + (qx & 3)] - 8) * 3;

                    g_zx_obj[q] = (uint8_t)(l > t ? best : 0);
                }
            }
            if (nlab > 30000) {
                goto done;
            }
        }
    }
done:
    for (y = 0; y < H; y++) {
        for (x = 0; x < W; x++) {
            size_t i = (size_t)y * w + x + X0;

            if (g_layer && g_layer[i] && g_zx_lab[y * 480 + x]) {
                px[i] = k_zx[g_zx_obj[y * 480 + x]] << 8 | 0xFFu;
            } else {
                int c = bgc[(y / 8) * 60 + x / 8];

                px[i] = (c && g_zx_l[y * 480 + x] >= 40) ? (k_zx[c] << 8 | 0xFFu) : 0x000000FFu;
            }
        }
    }
}

/* ---- ZX SPECTRUM (screen filter) -----------------------------------
 * The whole playfield is treated as one picture: lowres() boxes it to
 * 192 lines of square pixels (the Spectrum's 256x192 grid), each pixel
 * becomes the nearest of the 15 Spectrum colours (perceptual distance,
 * brightness lifted 10 %), no dithering and no attribute cells; then a
 * very light barrel curve and a soft vignette. All tables are built
 * once. */
static uint8_t g_zs_lut[4096];
static int32_t g_zs_map[480 * 400];
static uint8_t g_zs_vig[480 * 400];
static int g_zs_ok;
static uint32_t g_zs_tmp[480 * 400];

static void zs_build(void)
{
    int k, x, y;

    for (k = 0; k < 4096; k++) {
        int r = ((k >> 8) & 15) * 17, g = ((k >> 4) & 15) * 17, b = (k & 15) * 17, j, best = 0;
        long bd = -1;

        r = c8s(r * 11 / 10); g = c8s(g * 11 / 10); b = c8s(b * 11 / 10);
        for (j = 0; j < 15; j++) {
            long d = redmean(r, g, b, k_zx[j]);

            if (bd < 0 || d < bd) {
                bd = d;
                best = j;
            }
        }
        g_zs_lut[k] = (uint8_t)best;
    }
    for (y = 0; y < 400; y++) {
        for (x = 0; x < 480; x++) {
            double nx = (x + 0.5) / 240.0 - 1.0, ny = (y + 0.5) / 200.0 - 1.0;
            double r2 = nx * nx + ny * ny, f = 1.0 - 0.01 * r2 * 2.0;  /* barrel ~0.01 */
            int sx = (int)((nx * f + 1.0) * 240.0), sy = (int)((ny * f + 1.0) * 200.0);
            double v = 1.0 - 0.28 * r2 * r2;                            /* soft vignette */

            sx = sx < 0 ? 0 : sx > 479 ? 479 : sx;
            sy = sy < 0 ? 0 : sy > 399 ? 399 : sy;
            g_zs_map[y * 480 + x] = sy * 480 + sx;
            g_zs_vig[y * 480 + x] = (uint8_t)(v < 0.5 ? 128 : v * 256 > 255 ? 255 : v * 256);
        }
    }
    g_zs_ok = 1;
}

static __attribute__((unused)) void zx_screen(uint32_t *px, int w, int h)
{
    int x, y, X0 = 80, H = h > 400 ? 400 : h;

    if (w < 560) {
        return;
    }
    if (!g_zs_ok) {
        zs_build();
    }
    for (y = 0; y < H; y++) {
        const uint32_t *row = px + (size_t)y * w + X0;
        uint32_t *o = g_zs_tmp + y * 480;

        for (x = 0; x < 480; x++) {
            o[x] = k_zx[g_zs_lut[ZX_K12(row[x])]];
        }
    }
    for (y = 0; y < H; y++) {
        uint32_t *row = px + (size_t)y * w + X0;

        for (x = 0; x < 480; x++) {
            int q = y * 480 + x;
            uint32_t c = g_zs_tmp[g_zs_map[q] < H * 480 ? g_zs_map[q] : q];
            unsigned v = g_zs_vig[q];

            row[x] = (((c >> 16) & 0xFF) * v >> 8) << 24 | (((c >> 8) & 0xFF) * v >> 8) << 16 |
                     ((c & 0xFF) * v >> 8) << 8 | 0xFFu;
        }
    }
}

/* ---- Machine resolution ------------------------------------------
 * The playfield (x 80..559, 480x400) is read as a grid of machine
 * pixels "lines" tall and as many wide as square pixels give (no
 * stretching), each the average of the host pixels it covers, then
 * drawn back nearest-neighbour. The HUD text is painted after. */
static __attribute__((unused)) void lowres(uint32_t *px, int w, int h, int lines)
{
    float s = (float)h / (float)lines;
    int gw = (int)(480.0f / s + 0.5f), mx, my;

    if (w < 560 || s <= 1.0f) {
        return;
    }
    for (my = 0; my < lines; my++) {
        int y0 = (int)(my * s), y1 = (int)((my + 1) * s);

        y1 = y1 > h ? h : y1 <= y0 ? y0 + 1 : y1;
        for (mx = 0; mx < gw; mx++) {
            int x0 = 80 + (int)(mx * s), x1 = 80 + (int)((mx + 1) * s), x, y;
            unsigned r = 0, g = 0, b = 0, n = 0;
            uint32_t c;

            x1 = x1 > 560 ? 560 : x1 <= x0 ? x0 + 1 : x1;
            if (x0 >= 560) {
                break;
            }
            for (y = y0; y < y1; y++) {
                for (x = x0; x < x1; x++) {
                    uint32_t p = px[(size_t)y * w + x];

                    r += p >> 24;
                    g += (p >> 16) & 0xFF;
                    b += (p >> 8) & 0xFF;
                    n++;
                }
            }
            c = (r / n) << 24 | (g / n) << 16 | (b / n) << 8 | 0xFFu;
            for (y = y0; y < y1; y++) {
                for (x = x0; x < x1; x++) {
                    px[(size_t)y * w + x] = c;
                }
            }
        }
    }
}

/* ---- Machine grid shared by ZX SPECTRUM and MSX2 ------------------
 * The playfield (x 80..559, 480x400) box-averaged into "lines" rows
 * of square machine pixels (gw wide), and drawn back nearest-neighbour
 * with the same cell edges. */
#define MG_MAX (320 * 400)
static uint32_t g_mg[MG_MAX];
static uint8_t g_mg_spr[MG_MAX];
static int g_mg_x[321], g_mg_y[401];
static int g_mg_gwf;   /* columns over the playfield, 0 = square pixels */

static int mg_read(const uint32_t *px, int w, int h, int lines, int *gwp)
{
    float s = (float)h / (float)lines, sx;
    int gw = g_mg_gwf ? g_mg_gwf : (int)(480.0f / s + 0.5f), mx, my;

    if (w < 560 || s < 1.0f || gw > 320) {
        return 0;
    }
    sx = 480.0f / (float)gw;
    for (mx = 0; mx <= gw; mx++) {
        int x = 80 + (int)(mx * sx);
        g_mg_x[mx] = x > 560 ? 560 : x;
    }
    g_mg_x[gw] = 560;
    for (my = 0; my <= lines; my++) {
        int y = (int)(my * s);
        g_mg_y[my] = y > h ? h : y;
    }
    for (my = 0; my < lines; my++) {
        int y0 = g_mg_y[my], y1 = g_mg_y[my + 1] > y0 ? g_mg_y[my + 1] : y0 + 1;

        for (mx = 0; mx < gw; mx++) {
            int x0 = g_mg_x[mx], x1 = g_mg_x[mx + 1] > x0 ? g_mg_x[mx + 1] : x0 + 1, x, y;
            unsigned r = 0, g = 0, b = 0, n = 0, sp = 0;

            for (y = y0; y < y1 && y < h; y++) {
                const uint32_t *row = px + (size_t)y * w;

                for (x = x0; x < x1; x++) {
                    uint32_t p = row[x];

                    r += p >> 24;
                    g += (p >> 16) & 0xFF;
                    b += (p >> 8) & 0xFF;
                    n++;
                    if (g_layer && g_layer[(size_t)y * w + x]) {
                        sp++;
                    }
                }
            }
            if (!n) {
                n = 1;
            }
            g_mg[my * gw + mx] = (r / n) << 16 | (g / n) << 8 | (b / n);
            g_mg_spr[my * gw + mx] = sp * 2 >= n;
        }
    }
    *gwp = gw;
    return 1;
}

static void mg_write(uint32_t *px, int w, int h, int lines, int gw)
{
    int mx, my;

    for (my = 0; my < lines; my++) {
        int y, y0 = g_mg_y[my], y1 = g_mg_y[my + 1] > y0 ? g_mg_y[my + 1] : y0 + 1;

        for (y = y0; y < y1 && y < h; y++) {
            uint32_t *row = px + (size_t)y * w;

            for (mx = 0; mx < gw; mx++) {
                uint32_t c = g_mg[my * gw + mx] << 8 | 0xFFu;
                int x;

                for (x = g_mg_x[mx]; x < g_mg_x[mx + 1]; x++) {
                    row[x] = c;
                }
            }
        }
    }
}

/* ---- ZX SPECTRUM: attribute clash --------------------------------
 * 192 lines (the 256x192 screen's scale); every 8x8 cell gets one INK
 * and one PAPER from the 8 base colours with one shared BRIGHT bit,
 * and each pixel is one bit: ink or paper. The pair is the one with the
 * least error over the cell, sprite pixels weighted up (so a ship takes
 * the cell's colours and the scenery under it changes: the clash),
 * with a bias toward black paper. Pixels pick ink or paper by distance,
 * but a pixel much darker or lighter than its neighbours keeps that
 * side, and a sprite pixel next to scenery is pushed to the other
 * colour, so outlines and detail survive. */
static const uint32_t k_zx16[16] = {
    0x000000u, 0x0000D7u, 0xD70000u, 0xD700D7u, 0x00D700u, 0x00D7D7u, 0xD7D700u, 0xD7D7D7u,
    0x000000u, 0x0000FFu, 0xFF0000u, 0xFF00FFu, 0x00FF00u, 0x00FFFFu, 0xFFFF00u, 0xFFFFFFu};
static uint16_t g_zc_d[4096][16];
static int g_zc_ok;

#define K24_12(p) ((int)(((p) >> 12) & 0xF00u) | (int)(((p) >> 8) & 0xF0u) | (int)(((p) >> 4) & 0xFu))
#define LUM24(p) ((int)((((p) >> 16) & 0xFF) * 77 + (((p) >> 8) & 0xFF) * 150 + ((p) & 0xFF) * 29) >> 8)

static void zc_build(void)
{
    int k, j;

    for (k = 0; k < 4096; k++) {
        int r = ((k >> 8) & 15) * 17, g = ((k >> 4) & 15) * 17, b = (k & 15) * 17;

        r = c8s(r * 11 / 10); g = c8s(g * 11 / 10); b = c8s(b * 11 / 10);
        for (j = 0; j < 16; j++) {
            long d = redmean(r, g, b, k_zx16[j]) >> 6;

            g_zc_d[k][j] = (uint16_t)(d > 65535 ? 65535 : d);
        }
    }
    g_zc_ok = 1;
}

static void zx_clash(uint32_t *px, int w, int h)
{
    int gw, cx, cy;
    static uint8_t lum[MG_MAX];
    static uint8_t key[MG_MAX];

    if (!mg_read(px, w, h, 192, &gw)) {
        return;
    }
    if (!g_zc_ok) {
        zc_build();
    }
    {
        int i;
        for (i = 0; i < gw * 192; i++) {
            lum[i] = (uint8_t)LUM24(g_mg[i]);
        }
    }
    for (cy = 0; cy < 192; cy += 8) {
        for (cx = 0; cx < gw; cx += 8) {
            int ix[64], wt[64], n = 0, i, br, pp, qq, bestA = 0, bestB = 0, cw = gw - cx < 8 ? gw - cx : 8;
            long best = -1;
            uint8_t side[64];
            int dd[64][16];

            for (i = 0; i < 64; i++) {
                int yy = cy + i / 8, xx = cx + i % 8;

                if (i % 8 >= cw) {
                    continue;
                }
                ix[n] = yy * gw + xx;
                wt[n] = g_mg_spr[ix[n]] ? 3 : 1;
                key[ix[n]] = 0;
                n++;
            }
            for (i = 0; i < n; i++) {
                const uint16_t *d = g_zc_d[K24_12(g_mg[ix[i]])];
                int j;

                for (j = 0; j < 16; j++) {
                    dd[i][j] = d[j];
                }
            }
            for (br = 0; br < 2; br++) {
                int base = br * 8, cand = 1;   /* black always a candidate */

                /* only colours that are some pixel's nearest can win */
                for (i = 0; i < n; i++) {
                    int j, bj = 0;

                    for (j = 1; j < 8; j++) {
                        if (dd[i][base + j] < dd[i][base + bj]) {
                            bj = j;
                        }
                    }
                    cand |= 1 << bj;
                }
                for (pp = 0; pp < 8; pp++) {
                    if (!(cand >> pp & 1)) {
                        continue;
                    }
                    for (qq = pp; qq < 8; qq++) {
                        long e = 0;

                        if (!(cand >> qq & 1)) {
                            continue;
                        }

                        for (i = 0; i < n; i++) {
                            int a = dd[i][base + pp], b = dd[i][base + qq];

                            e += (long)wt[i] * (a < b ? a : b);
                        }
                        if (pp != 0) {
                            e = e * 6 / 5 + 8 * n;   /* black paper bias */
                        }
                        if (best < 0 || e < best) {
                            best = e;
                            bestA = base + pp;
                            bestB = base + qq;
                        }
                    }
                }
            }
            /* assign: by distance, then edge rules */
            {
                int lA = LUM24(k_zx16[bestA]), lB = LUM24(k_zx16[bestB]);
                int dk = lA <= lB ? 0 : 1;   /* side 0 = A, the darker */

                for (i = 0; i < n; i++) {
                    const uint16_t *d = g_zc_d[K24_12(g_mg[ix[i]])];
                    int q = ix[i], y = q / gw, x = q % gw, m = 0, c = 0, l = lum[q];

                    side[i] = d[bestB] < d[bestA];
                    if (bestA == bestB) {
                        continue;
                    }
                    if (x > 0) { m += lum[q - 1]; c++; }
                    if (x < gw - 1) { m += lum[q + 1]; c++; }
                    if (y > 0) { m += lum[q - gw]; c++; }
                    if (y < 191) { m += lum[q + gw]; c++; }
                    m = c ? m / c : l;
                    if (l < m - 36) {
                        side[i] = (uint8_t)dk;          /* dark detail line */
                    } else if (l > m + 36) {
                        side[i] = (uint8_t)!dk;         /* highlight */
                    }
                }
                /* sprite edge against scenery: keep the silhouette */
                for (i = 0; i < n; i++) {
                    int q = ix[i], x = q % gw, j;

                    if (!g_mg_spr[q] || bestA == bestB) {
                        continue;
                    }
                    for (j = 0; j < n; j++) {
                        int q2 = ix[j];

                        if (!g_mg_spr[q2] && (q2 == q - 1 || q2 == q + 1 || q2 == q - gw || q2 == q + gw) &&
                            side[j] == side[i] && (x > 0 || q2 != q - 1)) {
                            side[i] = (uint8_t)!side[j];
                            break;
                        }
                    }
                }
                for (i = 0; i < n; i++) {
                    g_mg[ix[i]] = k_zx16[side[i] ? bestB : bestA];
                }
            }
        }
    }
    mg_write(px, w, h, 192, gw);
}

/* ---- ZX SPECTRUM, world-locked attributes -------------------------
 * Machine pixels are 2x2 host pixels and attribute cells 16x16 host
 * pixels (8x8 machine pixels), both on the scrolling page's own grid,
 * so the scenery's colours move with it and never crawl. Each scenery
 * cell gets INK + PAPER from the 8 base colours with one BRIGHT bit
 * (black paper preferred), worked out once from the cell's art and
 * kept until that art (or the palette) changes. Scenery stays on the
 * dark (non-BRIGHT) colours. Every sprite family gets one BRIGHT ink,
 * voted over its first frames and then fixed for good; its pixels are
 * that ink or black, whatever it overlaps, so a sprite never changes
 * colour. Stars stay white (a later pass). */
static const uint16_t *g_w_sid;
static const uint8_t *g_w_bgi;
static int g_w_scroll;

static const uint8_t *g_w_txt;
static const uint32_t *g_w_pre;

void cfilter_set_text(const uint8_t *mask, const uint32_t *pre)
{
    g_w_txt = mask;
    g_w_pre = pre;
}

void cfilter_set_world(const uint16_t *sid, const uint8_t *bgi, int scroll)
{
    g_w_sid = sid;
    g_w_bgi = bgi;
    g_w_scroll = scroll;
}

#define ZW_W 240
#define ZW_H 200
#define ZW_CW 30
#define ZW_CH 25
static uint32_t g_zw_sum[ZW_W * ZW_H][3];
static uint8_t g_zw_n[ZW_W * ZW_H], g_zw_sn[ZW_W * ZW_H];
static uint8_t g_zw_bd[ZW_W * ZW_H];   /* backdrop (index 0) pixels */
static uint8_t g_zw_dim[ZW_CW * ZW_CH];
static uint16_t g_zw_id[ZW_W * ZW_H];
static uint32_t g_zw_hash[ZW_CW * ZW_CH];
static uint8_t g_zw_ink[ZW_CW * ZW_CH], g_zw_pap[ZW_CW * ZW_CH];
static uint8_t g_zw_ok[ZW_CW * ZW_CH];
static uint32_t g_zw_col[ZW_W * ZW_H];
static uint32_t g_zw_out[ZW_W * ZW_H];
static uint32_t g_zw_scn[ZW_W * ZW_H]; /* scenery colour under a 2x2, for 1 px holes */
static uint8_t g_zw_spr[ZW_W * ZW_H];
static uint8_t g_tblk[ZW_W * ZW_H];
/* sprite family colour: votes over the first 12 frames, then locked */
static uint8_t g_sf_ink[65536];
static uint8_t g_sf_state[65536];     /* 0 new, 1..12 voting, 13 locked */
static uint16_t g_sf_vote[65536][8];
static int g_big_stage = -1;
static int g_cr_n; /* how many stage-2 cruisers locked */
static uint16_t g_cr_id[2]; /* sprite/stamp ids; [0] dark yellow, [1] cyan */
static int g_hull_box[2][4]; /* page-space x0,y0,x1,y1 of a tile cruiser */
static uint32_t g_zw_palh;
static uint8_t g_zw_tag[ZW_W * ZW_H]; /* 1 enemy, 2 item/shot, 3 enemy bullet */
static uint8_t g_zw_bgc[ZW_W * ZW_H]; /* spectrum index behind this pixel */
static int g_zx_space;

/* Nearest non-BRIGHT Spectrum colour, 0..7. */
static int zx_dark_near(uint32_t rgb)
{
    const uint16_t *d = g_zc_d[K24_12(rgb)];
    int j, bj = 0;

    for (j = 1; j < 8; j++) {
        if (d[j] < d[bj]) {
            bj = j;
        }
    }
    return bj;
}

/* Black backdrop plus white star dots. */
static int zx_is_space(uint32_t idx0)
{
    int r = (int)(idx0 >> 24), g = (int)((idx0 >> 16) & 255), b = (int)((idx0 >> 8) & 255);
    int y = (r * 77 + g * 150 + b * 29) >> 8;
    int stars = 0, dark = 0, i;

    if (idx0 && y > 36) {
        return 0;
    }
    for (i = 0; i < ZW_W * ZW_H; i++) {
        int l, rr, gg, bb, hi, lo, mx;

        if (g_zw_spr[i] || !g_zw_n[i]) {
            continue;
        }
        l = LUM24(g_zw_col[i]);
        if (l < 24) {
            dark++;
            continue;
        }
        /* a 1 px star averaged into a 2x2 is only mid-grey, and neutral */
        if (l < 40 || l > 210) {
            continue;
        }
        rr = (int)(g_zw_col[i] >> 16);
        gg = (int)((g_zw_col[i] >> 8) & 255);
        bb = (int)(g_zw_col[i] & 255);
        hi = rr > gg ? (rr > bb ? rr : bb) : (gg > bb ? gg : bb);
        lo = rr < gg ? (rr < bb ? rr : bb) : (gg < bb ? gg : bb);
        if (hi - lo > 36) {
            continue;
        }
        mx = i % ZW_W;
        if ((mx > 0 && LUM24(g_zw_col[i - 1]) + 24 < l) ||
            (mx + 1 < ZW_W && LUM24(g_zw_col[i + 1]) + 24 < l)) {
            stars++;
        }
    }
    return dark > 350 && stars >= 4;
}

/* Ordered dither: one ink and one paper per 8x8 cell, mixed in a Bayer
 * pattern at the density that matches the original colour. Pairs are a
 * colour with black, with white, or with its bright/dark twin, which is
 * how a missing semitone is faked. Sprites are not dithered. */
#define ZX_NP 40
static uint8_t g_pa[ZX_NP], g_pb[ZX_NP], g_pcloud[ZX_NP];
static int g_pn;
static uint8_t g_lut_p[4096], g_lut_t[4096], g_lut_cp[4096], g_lut_ct[4096];
static int g_lut_ok;
static const uint8_t k_bayer[64] = {
    0, 32, 8, 40, 2, 34, 10, 42, 48, 16, 56, 24, 50, 18, 58, 26,
    12, 44, 4, 36, 14, 46, 6, 38, 60, 28, 52, 20, 62, 30, 54, 22,
    3, 35, 11, 43, 1, 33, 9, 41, 51, 19, 59, 27, 49, 17, 57, 25,
    15, 47, 7, 39, 13, 45, 5, 37, 63, 31, 55, 23, 61, 29, 53, 21
};

static int zx_mix_err(int r, int g, int b, int ia, int ib, int t)
{
    int ar = (int)(k_zx16[ia] >> 16), ag = (int)((k_zx16[ia] >> 8) & 255), ab = (int)(k_zx16[ia] & 255);
    int br = (int)(k_zx16[ib] >> 16), bg = (int)((k_zx16[ib] >> 8) & 255), bb = (int)(k_zx16[ib] & 255);
    int mr = (ar * (8 - t) + br * t) / 8, mg = (ag * (8 - t) + bg * t) / 8, mb = (ab * (8 - t) + bb * t) / 8;

    return (int)redmean(r, g, b, (uint32_t)mr << 16 | (uint32_t)mg << 8 | (uint32_t)mb);
}

static void zx_lut_build(void)
{
    int c, i = 0, k;

    for (c = 1; c <= 15; c++) {
        g_pa[i] = 0;
        g_pb[i] = (uint8_t)c;
        g_pcloud[i] = (uint8_t)(c == 1 || c == 5 || c == 7 || c == 9 || c == 13 || c == 15);
        i++;
    }
    for (c = 1; c <= 14; c++) {
        g_pa[i] = (uint8_t)c;
        g_pb[i] = 15;
        g_pcloud[i] = (uint8_t)(c == 1 || c == 5 || c == 7 || c == 9 || c == 13);
        i++;
    }
    for (c = 1; c <= 7; c++) {
        g_pa[i] = (uint8_t)c;
        g_pb[i] = (uint8_t)(c + 8);
        g_pcloud[i] = (uint8_t)(c == 1 || c == 5 || c == 7);
        i++;
    }
    g_pn = i;
    for (k = 0; k < 4096; k++) {
        int r = ((k >> 8) & 15) * 17, g = ((k >> 4) & 15) * 17, b = (k & 15) * 17;
        int p, best = 0, bt = 0, be = -1, cb = 0, ct = 0, ce = -1;

        for (p = 0; p < g_pn; p++) {
            int t, te = -1, tt = 0;

            for (t = 0; t <= 8; t++) {
                int e = zx_mix_err(r, g, b, g_pa[p], g_pb[p], t);

                if (te < 0 || e < te) {
                    te = e;
                    tt = t;
                }
            }
            {
                /* Scenery stays on the dark colours. A tinted pixel must
                 * not collapse to black+white just because that line is
                 * nearer in luminance: keep the hue. */
                int mx = r > g ? (r > b ? r : b) : (g > b ? g : b);
                int mn = r < g ? (r < b ? r : b) : (g < b ? g : b);
                int grey = g_pa[p] == 0 && (g_pb[p] == 7 || g_pb[p] == 15);

                if (grey && mx - mn > 8) {
                    te += 200000;
                }
                if (g_pa[p] >= 8 && g_pa[p] != 15) {
                    te += 200000;
                }
                if (g_pb[p] >= 8 && g_pb[p] != 15) {
                    te += 200000;
                }
            }
            if (be < 0 || te < be) {
                be = te;
                best = p;
                bt = tt;
            }
            if (g_pcloud[p] && (ce < 0 || te < ce)) {
                ce = te;
                cb = p;
                ct = tt;
            }
        }
        g_lut_p[k] = (uint8_t)best;
        g_lut_t[k] = (uint8_t)bt;
        g_lut_cp[k] = (uint8_t)cb;
        g_lut_ct[k] = (uint8_t)ct;
    }
    g_lut_ok = 1;
}

/* Every scenery cell, on every stage. Sprites are left alone. A dark
 * green field (a city) stays dark green; a light blue field (clouds)
 * falls out as white mixed with blue or cyan. */
static void zx_dither_scenery(void)
{
    static uint8_t votes[ZW_CW * ZW_CH][ZX_NP];
    static uint8_t win[ZW_CW * ZW_CH];
    static int green_cell[ZW_CW * ZW_CH];
    int i, c;

    if (!g_lut_ok) {
        zx_lut_build();
    }
    memset(votes, 0, sizeof votes);
    memset(green_cell, 0, sizeof green_cell);
    {
        static int rs[ZW_CW * ZW_CH], gs[ZW_CW * ZW_CH], bs[ZW_CW * ZW_CH], ns[ZW_CW * ZW_CH];

        memset(rs, 0, sizeof rs);
        memset(gs, 0, sizeof gs);
        memset(bs, 0, sizeof bs);
        memset(ns, 0, sizeof ns);
        for (i = 0; i < ZW_W * ZW_H; i++) {
            int mx = i % ZW_W, my = i / ZW_W, c0 = (my >> 3) * ZW_CW + (mx >> 3);
            int k12, p;

            if (g_zw_spr[i] || !g_zw_n[i]) {
                continue;
            }
            k12 = K24_12(g_zw_col[i]);
            /* every scenery cell, every stage: the pair that best matches
             * the original colours. Sprites are skipped above. */
            p = g_lut_p[k12];
            if (p < g_pn) {
                votes[c0][p]++;
            }
            rs[c0] += (int)(g_zw_col[i] >> 16);
            gs[c0] += (int)((g_zw_col[i] >> 8) & 255);
            bs[c0] += (int)(g_zw_col[i] & 255);
            ns[c0]++;
        }
        for (c = 0; c < ZW_CW * ZW_CH; c++) {
            int p, bp = 0, bv = -1;
            int n = ns[c] ? ns[c] : 1;
            int ar = rs[c] / n, ag = gs[c] / n, ab = bs[c] / n;

            for (p = 0; p < g_pn; p++) {
                if (votes[c][p] > bv) {
                    bv = votes[c][p];
                    bp = p;
                }
            }
            win[c] = (uint8_t)bp;
            /* city field: dark and green, not the clouds stage */
            green_cell[c] = g_cf_stage != 3 && ns[c] >= 8 && ag > ar + 18 && ag > ab + 18 &&
                            (ar + ag + ab) / 3 < 100;
        }
    }
    for (i = 0; i < ZW_W * ZW_H; i++) {
        int mx = i % ZW_W, my = i / ZW_W, c0 = (my >> 3) * ZW_CW + (mx >> 3);
        int k12, p, t, a, b, sel;
        int r, g, bv, best, bt;

        if (!g_zw_n[i]) {
            continue;
        }
        if (g_zw_spr[i]) {
            g_zw_scn[i] = k_zx16[g_zw_bgc[i] & 15];
            continue;
        }
        if (green_cell[c0]) {
            /* a dark green field is closer to black than to #00D700, so a
             * free mix would drop it. Keep the city ground dark green. */
            sel = 4;
            g_zw_out[i] = g_zw_scn[i] = k_zx16[sel];
            g_zw_bgc[i] = (uint8_t)sel;
            continue;
        } else {
            k12 = K24_12(g_zw_col[i]);
            p = win[c0];
            a = g_pa[p];
            b = g_pb[p];
            /* Clouds: light blue and white, not a grey field. */
            if (g_cf_stage == 3) {
                a = 5;
                b = 7;
            }
            r = (int)(g_zw_col[i] >> 16);
            g = (int)((g_zw_col[i] >> 8) & 255);
            bv = (int)(g_zw_col[i] & 255);
            best = -1;
            bt = g_cf_stage == 3 ? g_lut_ct[k12] : g_lut_t[k12];
            for (t = 0; t <= 8; t++) {
                int e = zx_mix_err(r, g, bv, a, b, t);

                if (best < 0 || e < best) {
                    best = e;
                    bt = t;
                }
            }
            t = bt;
        }
        sel = (t * 8 > k_bayer[(my & 7) * 8 + (mx & 7)]) ? b : a;
        g_zw_out[i] = g_zw_scn[i] = k_zx16[sel];
        g_zw_bgc[i] = (uint8_t)sel;
    }
}

/* Stage 2 cruisers drawn as background tiles: a large connected mass
 * that is not a live sprite. Flat colour, no dither. First mass from
 * the top is dark yellow, the next is cyan. */
static void zx_stage2_hull(void)
{
    static uint16_t lab[ZW_W * ZW_H];
    static int stk[ZW_W * ZW_H];
    int i, nlab = 1;

    if (g_cf_stage != 2) {
        return;
    }
    memset(lab, 0, sizeof lab);
    for (i = 0; i < ZW_W * ZW_H; i++) {
        int sp, cnt, x0, y0, x1, y1, mx, my, k;
        int slot = 0;

        /* Dark lines inside the hull used to split it, so the pieces
         * stayed dithered. Anything above pure space-black can connect. */
        if (lab[i] || g_zw_spr[i] || !g_zw_n[i] || LUM24(g_zw_col[i]) <= 8) {
            continue;
        }
        sp = 0;
        cnt = 0;
        x0 = y0 = 9999;
        x1 = y1 = -1;
        stk[sp++] = i;
        lab[i] = (uint16_t)nlab;
        while (sp) {
            int q = stk[--sp], qx = q % ZW_W, qy = q / ZW_W, d;
            cnt++;
            if (qx < x0) x0 = qx;
            if (qy < y0) y0 = qy;
            if (qx > x1) x1 = qx;
            if (qy > y1) y1 = qy;
            for (d = 0; d < 8; d++) {
                static const int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
                static const int dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
                int nx = qx + dx[d], ny = qy + dy[d], n;

                if (nx < 0 || nx >= ZW_W || ny < 0 || ny >= ZW_H) {
                    continue;
                }
                n = ny * ZW_W + nx;
                if (lab[n] || g_zw_spr[n] || !g_zw_n[n] || LUM24(g_zw_col[n]) <= 8) {
                    continue;
                }
                lab[n] = (uint16_t)nlab;
                stk[sp++] = n;
            }
        }
        /* A one-pixel gap with the hull on both sides is detail, not
         * space. Do not grow outward: that would swallow the starfield. */
        {
            int yy, xx;

            for (yy = y0; yy <= y1; yy++) {
                for (xx = x0; xx <= x1; xx++) {
                    int q = yy * ZW_W + xx;
                    int left = xx > 0 && lab[q - 1] == (uint16_t)nlab;
                    int right = xx + 1 < ZW_W && lab[q + 1] == (uint16_t)nlab;
                    int up = yy > 0 && lab[q - ZW_W] == (uint16_t)nlab;
                    int down = yy + 1 < ZW_H && lab[q + ZW_W] == (uint16_t)nlab;

                    if (lab[q] || g_zw_spr[q] || !g_zw_n[q]) {
                        continue;
                    }
                    if ((left && right) || (up && down)) {
                        lab[q] = (uint16_t)nlab;
                        cnt++;
                    }
                }
            }
        }
        if (cnt < 80 || ((x1 - x0 + 1) < 12 && (y1 - y0 + 1) < 12)) {
            nlab++;
            continue;
        }
        for (k = 0; k < g_cr_n && k < 2; k++) {
            if (g_hull_box[k][2] >= x0 && x1 >= g_hull_box[k][0] &&
                g_hull_box[k][3] >= y0 && y1 >= g_hull_box[k][1]) {
                slot = k + 1;
            }
        }
        if (!slot && g_cr_n < 2) {
            slot = ++g_cr_n;
        }
        if (slot) {
            g_hull_box[slot - 1][0] = x0;
            g_hull_box[slot - 1][1] = y0;
            g_hull_box[slot - 1][2] = x1;
            g_hull_box[slot - 1][3] = y1;
        }
        if (!slot) {
            nlab++;
            continue;
        }
        fprintf(stderr, "zx cruiser slot %d pixels %d box %d,%d %dx%d\n",
                slot, cnt, x0, y0, x1 - x0 + 1, y1 - y0 + 1);
        for (my = y0; my <= y1; my++) {
            for (mx = x0; mx <= x1; mx++) {
                int q = my * ZW_W + mx;
                int l, ink;

                if (lab[q] != (uint16_t)nlab) {
                    continue;
                }
                l = LUM24(g_zw_col[q]);
                ink = slot == 1 ? 6 : 13;
                g_zw_out[q] = k_zx16[l < 28 ? 0 : ink];
                g_zw_spr[q] = 1;
            }
        }
        nlab++;
        (void)k;
    }
}

static void zx_world(uint32_t *px, int w, int h)
{
    int x, y, i, sc = g_w_scroll;
    uint32_t palh = 2166136261u;
    uint32_t idxrgb[16] = {0};
    static uint16_t seen[ZW_W * ZW_H];
    int nseen = 0;
    static uint32_t fvote[65536][8];   /* this frame's votes per family */
    static uint8_t ftouch[65536];

    if (w < 560 || h < 400 || !g_w_bgi) {
        zx_clash(px, w, h);
        return;
    }
    if (!g_zc_ok) {
        zc_build();
    }
    memset(g_zw_sum, 0, sizeof g_zw_sum);
    memset(g_zw_n, 0, sizeof g_zw_n);
    memset(g_zw_sn, 0, sizeof g_zw_sn);
    memset(g_zw_bd, 0, sizeof g_zw_bd);
    memset(g_zw_id, 0, sizeof g_zw_id);
    memset(g_zw_tag, 0, sizeof g_zw_tag);
    memset(g_zw_bgc, 0, sizeof g_zw_bgc);
    memset(g_zw_scn, 0, sizeof g_zw_scn);
    if (g_big_stage != g_cf_stage) {
        g_big_stage = g_cf_stage;
        g_cr_n = 0;
        g_cr_id[0] = g_cr_id[1] = 0;
        memset(g_hull_box, 0, sizeof g_hull_box);
    }
    {
        static uint32_t hsh[ZW_CW * ZW_CH];

        for (i = 0; i < ZW_CW * ZW_CH; i++) {
            hsh[i] = 2166136261u;
        }
        for (y = 0; y < 400; y++) {
            int src = (y + sc) % 400, my = src >> 1, cy = src >> 4;
            const uint32_t *row = px + (size_t)y * w;

            for (x = 80; x < 560; x++) {
                int mx = (x - 80) >> 1, q = my * ZW_W + mx;
                size_t at = (size_t)y * w + x;
                uint32_t p = g_w_txt && g_w_txt[at] && g_w_pre ? g_w_pre[at] : row[x];
                uint8_t bi = g_w_bgi[at];
                uint16_t id = g_w_sid ? g_w_sid[at] : 0;
                uint32_t *hh = &hsh[cy * ZW_CW + ((x - 80) >> 4)];

                *hh = (*hh ^ (uint32_t)(bi + 1u + (uint32_t)((src & 15) << 4 | ((x - 80) & 15)) * 17u)) * 16777619u;
                g_zw_n[q]++;
                if (id) {
                    g_zw_sn[q]++;
                    g_zw_id[q] = id;
                } else {
                    if ((bi & 15) == 0) {
                        g_zw_bd[q]++;
                    }
                    g_zw_sum[q][0] += p >> 24;
                    g_zw_sum[q][1] += (p >> 16) & 0xFF;
                    g_zw_sum[q][2] += (p >> 8) & 0xFF;
                    idxrgb[bi & 15] = p;
                }
                if (id) {
                    /* sprite colour still needs the sprite's own pixel */
                    g_zw_sum[q][0] += (p >> 24) << 16;
                    g_zw_sum[q][1] += ((p >> 16) & 0xFF) << 16;
                    g_zw_sum[q][2] += ((p >> 8) & 0xFF) << 16;
                    if (g_kind && g_kind[at] > g_zw_tag[q]) {
                        g_zw_tag[q] = g_kind[at];
                    }
                }
            }
        }
        for (i = 0; i < 16; i++) {
            palh = (palh ^ idxrgb[i]) * 16777619u;
        }
        /* machine pixel colour: scenery part, else sprite part */
        for (i = 0; i < ZW_W * ZW_H; i++) {
            int n = g_zw_n[i], sn = g_zw_sn[i], bn = n - sn;
            uint32_t r, g, b;

            /* one sprite sample owns the machine pixel: a real sprite
             * was dropped when the 2x2 was mostly scenery, so cruisers
             * and small enemies were painted as the background */
            /* Majority: a single ship pixel must not paint the whole 2x2,
             * or the checker holes and the thin cannons disappear. */
            g_zw_spr[i] = n && sn * 2 >= n;
            if (g_zw_spr[i]) {
                r = (g_zw_sum[i][0] >> 16) / (sn ? sn : 1);
                g = (g_zw_sum[i][1] >> 16) / (sn ? sn : 1);
                b = (g_zw_sum[i][2] >> 16) / (sn ? sn : 1);
            } else {
                r = (g_zw_sum[i][0] & 0xFFFFu) / (bn ? bn : 1);
                g = (g_zw_sum[i][1] & 0xFFFFu) / (bn ? bn : 1);
                b = (g_zw_sum[i][2] & 0xFFFFu) / (bn ? bn : 1);
            }
            g_zw_col[i] = r << 16 | g << 8 | b;
        }
        /* scenery cells: recompute only when the art or palette changed */
        for (i = 0; i < ZW_CW * ZW_CH; i++) {
            int cx = i % ZW_CW, cy = i / ZW_CW, k, br, pp, qq, bA = 0, bB = 0, n = 0;
            long best = -1;
            int dd[64][16];

            if (g_zw_ok[i] && g_zw_hash[i] == hsh[i] && palh == g_zw_palh) {
                continue;
            }
            for (k = 0; k < 64; k++) {
                int q = (cy * 8 + k / 8) * ZW_W + cx * 8 + k % 8, j;
                const uint16_t *d;

                if (g_zw_spr[q] || !g_zw_n[q]) {
                    continue;
                }
                d = g_zc_d[K24_12(g_zw_col[q])];
                for (j = 0; j < 16; j++) {
                    dd[n][j] = d[j];
                }
                n++;
            }
            if (!n) {
                if (!g_zw_ok[i]) {
                    g_zw_ink[i] = 7;
                    g_zw_pap[i] = 0;
                }
                continue;   /* all sprite: keep, look again later */
            }
            /* scenery: the 8 dark colours only; sprites take the bright set */
            for (br = 0; br < 1; br++) {
                int base = br * 8;

                for (pp = 0; pp < 8; pp++) {
                    for (qq = pp; qq < 8; qq++) {
                        long e = 0;

                        for (k = 0; k < n; k++) {
                            int a = dd[k][base + pp], b = dd[k][base + qq];

                            e += a < b ? a : b;
                        }
                        if (pp != 0) {
                            e = e * 6 / 5 + 8 * n;
                        }
                        if (best < 0 || e < best) {
                            best = e;
                            bA = base + pp;
                            bB = base + qq;
                        }
                    }
                }
            }
            g_zw_pap[i] = (uint8_t)bA;   /* pp <= qq: black (0) when it is in */
            g_zw_ink[i] = (uint8_t)bB;
            g_zw_hash[i] = hsh[i];
            g_zw_ok[i] = 1;
        }
        g_zw_palh = palh;
        g_zx_space = zx_is_space(idxrgb[0]);
    }
    /* dim scenery (stage 1's asteroids and the like): its best ink and
     * paper are both near black, so it would vanish; such cells are
     * drawn as one object colour on black paper instead (dark magenta;
     * cyan on later stages), dark inner lines kept black. */
    {
        static int dsum[ZW_CW * ZW_CH], dn[ZW_CW * ZW_CH];

        memset(dsum, 0, sizeof dsum);
        memset(dn, 0, sizeof dn);
        for (i = 0; i < ZW_W * ZW_H; i++) {
            int mx = i % ZW_W, my = i / ZW_W, c = (my >> 3) * ZW_CW + (mx >> 3);

            if (!g_zw_spr[i] && g_zw_n[i] && g_zw_bd[i] * 2 < g_zw_n[i] - g_zw_sn[i]) {
                dsum[c] += LUM24(g_zw_col[i]);
                dn[c]++;
            }
        }
        for (i = 0; i < ZW_CW * ZW_CH; i++) {
            g_zw_dim[i] = dn[i] >= 4 && dsum[i] < dn[i] * 110;
        }
    }
    /* scenery pixels: ink or paper by distance, local contrast kept */
    for (i = 0; i < ZW_W * ZW_H; i++) {
        int mx = i % ZW_W, my = i / ZW_W, c = (my >> 3) * ZW_CW + (mx >> 3);
        int A = g_zw_pap[c], B = g_zw_ink[c];

        if (g_zw_spr[i]) {
            continue;
        }
        {
            int bn = g_zw_n[i] - g_zw_sn[i];
            int bd = bn > 0 && g_zw_bd[i] * 2 >= bn;
            int ci;

            if (g_zw_dim[c]) {
                int l = LUM24(g_zw_col[i]), m = 0, cnt = 0;

                if (mx > 0) { m += LUM24(g_zw_col[i - 1]); cnt++; }
                if (mx < ZW_W - 1) { m += LUM24(g_zw_col[i + 1]); cnt++; }
                if (my > 0) { m += LUM24(g_zw_col[i - ZW_W]); cnt++; }
                if (my < ZW_H - 1) { m += LUM24(g_zw_col[i + ZW_W]); cnt++; }
                m = cnt ? m / cnt : l;
                ci = (!bd && l > 16 && l >= m - 24) ? (g_cf_stage == 1 ? 3 : 5) : 0;
            } else {
                const uint16_t *d = g_zc_d[K24_12(g_zw_col[i])];
                int side = d[B] < d[A], l = LUM24(g_zw_col[i]), m = 0, cnt = 0;
                int lA = LUM24(k_zx16[A]), lB = LUM24(k_zx16[B]);

                if (A != B) {
                    if (mx > 0) { m += LUM24(g_zw_col[i - 1]); cnt++; }
                    if (mx < ZW_W - 1) { m += LUM24(g_zw_col[i + 1]); cnt++; }
                    if (my > 0) { m += LUM24(g_zw_col[i - ZW_W]); cnt++; }
                    if (my < ZW_H - 1) { m += LUM24(g_zw_col[i + ZW_W]); cnt++; }
                    m = cnt ? m / cnt : l;
                    if (l < m - 36) {
                        side = lA > lB;
                    } else if (l > m + 36) {
                        side = lB >= lA;
                    }
                }
                ci = side ? B : A;
            }
            g_zw_out[i] = k_zx16[ci];
            g_zw_bgc[i] = (uint8_t)ci;
        }
    }
    /* Halftone every background cell. Sprites stay flat (painted next). */
    zx_dither_scenery();
    /* background under sprites: the scenery that pixel covered, else the cell */
    {
        static uint8_t mode[ZW_CW * ZW_CH];
        static int hist[ZW_CW * ZW_CH][8];
        int c;

        memset(hist, 0, sizeof hist);
        for (i = 0; i < ZW_W * ZW_H; i++) {
            int mx = i % ZW_W, my = i / ZW_W, c0;

            if (g_zw_spr[i] || !g_zw_n[i]) {
                continue;
            }
            c0 = (my >> 3) * ZW_CW + (mx >> 3);
            hist[c0][g_zw_bgc[i] & 7]++;
        }
        for (c = 0; c < ZW_CW * ZW_CH; c++) {
            int j, b = g_zw_pap[c] & 7, best = -1;

            for (j = 0; j < 8; j++) {
                if (hist[c][j] > best) {
                    best = hist[c][j];
                    b = j;
                }
            }
            mode[c] = (uint8_t)b;
        }
        for (i = 0; i < ZW_W * ZW_H; i++) {
            int mx = i % ZW_W, my = i / ZW_W, c0 = (my >> 3) * ZW_CW + (mx >> 3);
            int bn = g_zw_n[i] - g_zw_sn[i];
            int bg;

            if (!g_zw_spr[i]) {
                continue;
            }
            if (bn > 0) {
                unsigned r = (g_zw_sum[i][0] & 0xFFFFu) / (unsigned)bn;
                unsigned g = (g_zw_sum[i][1] & 0xFFFFu) / (unsigned)bn;
                unsigned b = (g_zw_sum[i][2] & 0xFFFFu) / (unsigned)bn;

                bg = zx_dark_near((uint32_t)r << 16 | (uint32_t)g << 8 | (uint32_t)b);
            } else {
                bg = mode[c0];
            }
            if (g_cf_stage == 3 && bg == 0 && (g_zw_bd[i] > 0 || mode[c0] == 4)) {
                bg = 4;
            }
            g_zw_bgc[i] = (uint8_t)bg;
        }
    }
    /* sprites: one fixed ink per family, dark pixels black */
    for (i = 0; i < ZW_W * ZW_H; i++) {
        uint16_t id = g_zw_id[i];

        if (!g_zw_spr[i] || !id) {
            continue;
        }
        if (g_sf_state[id] < 13) {
            uint32_t p = g_zw_col[i];

            if (LUM24(p) > 48) {
                const uint16_t *d = g_zc_d[K24_12(p)];
                int j, bj = 1;

                for (j = 2; j < 8; j++) {
                    int dj = d[j] < d[8 + j] ? d[j] : d[8 + j], db = d[bj] < d[8 + bj] ? d[bj] : d[8 + bj];

                    if (dj < db) {
                        bj = j;
                    }
                }
                if (!ftouch[id]) {
                    ftouch[id] = 1;
                    seen[nseen++] = id;
                    memset(fvote[id], 0, sizeof fvote[id]);
                }
                fvote[id][bj]++;
            }
        }
    }
    for (i = 0; i < nseen; i++) {
        uint16_t id = seen[i];
        int j, bj = 1;

        ftouch[id] = 0;
        for (j = 1; j < 8; j++) {
            if (fvote[id][j] > fvote[id][bj]) {
                bj = j;
            }
        }
        g_sf_vote[id][bj]++;
        g_sf_state[id] = 13;    /* fixed from the first frame it is seen */
        if (g_sf_state[id] >= 13 || !g_sf_ink[id]) {
            int b2 = 1;

            for (j = 1; j < 8; j++) {
                if (g_sf_vote[id][j] > g_sf_vote[id][b2]) {
                    b2 = j;
                }
            }
            if (g_sf_state[id] >= 13) {
                g_sf_state[id] = 13;
                g_sf_ink[id] = (uint8_t)(8 + b2);   /* bright */
            }
        }
    }
    /* Stage 2 cruisers: one large enemy family, then kept for the stage. */
    if (g_cf_stage == 2) {
        static uint16_t fid[512];
        static int fx0[512], fy0[512], fx1[512], fy1[512], fn[512];
        static uint8_t ftag[512];
        int nf = 0, k;

        for (i = 0; i < ZW_W * ZW_H; i++) {
            uint16_t id = g_zw_id[i];
            int mx, my, f;

            if (!g_zw_spr[i] || !id || id >= 0xFFF0u) {
                continue;
            }
            for (f = 0; f < nf; f++) {
                if (fid[f] == id) {
                    break;
                }
            }
            if (f == nf) {
                if (nf == 512) {
                    continue;
                }
                fid[nf] = id;
                fx0[nf] = fy0[nf] = 9999;
                fx1[nf] = fy1[nf] = -1;
                fn[nf] = 0;
                ftag[nf] = 0;
                nf++;
            }
            mx = i % ZW_W;
            my = i / ZW_W;
            if (mx < fx0[f]) {
                fx0[f] = mx;
            }
            if (my < fy0[f]) {
                fy0[f] = my;
            }
            if (mx > fx1[f]) {
                fx1[f] = mx;
            }
            if (my > fy1[f]) {
                fy1[f] = my;
            }
            fn[f]++;
            if (g_zw_tag[i] == 1) {
                ftag[f] = 1;
            }
        }
        for (k = 0; k < nf; k++) {
            int wdt = fx1[k] - fx0[k] + 1, hgt = fy1[k] - fy0[k] + 1;

            /* real sprite, large enough to be a cruiser: first one seen
             * (top of the screen first) is dark yellow, the next is cyan */
            if (ftag[k] == 1 && fn[k] >= 80 && (wdt >= 14 || hgt >= 14) && g_cr_n < 2) {
                int s, seen = 0;

                for (s = 0; s < g_cr_n; s++) {
                    if (g_cr_id[s] == fid[k]) {
                        seen = 1;
                    }
                }
                if (!seen) {
                    g_cr_id[g_cr_n++] = fid[k];
                }
            }
        }
    }
    for (i = 0; i < ZW_W * ZW_H; i++) {
        uint16_t id = g_zw_id[i];
        int ink, col, l, tag, bg, big;
        static const int alt[6] = {10, 14, 11, 12, 9, 13}; /* bright, not white */

        if (!g_zw_spr[i]) {
            continue;
        }
        l = LUM24(g_zw_col[i]);
        tag = g_zw_tag[i];
        bg = g_zw_bgc[i] & 7;
        big = (g_cr_n > 0 && id == g_cr_id[0]) ? 1 : (g_cr_n > 1 && id == g_cr_id[1]) ? 2 : 0;
        if (id >= 0xFFF1u && id <= 0xFFF5u) {
            ink = id == 0xFFF1u ? 10 : id == 0xFFF3u ? 14 : 9; /* V red, M yellow, L blue */
        } else if (big == 1) {
            ink = 6; /* dark yellow */
        } else if (big == 2) {
            ink = 13; /* cyan */
        } else if (id && g_sf_ink[id]) {
            ink = g_sf_ink[id];
        } else {
            ink = 15;
        }
        if (!big && g_zx_space && tag == 1 && id < 0xFFF0u && (ink & 7) == 7) {
            ink = (id % 3) == 0 ? 14 : (id % 3) == 1 ? 11 : 12; /* yellow, magenta, green */
        }
        if (big) {
            int m = l, d, mx = i % ZW_W, my = i / ZW_W;

            for (d = 0; d < 4; d++) {
                int nx = mx + (d == 0 ? -1 : d == 1 ? 1 : 0);
                int ny = my + (d == 2 ? -1 : d == 3 ? 1 : 0);
                int n, ln;

                if (nx < 0 || nx >= ZW_W || ny < 0 || ny >= ZW_H) {
                    continue;
                }
                n = ny * ZW_W + nx;
                if (g_zw_id[n] != id) {
                    continue;
                }
                ln = LUM24(g_zw_col[n]);
                if (ln > m) {
                    m = ln;
                }
            }
            /* flat cruiser colour; only the darker lines go black */
            col = (l * 3 < m * 2) ? 0 : ink;
        } else if (tag == 3) {
            col = l > 36 ? ink : 0;
            if ((col & 7) == bg) {
                int a, s;

                if (ink && (ink & 7) != bg) {
                    col = ink;
                } else {
                    col = 10;
                    s = (int)(id % 6);
                    for (a = 0; a < 6; a++) {
                        int c2 = alt[(s + a) % 6];

                        if ((c2 & 7) != bg) {
                            col = c2;
                            break;
                        }
                    }
                }
            }
        } else {
            col = l > 48 ? ink : 0;
        }
        /* A sprite must not wear the colour of the background under it
         * (white enemies on white clouds, same rule as enemy bullets). */
        if (!big && col && (col & 7) == (bg & 7)) {
            static const int alt[6] = {10, 14, 11, 12, 9, 13};
            int a, s = (int)(id % 6);

            col = 10;
            for (a = 0; a < 6; a++) {
                int c2 = alt[(s + a) % 6];

                if ((c2 & 7) != (bg & 7)) {
                    col = c2;
                    break;
                }
            }
        }
        g_zw_out[i] = k_zx16[col];
    }
    zx_stage2_hull();
    if (g_w_txt) {
        for (y = 0; y < 400; y += 2) {
            for (x = 80; x < 560; x += 2) {
                size_t q = (size_t)y * w + x;
                const size_t k4[4] = {q, q + 1, q + (size_t)w, q + (size_t)w + 1};
                int k, lit = 0, dark = 0;

                for (k = 0; k < 4; k++) {
                    if (g_w_txt[k4[k]]) {
                        if (LUM24(px[k4[k]] >> 8) > 60) {
                            lit++;
                        } else {
                            dark++;
                        }
                    }
                }
                g_tblk[(y >> 1) * ZW_W + ((x - 80) >> 1)] = (uint8_t)(lit ? 2 : dark ? 1 : 0);
            }
        }
    }
    /* back to the screen, through the same scroll */
    for (y = 0; y < 400; y++) {
        int src = (y + sc) % 400, my = src >> 1;
        uint32_t *row = px + (size_t)y * w;
        const uint32_t *o = g_zw_out + my * ZW_W;

        for (x = 80; x < 560; x++) {
            row[x] = o[(x - 80) >> 1] << 8 | 0xFFu;
        }
    }
    /* in-screen text (HUD, stage captions) is outside the clash rules:
     * white at the machine's 2x2 pixel size, over the scene; its dark
     * outline pixels black */
    if (g_w_txt) {
        for (y = 0; y < 400; y += 2) {
            for (x = 80; x < 560; x += 2) {
                size_t q = (size_t)y * w + x;
                uint8_t t = g_tblk[(y >> 1) * ZW_W + ((x - 80) >> 1)];

                if (t) {
                    uint32_t c = t == 2 ? 0xFFFFFFFFu : 0x000000FFu;

                    px[q] = px[q + 1] = px[q + (size_t)w] = px[q + (size_t)w + 1] = c;
                }
            }
        }
    }
}

/* ---- MSX2 SCREEN 5 -------------------------------------------------
 * On the game's own pixels (no resampling: a 212-line grid made
 * everything shimmer as it moved); 16 colours out of the V9938's 512 (3 bits per channel),
 * chosen for the scene: warm-started from the last palette and refined
 * on a 512-bin histogram, entries snapped to the 3-bit grid, and only
 * swapped in when the new set is clearly better (no flicker). Then
 * every pixel takes the nearest of the 16. No clash. */
static int g_m2_pal[16];       /* 9-bit colours */
static int g_m2_ok;
static uint8_t g_m2_map[512];

static uint32_t m2_rgb(int c9)
{
    int r = (c9 >> 6) & 7, g = (c9 >> 3) & 7, b = c9 & 7;

    return (uint32_t)(r * 255 / 7) << 16 | (uint32_t)(g * 255 / 7) << 8 | (uint32_t)(b * 255 / 7);
}

static long m2_dist(int a, int b)
{
    uint32_t cb = m2_rgb(b), ca = m2_rgb(a);

    return redmean((int)(ca >> 16), (int)((ca >> 8) & 0xFF), (int)(ca & 0xFF), cb);
}

static long m2_err(const long *hist, const int *pal)
{
    long e = 0;
    int k, j;

    for (k = 0; k < 512; k++) {
        long bd = -1;

        if (!hist[k]) {
            continue;
        }
        for (j = 0; j < 16; j++) {
            long d = m2_dist(k, pal[j]);

            if (bd < 0 || d < bd) {
                bd = d;
            }
        }
        e += bd * hist[k];
    }
    return e;
}

static void msx2_screen(uint32_t *px, int w, int h)
{
    static long hist[512];
    static uint16_t key[MG_MAX];
    int gw, n, i, k, j, it, pal[16];

    g_mg_gwf = 240;                        /* the game's own pixels: no resampling */
    if (!mg_read(px, w, h, 400, &gw)) {
        g_mg_gwf = 0;
        return;
    }
    g_mg_gwf = 0;
    n = gw * 400;
    memset(hist, 0, sizeof hist);
    for (i = 0; i < n; i++) {
        uint32_t p = g_mg[i];
        int r = (int)((p >> 16) * 7 + 127) / 255, g = (int)(((p >> 8) & 0xFF) * 7 + 127) / 255,
            b = (int)((p & 0xFF) * 7 + 127) / 255;

        key[i] = (uint16_t)(r << 6 | g << 3 | b);
        hist[key[i]]++;
    }
    if (!g_m2_ok) {
        for (j = 0; j < 16; j++) {
            g_m2_pal[j] = j * 511 / 15;
        }
        g_m2_ok = 1;
    }
    memcpy(pal, g_m2_pal, sizeof pal);
    pal[0] = 0;                            /* colour 0: black */
    for (it = 0; it < 4; it++) {
        long sr[16] = {0}, sg[16] = {0}, sb[16] = {0}, cnt[16] = {0};

        for (k = 0; k < 512; k++) {
            long bd = -1;
            int bj = 0;

            if (!hist[k]) {
                continue;
            }
            for (j = 0; j < 16; j++) {
                long d = m2_dist(k, pal[j]);

                if (bd < 0 || d < bd) {
                    bd = d;
                    bj = j;
                }
            }
            sr[bj] += ((k >> 6) & 7) * hist[k];
            sg[bj] += ((k >> 3) & 7) * hist[k];
            sb[bj] += (k & 7) * hist[k];
            cnt[bj] += hist[k];
        }
        for (j = 1; j < 16; j++) {
            if (cnt[j]) {
                int r = (int)((sr[j] * 2 + cnt[j]) / (2 * cnt[j])), g = (int)((sg[j] * 2 + cnt[j]) / (2 * cnt[j])),
                    b = (int)((sb[j] * 2 + cnt[j]) / (2 * cnt[j]));

                pal[j] = r << 6 | g << 3 | b;
            } else {
                /* dead entry: the colour served worst so far */
                long worst = -1;
                int wk = pal[j];

                for (k = 0; k < 512; k++) {
                    long bd = -1;
                    int q;

                    if (!hist[k]) {
                        continue;
                    }
                    for (q = 0; q < 16; q++) {
                        long d = m2_dist(k, pal[q]);

                        if (bd < 0 || d < bd) {
                            bd = d;
                        }
                    }
                    bd *= hist[k];
                    if (bd > worst) {
                        worst = bd;
                        wk = k;
                    }
                }
                pal[j] = wk;
            }
        }
    }
    /* hysteresis: keep the old palette unless the new one is 12 % better */
    if (m2_err(hist, pal) * 112 < m2_err(hist, g_m2_pal) * 100) {
        memcpy(g_m2_pal, pal, sizeof pal);
    }
    for (k = 0; k < 512; k++) {
        long bd = -1;

        for (j = 0; j < 16; j++) {
            long d = m2_dist(k, g_m2_pal[j]);

            if (bd < 0 || d < bd) {
                bd = d;
                g_m2_map[k] = (uint8_t)j;
            }
        }
    }
    for (i = 0; i < n; i++) {
        g_mg[i] = m2_rgb(g_m2_pal[g_m2_map[key[i]]]);
    }
    mg_write(px, w, h, 400, gw);
}


/* ---- MEGA DRIVE (interlace, 8x16 tiles) ----------------------------
 * 9-bit colour (3 bits per channel) on the VDP's
 * non-linear DAC levels; four palette lines of 15 colours plus the
 * shared transparent/backdrop (black): 61 colours on screen. Lines 0-1
 * serve the scenery, 2-3 the sprites; every 8x16 tile picks one line
 * for its scenery pixels and one for its sprite pixels. The lines are
 * refitted to the scene each frame from the last ones and only
 * swapped in when clearly better (no flicker). */
static const uint8_t k_md_lv[8] = {0, 52, 87, 116, 144, 172, 206, 255};
#define MDL 15
#define MDT_W 40
#define MDT_H 28
/* The grid is the game's own pixels: 240 columns of two host pixels
 * and all 400 lines (the VDP's interlace mode, 8x16 tiles), so nothing
 * is resampled. A 320x224 grid over a 480x400 playfield cut pixels to
 * uneven widths and heights, and everything that moves against that
 * grid (sprites, the HUD against the anchored scenery) shimmered. */
#define MD_COLS 240
#define MD_ROWS 400
#define MDT_PH 16
static int g_md_pal[4][MDL];
static int g_md_ok, g_md_seed;
static uint8_t g_md_q[256];
static uint16_t g_md_d[512][512];       /* redmean distance / 16 */
static uint8_t g_md_tl[2][MDT_W * MDT_H];

static uint32_t md_rgb(int c9)
{
    return (uint32_t)k_md_lv[(c9 >> 6) & 7] << 16 | (uint32_t)k_md_lv[(c9 >> 3) & 7] << 8 | k_md_lv[c9 & 7];
}

static void md_build(void)
{
    int a, b, v;

    for (v = 0; v < 256; v++) {
        int bj = 0, bd = 999;

        for (a = 0; a < 8; a++) {
            int d = abs(v - (int)k_md_lv[a]);

            if (d < bd) {
                bd = d;
                bj = a;
            }
        }
        g_md_q[v] = (uint8_t)bj;
    }
    for (a = 0; a < 512; a++) {
        uint32_t ca = md_rgb(a);

        for (b = 0; b < 512; b++) {
            long d = redmean((int)(ca >> 16), (int)((ca >> 8) & 0xFF), (int)(ca & 0xFF), md_rgb(b)) / 16;

            g_md_d[a][b] = (uint16_t)(d > 65535 ? 65535 : d);
        }
    }
    for (a = 0; a < 4; a++) {
        for (b = 0; b < MDL; b++) {
            g_md_pal[a][b] = (b + 1) * 511 / MDL;
        }
    }
    g_md_ok = 1;
}

/* best of the line's colours or the black backdrop */
static int md_near(const int *pal, int k, long *dout)
{
    int j, bc = 0;
    long bd = g_md_d[k][0];

    for (j = 0; j < MDL; j++) {
        long d = g_md_d[k][pal[j]];

        if (d < bd) {
            bd = d;
            bc = pal[j];
        }
    }
    if (dout) {
        *dout = bd;
    }
    return bc;
}

static long md_err(const long *hist, const int *pal)
{
    long e = 0, d;
    int k;

    for (k = 0; k < 512; k++) {
        if (hist[k]) {
            md_near(pal, k, &d);
            e += d * hist[k];
        }
    }
    return e;
}

static void md_fit(const long *hist, int *pal)
{
    int it, k, j;

    for (it = 0; it < 3; it++) {
        long sr[MDL] = {0}, sg[MDL] = {0}, sb[MDL] = {0}, cnt[MDL] = {0};

        for (k = 0; k < 512; k++) {
            long bd = -1;
            int bj = 0;

            if (!hist[k]) {
                continue;
            }
            for (j = 0; j < MDL; j++) {
                long d = g_md_d[k][pal[j]];

                if (bd < 0 || d < bd) {
                    bd = d;
                    bj = j;
                }
            }
            sr[bj] += ((k >> 6) & 7) * hist[k];
            sg[bj] += ((k >> 3) & 7) * hist[k];
            sb[bj] += (k & 7) * hist[k];
            cnt[bj] += hist[k];
        }
        for (j = 0; j < MDL; j++) {
            if (cnt[j]) {
                pal[j] = (int)((sr[j] * 2 + cnt[j]) / (2 * cnt[j])) << 6 | (int)((sg[j] * 2 + cnt[j]) / (2 * cnt[j])) << 3 |
                         (int)((sb[j] * 2 + cnt[j]) / (2 * cnt[j]));
            } else {
                long worst = -1, d;
                int wk = pal[j];

                for (k = 0; k < 512; k++) {
                    if (!hist[k]) {
                        continue;
                    }
                    md_near(pal, k, &d);
                    if (d * hist[k] > worst) {
                        worst = d * hist[k];
                        wk = k;
                    }
                }
                pal[j] = wk;
            }
        }
    }
}

/* tile of grid pixel i: scenery tiles stay in page space (glued to
 * the scrolling scenery), sprite tiles in screen space (glued to the
 * sprites and the HUD, which do not scroll) */
static int g_rot_sc;
static int md_tile(int i, int gw, int spr)
{
    int y = i / gw;

    if (spr && g_rot_sc) {
        y = (y - g_rot_sc + MD_ROWS) % MD_ROWS;
    }
    return (y / MDT_PH) * MDT_W + (i % gw) / 8;
}

static void md_screen(uint32_t *px, int w, int h)
{
    static uint16_t key[MG_MAX];
    static long hist[4][512];
    static uint8_t tsc[MDT_W * MDT_H], tsp[MDT_W * MDT_H];
    static uint16_t map[4][512];
    int gw, n, i, l, t, pass;

    g_mg_gwf = MD_COLS;
    if (!mg_read(px, w, h, MD_ROWS, &gw)) {
        g_mg_gwf = 0;
        return;
    }
    g_mg_gwf = 0;
    if (!g_md_ok) {
        md_build();
    }
    n = gw * MD_ROWS;
    if (!g_md_seed) {
        /* first frame: dark tiles to line 0/2, light ones to 1/3 */
        static long ts[MDT_W * MDT_H], tn[MDT_W * MDT_H];

        for (i = 0; i < n; i++) {
            int tt = md_tile(i, gw, g_mg_spr[i]);

            ts[tt] += LUM24(g_mg[i]);
            tn[tt]++;
        }
        for (t = 0; t < MDT_W * MDT_H; t++) {
            g_md_tl[0][t] = g_md_tl[1][t] = (uint8_t)(tn[t] && ts[t] / tn[t] > 60);
        }
        g_md_seed = 1;
    }
    for (i = 0; i < n; i++) {
        uint32_t p = g_mg[i];

        key[i] = (uint16_t)(g_md_q[p >> 16] << 6 | g_md_q[(p >> 8) & 0xFF] << 3 | g_md_q[p & 0xFF]);
    }
    for (pass = 0; pass < 2; pass++) {
        int pal[4][MDL];

        memset(hist, 0, sizeof hist);
        for (i = 0; i < n; i++) {
            int tt = md_tile(i, gw, g_mg_spr[i]);

            hist[g_mg_spr[i] ? 2 + g_md_tl[1][tt] : g_md_tl[0][tt]][key[i]]++;
        }
        for (l = 0; l < 4; l++) {
            memcpy(pal[l], g_md_pal[l], sizeof pal[l]);
            md_fit(hist[l], pal[l]);
            /* hysteresis: keep the old line unless the new one is 8 % better */
            if (md_err(hist[l], pal[l]) * 108 < md_err(hist[l], g_md_pal[l]) * 100) {
                memcpy(g_md_pal[l], pal[l], sizeof pal[l]);
            }
            for (i = 0; i < 512; i++) {
                long d;

                map[l][i] = (uint16_t)md_near(g_md_pal[l], i, &d);
                (void)d;
            }
        }
        /* each tile: the line with the least error, a small bias to its last one */
        {
            static long te[MDT_W * MDT_H][4];

            memset(te, 0, sizeof te);
            for (i = 0; i < n; i++) {
                int tt = md_tile(i, gw, g_mg_spr[i]), k = key[i], b = g_mg_spr[i] ? 2 : 0;

                te[tt][b] += g_md_d[k][map[b][k]];
                te[tt][b + 1] += g_md_d[k][map[b + 1][k]];
            }
            for (t = 0; t < MDT_W * MDT_H; t++) {
                int s0 = g_md_tl[0][t], s1 = g_md_tl[1][t];

                tsc[t] = (uint8_t)(te[t][1 - s0] * 10 < te[t][s0] * 9 ? 1 - s0 : s0);
                tsp[t] = (uint8_t)(te[t][2 + 1 - s1] * 10 < te[t][2 + s1] * 9 ? 1 - s1 : s1);
            }
            memcpy(g_md_tl[0], tsc, sizeof tsc);
            memcpy(g_md_tl[1], tsp, sizeof tsp);
        }
    }
    for (i = 0; i < n; i++) {
        int tt = md_tile(i, gw, g_mg_spr[i]);
        int l2 = g_mg_spr[i] ? 2 + g_md_tl[1][tt] : g_md_tl[0][tt];

        g_mg[i] = md_rgb(map[l2][key[i]]);
    }
    mg_write(px, w, h, MD_ROWS, gw);
}



static void cf_apply_screen(uint32_t *px, int w, int h, int mode)
{
    if (!px || w <= 0 || h <= 0) {
        return;
    }
    if (mode == CF_ORIGINAL) {
        g_prev_mode = -1;
        return;
    }
    /* machine line counts: PCE 224, ZX 192 (C64 is its 2x2 cell grid,
     * 200 lines; GAME BOY, VIRTUAL BOY and GAME GEAR keep their dots) */
    /* PC ENGINE keeps the game's own pixel grid: resampling to 224
     * lines made sprites and the HUD shimmer against the scenery */
    if (mode == CF_ZX) {
        zx_world(px, w, h);     /* zx_clash (screen cells) and the Syder-style zx_screen stay as fallback / unused */
        return;
    }
    if (mode == CF_MSX2) {
        msx2_screen(px, w, h);
        return;
    }
    if (mode == CF_MD) {
        md_screen(px, w, h);
        return;
    }
    if (mode == CF_C64) {
        clash(px, w, h, mode);
    } else {
        cfilter_apply_flat(px, (size_t)w * h, mode);
    }
    flaws(px, w, h, mode);
}

/* World anchoring: the stage background scrolls through a 400-line page
 * (screen row y shows page row (y + scroll) % 400). Every palette-limited
 * filter except ZX (which is world-locked on its own) runs on the frame
 * rotated into page space, so its tiles, cells and per-tile palette
 * choices stay glued to the scenery instead of sliding through it. */
static uint32_t g_rot_px[640 * 400];
static uint8_t g_rot_l[640 * 400], g_rot_k[640 * 400];

static void rot_rows(void *dst, const void *src, int w, int h, int sc, size_t es)
{
    size_t row = (size_t)w * es, top = (size_t)(h - sc);
    /* source row y lands on row (y + sc) % h */
    memcpy((char *)dst + (size_t)sc * row, src, top * row);
    memcpy(dst, (const char *)src + top * row, (size_t)sc * row);
}

/* ---- Hardware pixel grid (PC ENGINE 256x224, MSX2 256x212) ----------
 * Point-samples the finished playfield onto the console's grid, the way
 * the hardware composes it: background cells are fixed to the scrolling
 * scenery (page rows, so the scenery moves rigidly, a whole cell pattern
 * at a time), sprite and HUD cells are fixed to the screen. Nothing
 * resamples a sprite at a different phase from one frame to the next,
 * so it never changes shape (the old box resample in page space made
 * sprites and the score line shimmer). Fixed nearest mapping, no new
 * colours. sc: page scroll (screen row y shows page row (y + sc) % h). */
static uint32_t g_hg_src[640 * 400];

static void hw_grid(uint32_t *px, int w, int h, int cols, int rows, int sc)
{
    int x, y, c;
    static int cx[480], cc[480];

    if (w != 640 || h != 400) {
        return;
    }
    memcpy(g_hg_src, px, (size_t)w * h * 4u);
    for (c = 0; c < 480; c++) {
        cc[c] = c * cols / 480;
        cx[c] = 80 + (int)(((float)cc[c] + 0.5f) * 480.0f / (float)cols);
    }
    for (y = 0; y < h; y++) {
        int rs = y * rows / h, ys = (int)(((float)rs + 0.5f) * (float)h / (float)rows);
        int py = (y + sc) % h, rb = py * rows / h;
        int pb0 = (rb * h + rows - 1) / rows, pb1 = ((rb + 1) * h + rows - 1) / rows;
        int pc = (int)(((float)rb + 0.5f) * (float)h / (float)rows);
        int yb = ((pc - sc) % h + h) % h;

        for (x = 80; x < 560; x++) {
            int xs = cx[x - 80];
            size_t sa = (size_t)ys * w + xs, ba = (size_t)yb * w + xs;
            uint32_t o;

            if ((g_layer && g_layer[sa]) || (g_w_txt && g_w_txt[sa])) {
                o = g_hg_src[sa];
            } else {
                o = g_hg_src[ba];
                if ((g_layer && g_layer[ba]) || (g_w_txt && g_w_txt[ba])) {
                    int q;

                    for (q = pb0; q < pb1; q++) {   /* scenery under a sprite */
                        size_t a = (size_t)(((q - sc) % h + h) % h) * w + xs;

                        if (!(g_layer && g_layer[a]) && !(g_w_txt && g_w_txt[a])) {
                            o = g_hg_src[a];
                            break;
                        }
                    }
                }
            }
            px[(size_t)y * w + x] = o;
        }
    }
}

static void cf_apply_inner(uint32_t *px, int w, int h, int mode)
{
    int sc = g_w_scroll;

    if (!px || w <= 0 || h <= 0) {
        return;
    }
    if (mode != CF_MD && mode != CF_PCENGINE && mode != CF_GAMEGEAR && mode != CF_MSX2 && mode != CF_C64) {
        cf_apply_screen(px, w, h, mode);
        return;
    }
    if (!g_w_bgi || w != 640 || h != 400 || sc <= 0 || sc >= h) {
        cf_apply_screen(px, w, h, mode);
        return;
    }
    {
        const uint8_t *L = g_layer, *K = g_kind;

        rot_rows(g_rot_px, px, w, h, sc, 4);
        if (L) { rot_rows(g_rot_l, L, w, h, sc, 1); g_layer = g_rot_l; }
        if (K) { rot_rows(g_rot_k, K, w, h, sc, 1); g_kind = g_rot_k; }
        g_rot_sc = sc;
        cf_apply_screen(g_rot_px, w, h, mode);
        g_rot_sc = 0;
        g_layer = L;
        g_kind = K;
        rot_rows(px, g_rot_px, w, h, h - sc, 4);
    }
}

static void cfilter_apply2(uint32_t *px, int w, int h, int mode);
/* In-screen text (HUD line, stage captions) for every filter, the ZX
 * SPECTRUM rule: outside the machine's colour and resolution rules,
 * read from the frame before the filter in 2x2 blocks and drawn over
 * the scene at that size, lit pixels in the filter's brightest colour
 * (white; the lightest LCD green on GAME BOY, the brightest red on
 * VIRTUAL BOY) and the dark outline in its darkest, so it stays
 * readable. ZX SPECTRUM does this inside its own pass. */
static uint8_t g_hud_blk[240 * 200];

static int hud_read(const uint32_t *px, int w, int h)
{
    int x, y, any = 0;

    if (!g_w_txt || w != 640 || h != 400) {
        return 0;
    }
    for (y = 0; y < 400; y += 2) {
        for (x = 80; x < 560; x += 2) {
            size_t q = (size_t)y * w + x;
            const size_t k4[4] = {q, q + 1, q + (size_t)w, q + (size_t)w + 1};
            int k, lit = 0, dark = 0;

            for (k = 0; k < 4; k++) {
                if (g_w_txt[k4[k]]) {
                    if (LUM24(px[k4[k]] >> 8) > 60) {
                        lit++;
                    } else {
                        dark++;
                    }
                }
            }
            g_hud_blk[(y >> 1) * 240 + ((x - 80) >> 1)] = (uint8_t)(lit ? 2 : dark ? 1 : 0);
            any |= lit | dark;
        }
    }
    return any;
}

static void hud_write(uint32_t *px, int w, int mode)
{
    uint32_t lit = 0xFFFFFFFFu, dark = 0x000000FFu;
    int x, y;

    if (mode == CF_GAMEBOY) {
        lit = k_gb[5] << 8 | 0xFFu;
        dark = k_gb[0] << 8 | 0xFFu;
    } else if (mode == CF_VIRTUALBOY) {
        lit = k_vb[5] << 8 | 0xFFu;
        dark = k_vb[0] << 8 | 0xFFu;
    }
    for (y = 0; y < 400; y += 2) {
        for (x = 80; x < 560; x += 2) {
            uint8_t t = g_hud_blk[(y >> 1) * 240 + ((x - 80) >> 1)];

            if (t) {
                size_t q = (size_t)y * w + x;
                uint32_t c = t == 2 ? lit : dark;

                px[q] = px[q + 1] = px[q + (size_t)w] = px[q + (size_t)w + 1] = c;
            }
        }
    }
}

/* ZX SPECTRUM detail pass, on 2x2 machine pixels in screen space, read
 * from the frame before the filter: a block clearly darker than a
 * neighbour (an outline, an edge, an inner detail line) is drawn black
 * so shapes read through the attribute colours, and the background
 * stars (small bright scenery dots and streaks with dark on both
 * sides) are always white. The text keeps its own rule. */
static uint32_t g_zd_src[640 * 400];
static uint8_t g_zd_l[240 * 200], g_zd_m[240 * 200];

static void zx_detail(uint32_t *px, int w)
{
    int x, y;

    for (y = 0; y < 200; y++) {
        for (x = 0; x < 240; x++) {
            size_t q = (size_t)(y * 2) * w + 80 + x * 2;
            uint32_t a = g_zd_src[q] >> 8, b = g_zd_src[q + 1] >> 8, c = g_zd_src[q + w] >> 8, d = g_zd_src[q + w + 1] >> 8;

            g_zd_l[y * 240 + x] = (uint8_t)((LUM24(a) + LUM24(b) + LUM24(c) + LUM24(d)) >> 2);
            {
                uint32_t v[4] = {a, b, c, d};
                int k, m = 0, grey = 1;

                for (k = 0; k < 4; k++) {
                    int r = (int)(v[k] >> 16 & 255), g = (int)(v[k] >> 8 & 255), bb = (int)(v[k] & 255);
                    int hi = r > g ? (r > bb ? r : bb) : (g > bb ? g : bb), lo = r < g ? (r < bb ? r : bb) : (g < bb ? g : bb);
                    int l = LUM24(v[k]);

                    if (l > m) {
                        m = l;
                        grey = hi - lo < 64;
                    }
                }
                g_zd_m[y * 240 + x] = (uint8_t)(grey ? m : 0);
            }
        }
    }
    for (y = 1; y < 199; y++) {
        for (x = 1; x < 239; x++) {
            int l = g_zd_l[y * 240 + x], k, mx = 0;
            const int nb[4] = {-1, 1, -240, 240};
            size_t q = (size_t)(y * 2) * w + 80 + x * 2;
            uint32_t c;

            if (g_w_txt && (g_w_txt[q] || g_w_txt[q + 1] || g_w_txt[q + w] || g_w_txt[q + w + 1])) {
                continue;
            }
            /* enemy bullets keep the colour chosen against the background */
            if (g_kind && (g_kind[q] == 3 || g_kind[q + 1] == 3 ||
                           g_kind[q + w] == 3 || g_kind[q + w + 1] == 3)) {
                continue;
            }
            for (k = 0; k < 4; k++) {
                int v = g_zd_l[y * 240 + x + nb[k]];

                if (v > mx) {
                    mx = v;
                }
            }
            {
                int spr = g_kind && (g_kind[q] || g_kind[q + 1] || g_kind[q + w] || g_kind[q + w + 1]);

                /* stars are scenery: a bright sprite must not be turned white */
                if (!spr && g_zd_m[y * 240 + x] >= 90 && g_zd_l[y * 240 + x - 1] < 40 &&
                    g_zd_l[y * 240 + x + 1] < 40) {
                    c = 0xFFFFFFFFu;
                } else if (spr && mx - l > 18 && l * 5 < mx * 4) {
                    c = 0x000000FFu;             /* sprite inner detail */
                } else {
                    continue;
                }
            }
            px[q] = px[q + 1] = px[q + (size_t)w] = px[q + (size_t)w + 1] = c;
        }
    }
}

/* ZX: the player's own pixels after the clash pass. The ship is bright
 * white with its black detail lines kept; a wing tip (one source pixel
 * at the end of the silhouette) fills its whole machine pixel so it
 * stays legible in straight flight. Options and shots use the weapon
 * colour (red, blue or yellow). Where the I-FRAMES aura painted, that
 * mode colour replaces the ship, as before. */
static const uint8_t *g_pl_own, *g_pl_aura, *g_shade;
static int g_pl_aink = 10, g_pl_sink = 10;

void cfilter_set_shade(const uint8_t *shade)
{
    g_shade = shade;
}

void cfilter_set_player(const uint8_t *own, const uint8_t *aura, int aura_ink, int shot_ink)
{
    g_pl_own = own;
    g_pl_aura = aura;
    g_pl_aink = aura_ink & 15;
    g_pl_sink = shot_ink & 15;
}

/* Art grey, before the grade lifts it. Grey shading on the ship collapses
 * to white or black against the brightest pixel of that object. */
static int pix_shade(size_t at)
{
    if (g_shade) {
        return g_shade[at];
    }
    return LUM24(g_zd_src[at] >> 8);
}

static void zx_player(uint32_t *px, int w)
{
    static uint8_t role[200 * 240];
    static uint8_t litn[200 * 240];
    static uint8_t maxl[200 * 240];
    static uint8_t aura[200 * 240];
    int by, bx, smax[4] = {0, 0, 0, 0};

    if (!g_pl_own) {
        return;
    }
    memset(role, 0, sizeof role);
    for (by = 0; by < 200; by++) {
        for (bx = 0; bx < 240; bx++) {
            size_t q = (size_t)(by * 2) * w + 80 + (size_t)bx * 2;
            const size_t k4[4] = {q, q + 1, q + (size_t)w, q + (size_t)w + 1};
            int k, n1 = 0, n2 = 0, n3 = 0, a = 0, r = 0, mx = 0, lit = 0;

            if (g_w_txt && (g_w_txt[k4[0]] || g_w_txt[k4[1]] || g_w_txt[k4[2]] || g_w_txt[k4[3]])) {
                continue;
            }
            for (k = 0; k < 4; k++) {
                int v = g_pl_own[k4[k]];

                if (g_pl_aura && g_pl_aura[k4[k]]) {
                    a = 1;
                }
                if (v == 1) {
                    n1++;
                } else if (v == 2) {
                    n2++;
                } else if (v == 3) {
                    n3++;
                }
            }
            if (n1 >= n2 && n1 > 0) {
                r = 1;
            } else if (n2 > 0) {
                r = 2;
            } else if (n3 > 0) {
                r = 3;
            } else {
                continue;
            }
            for (k = 0; k < 4; k++) {
                int v = g_pl_own[k4[k]];
                int l;

                if ((r == 1 && v != 1) || (r == 2 && v != 2) || (r == 3 && v != 3)) {
                    continue;
                }
                l = pix_shade(k4[k]);
                if (l > mx) {
                    mx = l;
                }
                if (l > smax[r]) {
                    smax[r] = l;
                }
                if (l >= 40) {
                    lit++;
                }
            }
            role[by * 240 + bx] = (uint8_t)r;
            litn[by * 240 + bx] = (uint8_t)lit;
            maxl[by * 240 + bx] = (uint8_t)(mx > 255 ? 255 : mx);
            aura[by * 240 + bx] = (uint8_t)a;
        }
    }
    for (by = 0; by < 200; by++) {
        for (bx = 0; bx < 240; bx++) {
            int ix = by * 240 + bx, r = role[ix], nb = 0, ink, tip;
            size_t q = (size_t)(by * 2) * w + 80 + (size_t)bx * 2;
            uint32_t c;

            if (!r) {
                continue;
            }
            if (bx > 0 && role[ix - 1] == r) {
                nb++;
            }
            if (bx < 239 && role[ix + 1] == r) {
                nb++;
            }
            if (by > 0 && role[ix - 240] == r) {
                nb++;
            }
            if (by < 199 && role[ix + 240] == r) {
                nb++;
            }
            ink = r == 1 ? 15 : g_pl_sink;
            /* I-FRAMES: the whole machine pixel takes the mode colour. */
            if (aura[ix] && r != 3) {
                c = k_zx16[g_pl_aink] << 8 | 0xFFu;
                px[q] = px[q + 1] = px[q + (size_t)w] = px[q + (size_t)w + 1] = c;
                continue;
            }
            {
                /* Light and mid grey stay white (or the weapon colour).
                 * Only the darkest art pixels become black. Every own
                 * pixel is kept at 1 px; a hole in the 2x2 goes back to
                 * the scenery so the checker and the cannons survive. */
                const size_t k4[4] = {q, q + 1, q + (size_t)w, q + (size_t)w + 1};
                int k, sc = g_w_scroll < 0 ? 0 : g_w_scroll;

                (void)tip;
                (void)nb;
                (void)c;
                (void)ink;
                for (k = 0; k < 4; k++) {
                    int v = g_pl_own[k4[k]];
                    int hy = by * 2 + (k >= 2);
                    int src = (hy + sc) % 400;
                    int my = src >> 1;
                    uint32_t scn = g_zw_scn[my * ZW_W + bx] << 8 | 0xFFu;

                    if (v == 1 || v == 2 || v == 3) {
                        int l = pix_shade(k4[k]);
                        int use = v == 1 ? 15 : g_pl_sink;
                        int lim = v == 3 ? 16 : (smax[v] / 4 > 0 ? smax[v] / 4 : 1);

                        px[k4[k]] = (l >= lim ? k_zx16[use] : 0) << 8 | 0xFFu;
                    } else {
                        px[k4[k]] = scn;
                    }
                }
            }
        }
    }
}

void cfilter_apply(uint32_t *px, int w, int h, int mode)
{
    int zxd = px && mode == CF_ZX && w == 640 && h == 400;

    if (zxd) {
        memcpy(g_zd_src, px, sizeof g_zd_src);
    }
    cfilter_apply2(px, w, h, mode);
    if (zxd) {
        zx_detail(px, w);
        zx_player(px, w);
    }
}

static void cfilter_apply2(uint32_t *px, int w, int h, int mode)
{
    int sc = g_w_bgi && g_w_scroll > 0 && g_w_scroll < h ? g_w_scroll : 0;
    int hud = px && mode != CF_ORIGINAL && mode != CF_ZX && hud_read(px, w, h);

    cf_apply_inner(px, w, h, mode);
    if (!px) {
        return;
    }
    /* PC ENGINE: no 256x224 point grid since 0.10.1 (back to the look
     * before it: the game's own pixels, tiles of 8x8 of them), which
     * also keeps sprites and the HUD from shimmering. */
    if (mode == CF_MSX2) {
        hw_grid(px, w, h, 256, 212, sc);   /* SCREEN 5: 256x212 */
    }
    if (hud) {
        hud_write(px, w, mode);
    }
}
