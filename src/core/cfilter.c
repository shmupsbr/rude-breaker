#include "cfilter.h"

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

/* VIC-II multicolour sprites: 12x21 wide pixels (48x42 host), 3
 * colours each: two shared by every sprite ($D025/$D026) and one of
 * its own ($D027+), plus transparent. Sprite pixels arrive already on
 * the 16 colours; this restricts them to those rules. */
static void c64_sprites(uint32_t *px, int w, int h)
{
    long all[16] = {0};
    int s1 = -1, s2 = -1, x, y, i;

    if (!g_layer || !g_ix_ready) {
        return;
    }
    for (y = 0; y < h; y += 2) {
        for (x = 0; x < w; x += 4) {
            if (g_layer[(size_t)y * w + x]) {
                uint32_t p = px[(size_t)y * w + x];
                int k = g_ix_c64[(CR(p) >> 4) << 8 | (CG(p) >> 4) << 4 | (CB(p) >> 4)];

                if (k) all[k]++;
            }
        }
    }
    for (i = 1; i < 16; i++) {
        if (all[i] && (s1 < 0 || all[i] > all[s1])) s1 = i;
    }
    for (i = 1; i < 16; i++) {
        if (i != s1 && all[i] && (s2 < 0 || all[i] > all[s2])) s2 = i;
    }
    for (y = 0; y < h; y += 42) {
        for (x = 0; x < w; x += 48) {
            long c[16] = {0};
            int own = -1, xx, yy, any = 0;
            uint32_t pal[4];
            int np = 0;

            for (yy = y; yy < y + 42 && yy < h; yy += 2) {
                for (xx = x; xx < x + 48 && xx < w; xx += 4) {
                    if (g_layer[(size_t)yy * w + xx]) {
                        uint32_t p = px[(size_t)yy * w + xx];
                        int k = g_ix_c64[(CR(p) >> 4) << 8 | (CG(p) >> 4) << 4 | (CB(p) >> 4)];

                        c[k]++;
                        any = 1;
                    }
                }
            }
            if (!any) continue;
            for (i = 1; i < 16; i++) {
                if (i != s1 && i != s2 && c[i] && (own < 0 || c[i] > c[own])) own = i;
            }
            pal[np++] = k_c64[0];
            if (s1 > 0) pal[np++] = k_c64[s1];
            if (s2 > 0) pal[np++] = k_c64[s2];
            if (own > 0) pal[np++] = k_c64[own];
            for (yy = y; yy < y + 42 && yy < h; yy++) {
                for (xx = x; xx < x + 48 && xx < w; xx++) {
                    if (g_layer[(size_t)yy * w + xx]) {
                        uint32_t *p = &px[(size_t)yy * w + xx];
                        uint32_t q = nearest(CR(*p), CG(*p), CB(*p), pal, np);

                        *p = (q << 8) | (*p & 0xFFu);
                    }
                }
            }
        }
    }
}

static void clash(uint32_t *px, int w, int h, int mode)
{
    int bw, bh, cw, ch;
    int cx, cy;
    MPix mp[64];

    (void)mode;  /* COMMODORE 64 */
    /* Multicolour mode: 160x200 wide pixels (2:1, here 4x2 host
     * pixels), cells of 4x8 of them (the 8x8 hires character). */
    bw = 4; bh = 2; cw = 4; ch = 8;
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
        /* HuC6260 VCE: background tiles are 8x8 machine pixels with one
         * of 16 sub-palettes of 16 (entry 0 shared), sprites 16x16 with
         * one sub-palette of 15 + transparent. After lowres() a machine
         * pixel is 400/224 host pixels, so a tile is 14 and a sprite
         * cell 29 host pixels, from the playfield's left edge (x 80). */
        int tx, ty, t = 14, sc = 29, x0 = w >= 560 ? 80 : 0, x1 = w >= 560 ? 560 : w;
        size_t i, n = (size_t)w * h;

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
void cfilter_set_stage(int stage) { (void)stage; }

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
static void lowres(uint32_t *px, int w, int h, int lines)
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
#define MG_MAX (320 * 224)
static uint32_t g_mg[MG_MAX];
static uint8_t g_mg_spr[MG_MAX];
static int g_mg_x[321], g_mg_y[257];
static int g_mg_gwf;   /* columns over the playfield, 0 = square pixels */

static int mg_read(const uint32_t *px, int w, int h, int lines, int *gwp)
{
    float s = (float)h / (float)lines, sx;
    int gw = g_mg_gwf ? g_mg_gwf : (int)(480.0f / s + 0.5f), mx, my;

    if (w < 560 || s <= 1.0f || gw > 320) {
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
 * kept until that art (or the palette) changes. Every sprite family
 * gets one ink colour, voted over its first frames and then fixed for
 * good; its pixels are that ink or black, whatever it overlaps, so a
 * sprite never changes colour. */
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
static uint16_t g_zw_id[ZW_W * ZW_H];
static uint32_t g_zw_hash[ZW_CW * ZW_CH];
static uint8_t g_zw_ink[ZW_CW * ZW_CH], g_zw_pap[ZW_CW * ZW_CH];
static uint8_t g_zw_ok[ZW_CW * ZW_CH];
static uint32_t g_zw_col[ZW_W * ZW_H];
static uint32_t g_zw_out[ZW_W * ZW_H];
static uint8_t g_zw_spr[ZW_W * ZW_H];
static uint8_t g_tblk[ZW_W * ZW_H];
/* sprite family colour: votes over the first 12 frames, then locked */
static uint8_t g_sf_ink[65536];
static uint8_t g_sf_state[65536];     /* 0 new, 1..12 voting, 13 locked */
static uint16_t g_sf_vote[65536][8];
static uint32_t g_zw_palh;

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
    memset(g_zw_id, 0, sizeof g_zw_id);
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
            for (br = 0; br < 2; br++) {
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
            g_zw_pap[i] = (uint8_t)bA;   /* pp <= qq: black (0/8) when it is in */
            g_zw_ink[i] = (uint8_t)bB;
            g_zw_hash[i] = hsh[i];
            g_zw_ok[i] = 1;
        }
        g_zw_palh = palh;
    }
    /* scenery pixels: ink or paper by distance, local contrast kept */
    for (i = 0; i < ZW_W * ZW_H; i++) {
        int mx = i % ZW_W, my = i / ZW_W, c = (my >> 3) * ZW_CW + (mx >> 3);
        int A = g_zw_pap[c], B = g_zw_ink[c];

        if (g_zw_spr[i]) {
            continue;
        }
        {
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
            g_zw_out[i] = k_zx16[side ? B : A];
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
    for (i = 0; i < ZW_W * ZW_H; i++) {
        uint16_t id = g_zw_id[i];
        int ink;

        if (!g_zw_spr[i]) {
            continue;
        }
        ink = id >= 0xFFF1u && id <= 0xFFF5u ? (id == 0xFFF1u ? 10 : id == 0xFFF3u ? 7 : 9)
            : id && g_sf_ink[id] ? g_sf_ink[id] : 15;   /* capsules: weapon colour; else white until locked */
        g_zw_out[i] = LUM24(g_zw_col[i]) > 48 ? k_zx16[ink] : 0;
    }
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
 * 212 lines; 16 colours out of the V9938's 512 (3 bits per channel),
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

    if (!mg_read(px, w, h, 212, &gw)) {
        return;
    }
    n = gw * 212;
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
    mg_write(px, w, h, 212, gw);
}


/* ---- MEGA DRIVE (H40, 320x224) --------------------------------------
 * 224 lines, 320 columns over the playfield (real H40 width; the
 * side panels stand for the border). 9-bit colour (3 bits per channel) on the VDP's
 * non-linear DAC levels; four palette lines of 15 colours plus the
 * shared transparent/backdrop (black): 61 colours on screen. Lines 0-1
 * serve the scenery, 2-3 the sprites; every 8x8 tile picks one line
 * for its scenery pixels and one for its sprite pixels. The lines are
 * refitted to the scene each frame from the last ones and only
 * swapped in when clearly better (no flicker). */
static const uint8_t k_md_lv[8] = {0, 52, 87, 116, 144, 172, 206, 255};
#define MDL 15
#define MDT_W 40
#define MDT_H 28
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

static void md_screen(uint32_t *px, int w, int h)
{
    static uint16_t key[MG_MAX];
    static long hist[4][512];
    static uint8_t tsc[MDT_W * MDT_H], tsp[MDT_W * MDT_H];
    static uint16_t map[4][512];
    int gw, n, i, l, t, pass;

    g_mg_gwf = 320;
    if (!mg_read(px, w, h, 224, &gw)) {
        g_mg_gwf = 0;
        return;
    }
    g_mg_gwf = 0;
    if (!g_md_ok) {
        md_build();
    }
    n = gw * 224;
    if (!g_md_seed) {
        /* first frame: dark tiles to line 0/2, light ones to 1/3 */
        static long ts[MDT_W * MDT_H], tn[MDT_W * MDT_H];

        for (i = 0; i < n; i++) {
            int tt = (i / gw / 8) * MDT_W + (i % gw) / 8;

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
            int tt = (i / gw / 8) * MDT_W + (i % gw) / 8;

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
                int tt = (i / gw / 8) * MDT_W + (i % gw) / 8, k = key[i], b = g_mg_spr[i] ? 2 : 0;

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
        int tt = (i / gw / 8) * MDT_W + (i % gw) / 8;
        int l2 = g_mg_spr[i] ? 2 + g_md_tl[1][tt] : g_md_tl[0][tt];

        g_mg[i] = md_rgb(map[l2][key[i]]);
    }
    mg_write(px, w, h, 224, gw);
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
    if (mode == CF_PCENGINE) {
        lowres(px, w, h, 224);
    }
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
        c64_sprites(px, w, h);
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

void cfilter_apply(uint32_t *px, int w, int h, int mode)
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
        cf_apply_screen(g_rot_px, w, h, mode);
        g_layer = L;
        g_kind = K;
        rot_rows(px, g_rot_px, w, h, h - sc, 4);
    }
}
