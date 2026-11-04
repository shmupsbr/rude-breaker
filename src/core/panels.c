#include <stdlib.h>
/*
 * panels.c - side panels for wide windows.
 *
 * Layout: playfield (game x 80-559, the area inside the HUD frame)
 * scaled to 1080x1080 in the middle, two 388 px columns of boxes on the
 * sides. Text uses the Robotica font (Courtney Novits), baked into
 * panel_font.inc by tools/bake_font.py. The logo and the metal art come
 * from the title screen captured at run time; nothing from the game is
 * stored here.
 */
#include "panels.h"
#include "hal.h"
#include "gfx.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "panel_font.inc"

#define PW RB_PANEL_W
#define PH RB_PANEL_H
#define FIELD_X 420
#define FIELD_W 1080
#define GAME_X0 80
#define GAME_W 480

typedef struct Rgb {
    int r, g, b;
} Rgb;

static const Rgb C_EDGE = {120, 110, 130};
static const Rgb C_FILL = {6, 3, 12};
static const Rgb C_HEAD = {50, 34, 60};
static const Rgb C_DIM = {160, 160, 180};  /* >= 5:1 on the title bar and fills */
static const Rgb C_G = {80, 240, 80};
static const Rgb C_Y = {250, 230, 60};
static const Rgb C_R = {235, 60, 50};
static const Rgb C_W = {235, 235, 245};
static const Rgb C_BG = {10, 6, 16};
static const Rgb C_C = {150, 120, 255};

/* Font sizes, index into PF_FONT. */
enum { F16, F20, F26, F34, F48, F60 };

/* Box rules (all boxes): 30 px title bar, 16 px inner margin, content
 * starts below the bar and is centred in the space left. */
#define PAD 16
#define HEAD 30

static uint32_t g_title[640 * 400];
static int g_have_title;
static uint32_t g_back[PW * PH];
static uint32_t g_frame;
static int g_back_ok = -1; /* built for g_have_title state */

static uint32_t pack(Rgb c)
{
    return ((uint32_t)c.r << 24) | ((uint32_t)c.g << 16) | ((uint32_t)c.b << 8) | 0xFFu;
}

static Rgb unpack(uint32_t p)
{
    Rgb c;

    c.r = (int)(p >> 24) & 255;
    c.g = (int)(p >> 16) & 255;
    c.b = (int)(p >> 8) & 255;
    return c;
}

static Rgb scale(Rgb c, int num, int den)
{
    Rgb o;

    o.r = c.r * num / den;
    o.g = c.g * num / den;
    o.b = c.b * num / den;
    if (o.r > 255) o.r = 255;
    if (o.g > 255) o.g = 255;
    if (o.b > 255) o.b = 255;
    return o;
}

static void blend(uint32_t *o, int x, int y, Rgb c, int a)
{
    Rgb d;

    if (x < 0 || y < 0 || x >= PW || y >= PH || a <= 0) {
        return;
    }
    if (a >= 255) {
        o[y * PW + x] = pack(c);
        return;
    }
    d = unpack(o[y * PW + x]);
    d.r += (c.r - d.r) * a / 255;
    d.g += (c.g - d.g) * a / 255;
    d.b += (c.b - d.b) * a / 255;
    o[y * PW + x] = pack(d);
}

static void rect(uint32_t *o, int x, int y, int w, int h, Rgb c, int a)
{
    int i;
    int j;

    for (j = y; j < y + h; j++) {
        for (i = x; i < x + w; i++) {
            blend(o, i, j, c, a);
        }
    }
}

static void frame(uint32_t *o, int x, int y, int w, int h, int t, Rgb c)
{
    rect(o, x, y, w, t, c, 255);
    rect(o, x, y + h - t, w, t, c, 255);
    rect(o, x, y, t, h, c, 255);
    rect(o, x + w - t, y, t, h, c, 255);
}

/* ---- text ---------------------------------------------------------- */

static const PfGlyph *glyph(int f, int ch)
{
    const PfGlyph *g;

    if (ch < 32 || ch > 126) {
        ch = '?';
    }
    g = &PF_GLYPH[PF_FONT[f].first + ch - 32];
    return g;
}

/* Glyphs the font lacks fall back to a plain box of the cap height
 * (none of the panel strings use them). */
static int text_w(int f, const char *s)
{
    int w = 0;

    for (; *s; s++) {
        w += glyph(f, (unsigned char)*s)->adv;
    }
    return w;
}

static void text(uint32_t *o, int x, int y, int f, const char *s, Rgb c)
{
    /* y = top of the text line; baseline at y + ascent. */
    for (; *s; s++) {
        const PfGlyph *g = glyph(f, (unsigned char)*s);
        int i;
        int j;

        if (g->off < 0) {
            if (*s != ' ') {
                int cap = PF_FONT[f].ascent * 3 / 4;

                frame(o, x + 1, y + PF_FONT[f].ascent - cap, g->adv - 2 > 2 ? g->adv - 2 : 2,
                      cap, 1, c);
            }
            x += g->adv;
            continue;
        }
        for (j = 0; j < g->h; j++) {
            for (i = 0; i < g->w; i++) {
                int v = (PF_DATA[g->off + j * ((g->w + 1) / 2) + i / 2] >> ((i & 1) * 4)) & 15;

                if (v) {
                    blend(o, x + g->ox + i, y + g->oy + j, c, v * 17);
                }
            }
        }
        x += g->adv;
    }
}

/* Tabular figures: every digit takes the widest digit's advance and
 * sits centred in it, so changing numbers do not jitter. */
static int tab_adv(int f)
{
    int k;
    int m = 0;

    for (k = '0'; k <= '9'; k++) {
        if (glyph(f, k)->adv > m) {
            m = glyph(f, k)->adv;
        }
    }
    return m;
}

static int tab_w(int f, const char *s)
{
    int w = 0;

    for (; *s; s++) {
        w += (*s >= '0' && *s <= '9') ? tab_adv(f) : glyph(f, (unsigned char)*s)->adv;
    }
    return w;
}

static void tab(uint32_t *o, int x, int y, int f, const char *s, Rgb c)
{
    int m = tab_adv(f);

    for (; *s; s++) {
        char one[2] = {*s, 0};

        if (*s >= '0' && *s <= '9') {
            text(o, x + (m - glyph(f, (unsigned char)*s)->adv) / 2, y, f, one, c);
            x += m;
        } else {
            text(o, x, y, f, one, c);
            x += glyph(f, (unsigned char)*s)->adv;
        }
    }
}

static void tab_r(uint32_t *o, int rx, int y, int f, const char *s, Rgb c)
{
    tab(o, rx - tab_w(f, s), y, f, s, c);
}

static void tab_c(uint32_t *o, int cx, int y, int f, const char *s, Rgb c)
{
    int w = tab_w(f, s);
    int l = 0;
    int r = w;
    size_t n = strlen(s);

    /* Ink edges; digits sit centred in the tabular advance. */
    if (n) {
        unsigned char a = (unsigned char)s[0];
        unsigned char z = (unsigned char)s[n - 1];
        const PfGlyph *ga = glyph(f, a);
        const PfGlyph *gz = glyph(f, z);
        int m = tab_adv(f);

        l = (a >= '0' && a <= '9') ? (m - ga->adv) / 2 + ga->ox : ga->ox;
        r = (z >= '0' && z <= '9') ? w - m + (m - gz->adv) / 2 + gz->ox + gz->w
                                   : w - gz->adv + gz->ox + gz->w;
    }
    tab(o, cx - (l + r) / 2, y, f, s, c);
}

/* Ink extent of a line: from the first glyph's left ink to the last
 * glyph's right ink, relative to the pen start. */
static void ink_x(int f, const char *s, int adv_total, int *l, int *r)
{
    size_t n = strlen(s);
    const PfGlyph *g0;
    const PfGlyph *gl;

    if (!n) { *l = 0; *r = 0; return; }
    g0 = glyph(f, (unsigned char)s[0]);
    gl = glyph(f, (unsigned char)s[n - 1]);
    *l = g0->ox;
    *r = adv_total - gl->adv + gl->ox + gl->w;
}

/* Centred on its ink, not on the advance box. */
static void text_c(uint32_t *o, int cx, int y, int f, const char *s, Rgb c)
{
    int l;
    int r;

    ink_x(f, s, text_w(f, s), &l, &r);
    text(o, cx - (l + r) / 2, y, f, s, c);
}

static void text_r(uint32_t *o, int rx, int y, int f, const char *s, Rgb c)
{
    text(o, rx - text_w(f, s), y, f, s, c);
}

/* Vertically centre a line of font f in a band [y, y+h): the cap
 * height measured from the font's own H (ink, not the font box). */
static int mid(int f, int y, int h)
{
    const PfGlyph *g = glyph(f, 'H');

    return y + (h - g->h + 1) / 2 - g->oy;
}

/* Panel boxes: 45-degree cuts of CHAMF dots on the top-left and
 * bottom-right corners, applied alike to fill, title bar and frame. */
#define CHAMF 20

/* Cap height of a size, and the pen y that puts its cap top at top. */
static int cap(int f)
{
    return glyph(f, 'H')->h;
}

static int at_cap(int f, int top)
{
    return top - glyph(f, 'H')->oy;
}

/* Top of a block gh tall centred in the content band of a box with a
 * title strip at y, h tall. */
static int band_top(int y, int h, int gh)
{
    return y + HEAD + (h - 3 - HEAD - gh + 1) / 2;
}

static int cham_in(int px, int py, int w, int h, int k)
{
    return px >= 0 && py >= 0 && px < w && py < h &&
           px + py >= k && (w - 1 - px) + (h - 1 - py) >= k;
}

/* Boxes drawn this frame, for the padding check (test hook). */
typedef struct PBox {
    int x, y, w, h, head;
    const char *label;
    const char *extra;
    Rgb extra_c;
} PBox;
static PBox g_boxes[32];
static int g_nbox;

/* Inner padding: content keeps this many dots clear of the frame's
 * inner edge on every side (the 3-dot frame, the title strip, and the
 * 45-degree cuts). */
#define INSET 8
#define MX 24 /* side margin of every box's content, from the outer edge */

static void box_shape(uint32_t *o, int x, int y, int w, int h, const char *label,
                      const char *extra, Rgb extra_c)
{
    int px;
    int py;
    /* A 3-dot frame along the diagonal is 3*sqrt(2) ~ 4 dots deep. */
    const int t = 3;
    const int td = 5;

    for (py = 0; py < h; py++) {
        for (px = 0; px < w; px++) {
            int edge;

            if (!cham_in(px, py, w, h, CHAMF)) {
                continue;
            }
            edge = px < t || py < t || px >= w - t || py >= h - t ||
                   px + py < CHAMF + td || (w - 1 - px) + (h - 1 - py) < CHAMF + td;
            if (edge) {
                blend(o, x + px, y + py, C_EDGE, 255);
            } else if (label && py < HEAD) {
                blend(o, x + px, y + py, C_HEAD, 255);
            } else {
                blend(o, x + px, y + py, C_FILL, 200);
            }
        }
    }
    if (label) {
        /* Centred on the whole width by its ink (the cut only takes a
         * corner triangle, so the strip's optical middle stays at w/2),
         * and on the cap height between the frame and the strip's
         * lower edge. */
        int lw = text_w(F20, label);
        int ew = extra ? text_w(F20, extra) + 12 : 0;
        int il;
        int ir;
        int lx;
        int ly = mid(F20, y + t, HEAD - t);

        if (extra) {
            int el;
            int er;

            ink_x(F20, label, lw, &il, &ir);
            ink_x(F20, extra, ew - 12, &el, &er);
            ir = lw + 12 + er;
        } else {
            ink_x(F20, label, lw, &il, &ir);
        }
        lx = x + w / 2 - (il + ir) / 2;
        text(o, lx, ly, F20, label, C_DIM);
        if (extra) {
            text(o, lx + lw + 12, ly, F20, extra, extra_c);
        }
    }
    if (g_nbox < (int)(sizeof g_boxes / sizeof g_boxes[0])) {
        PBox *b = &g_boxes[g_nbox++];

        b->x = x; b->y = y; b->w = w; b->h = h; b->head = label != NULL;
        b->label = label; b->extra = extra; b->extra_c = extra_c;
    }
}

/* Chamfered outline only, for empty slots. */
static void cham_outline(uint32_t *o, int x, int y, int w, int h, int k, Rgb c)
{
    int px;
    int py;

    for (py = 0; py < h; py++) {
        for (px = 0; px < w; px++) {
            if (cham_in(px, py, w, h, k) &&
                (px < 2 || py < 2 || px >= w - 2 || py >= h - 2 ||
                 px + py < k + 3 || (w - 1 - px) + (h - 1 - py) < k + 3)) {
                blend(o, x + px, y + py, c, 255);
            }
        }
    }
}

static void box(uint32_t *o, int x, int y, int w, int h, const char *label)
{
    box_shape(o, x, y, w, h, label, NULL, C_DIM);
}

/* Blocks that fill from the left: fill[i] is block i's lit share, 0..256. */
static void step_bar(uint32_t *o, int x, int y, int w, int h, int n, const int *fill, Rgb c)
{
    int i;
    int gap = 4;
    int sw = (w - gap * (n - 1)) / n;

    for (i = 0; i < n; i++) {
        int sx = x + i * (sw + gap);
        int f = fill[i] < 0 ? 0 : fill[i] > 256 ? 256 : fill[i];

        rect(o, sx, y, sw, h, scale(c, 1, 6), 255);
        if (f > 0) {
            rect(o, sx, y, f >= 256 ? sw : (sw * f) >> 8, h, f >= 256 ? c : scale(c, 3, 5), 255);
        }
    }
}

/* ---- background ---------------------------------------------------- */

/* Metal plate at 1:1. The same title-column tiling as the 1920 layout,
 * repeated, never stretched: a window just crops a bigger piece of it. */
uint32_t panels_plate_at(int x, int y)
{
    int ax = x < 0 ? -x : x;
    int ay = y < 0 ? -y : y;

    if (!g_have_title) {
        return pack(C_BG);
    }
    {
        int sy = (ay / 3) % 400;
        int u = ax + 48;
        int tile = u / 132;
        int tx = (u % 132) / 3;
        int sx = (tile & 1) ? 43 - tx : tx;
        Rgb c;
        Rgb b = C_BG;

        if (sx > 43) {
            sx = 43;
        }
        c = unpack(g_title[sy * 640 + sx]);
        b.r += (c.r - b.r) * 45 / 100;
        b.g += (c.g - b.g) * 45 / 100;
        b.b += (c.b - b.b) * 45 / 100;
        return pack(b);
    }
}

static void build_back(void)
{
    int x;
    int y;

    for (y = 0; y < PH; y++) {
        for (x = 0; x < PW; x++) {
            g_back[y * PW + x] = pack(C_BG);
        }
    }
    if (g_have_title) {
        /* Title art columns x 0-43, 3x, tiled with alternate mirror. */
        for (y = 0; y < PH; y++) {
            int sy = (y / 3) % 400;

            for (x = 0; x < FIELD_X; x++) {
                int u = x + 48;
                int tile = u / 132;
                int tx = (u % 132) / 3;
                int sx = (tile & 1) ? 43 - tx : tx;
                Rgb c;
                Rgb b = C_BG;

                if (sx > 43) sx = 43;
                c = unpack(g_title[sy * 640 + sx]);
                b.r += (c.r - b.r) * 45 / 100;
                b.g += (c.g - b.g) * 45 / 100;
                b.b += (c.b - b.b) * 45 / 100;
                g_back[y * PW + x] = pack(b);
                g_back[y * PW + (PW - 1 - x)] = pack(b);
            }
        }
    }
    g_back_ok = g_have_title;
}

void panels_capture_title(const uint32_t *rgba640)
{
    memcpy(g_title, rgba640, sizeof g_title);
    g_have_title = 1;
    if (((char *)0)) {
        FILE *f = fopen(((char *)0), "wb");

        if (f) {
            fwrite(g_title, 1, sizeof g_title, f);
            fclose(f);
        }
    }
    g_back_ok = -1;
}

const uint32_t *panels_title_pixels(void)
{
    return g_have_title ? g_title : NULL;
}

int panels_have_title(void)
{
    return g_have_title;
}

static void draw_logo(uint32_t *o, int x, int y, int w)
{
    const int sx0 = 45;
    const int sy0 = 118;
    const int sw = 553;
    const int sh = 106;
    int h = sh * w / sw;
    int i;
    int j;

    if (g_have_title) {
        for (j = 0; j < h; j++) {
            for (i = 0; i < w; i++) {
                uint32_t p = g_title[(sy0 + j * sh / h) * 640 + sx0 + i * sw / w];
                Rgb c = unpack(p);

                if (c.r + c.g + c.b > 60) {
                    blend(o, x + i, y + j, c, 255);
                }
            }
        }
    } else {
        text_c(o, x + w / 2, y + 4, F48, "RUDE BREAKER", C_W);
        h = 66;
    }
    {
        /* The subtitle spans the logo's ink exactly: the largest font
         * that fits, then even tracking so both edges line up. */
        static const char sub[] = "30TH ANNIVERSARY EDITION";
        int l = x;
        int r = x + w - 1;
        int f = F26;
        int n = (int)sizeof sub - 1;
        int ink;
        int extra;
        int k;
        int cx;
        const PfGlyph *g0;
        const PfGlyph *gl;

        if (g_have_title) {
            int found = 0;

            for (i = 0; i < w && !found; i++) {
                for (j = 0; j < h; j++) {
                    Rgb c = unpack(g_title[(sy0 + j * sh / h) * 640 + sx0 + i * sw / w]);
                    if (c.r + c.g + c.b > 60) { l = x + i; found = 1; break; }
                }
            }
            found = 0;
            for (i = w - 1; i >= 0 && !found; i--) {
                for (j = 0; j < h; j++) {
                    Rgb c = unpack(g_title[(sy0 + j * sh / h) * 640 + sx0 + i * sw / w]);
                    if (c.r + c.g + c.b > 60) { r = x + i; found = 1; break; }
                }
            }
        }
        for (;;) {
            g0 = glyph(f, (unsigned char)sub[0]);
            gl = glyph(f, (unsigned char)sub[n - 1]);
            ink = text_w(f, sub) - gl->adv + gl->ox + gl->w - g0->ox;
            if (ink <= r - l + 1 || f == F16) {
                break;
            }
            f--;
        }
        extra = (r - l + 1) - ink;
        cx = l - g0->ox;
        for (k = 0; k < n; k++) {
            char one[2] = {sub[k], 0};

            text(o, cx + extra * k / (n - 1), y + h + 8, f, one, C_W);
            cx += glyph(f, (unsigned char)sub[k])->adv;
        }
        gfx_halo(o, PW, PH, l - 4, y + h + 4, r + 4, y + h + 12 + cap(f) + 8,
                 gfx_glow_level(g_frame));
    }
}

/* ---- rank graph ---------------------------------------------------- */

static void line(uint32_t *o, int x0, int y0, int x1, int y1, Rgb c, int t)
{
    int dx = x1 > x0 ? x1 - x0 : x0 - x1;
    int dy = y1 > y0 ? y1 - y0 : y0 - y1;
    int n = dx > dy ? dx : dy;
    int i;

    if (n == 0) n = 1;
    for (i = 0; i <= n; i++) {
        int x = x0 + (x1 - x0) * i / n;
        int y = y0 + (y1 - y0) * i / n;

        rect(o, x - t / 2, y - t / 2, t, t, c, 255);
    }
}

/* 9x9 skull, drawn at 2x, centred on (cx, cy). */
static void skull(uint32_t *o, int cx, int cy, Rgb c)
{
    static const char *const art[9] = {
        "..#####..",
        ".#######.",
        "#########",
        "#..###..#",
        "#..###..#",
        "####.####",
        ".#######.",
        "..#.#.#..",
        "..#####..",
    };
    int i;
    int j;

    for (j = 0; j < 9; j++) {
        for (i = 0; i < 9; i++) {
            if (art[j][i] == '#') {
                rect(o, cx - 9 + i * 2, cy - 9 + j * 2, 2, 2, c, 255);
            }
        }
    }
}

/* 9x9 up arrow, drawn at 2x, centred on (cx, cy). */
static void up_arrow(uint32_t *o, int cx, int cy, Rgb c)
{
    static const char *const art[9] = {
        "....#....",
        "...###...",
        "..#####..",
        ".#######.",
        "#########",
        "...###...",
        "...###...",
        "...###...",
        "...###...",
    };
    int i;
    int j;

    for (j = 0; j < 9; j++) {
        for (i = 0; i < 9; i++) {
            if (art[j][i] == '#') {
                rect(o, cx - 9 + i * 2, cy - 9 + j * 2, 2, 2, c, 255);
            }
        }
    }
}

/* ---- rank OFF card -------------------------------------------------- */

/* Ink extent of a string: first and last lit column relative to x. */
static void ink_span(int f, const char *s, int sp, int *l, int *r)
{
    int x = 0;
    int lo = 1 << 30;
    int hi = -(1 << 30);

    for (; *s; s++) {
        const PfGlyph *g = glyph(f, (unsigned char)*s);

        if (*s != ' ' && g->w > 0) {
            if (x + g->ox < lo) lo = x + g->ox;
            if (x + g->ox + g->w > hi) hi = x + g->ox + g->w;
        }
        x += g->adv + (s[1] ? sp : 0);
    }
    *l = lo;
    *r = hi;
}

/* Text with per-gap extra spacing: `extra` dots spread evenly over the
 * gaps between letters so the line lands on an exact ink width. */
static void text_spread(uint32_t *o, int x, int y, int f, const char *s, int extra, Rgb c)
{
    int n = (int)strlen(s) - 1;
    int i;
    char ch[2] = {0, 0};

    for (i = 0; s[i]; i++) {
        ch[0] = s[i];
        text(o, x, y, f, ch, c);
        x += glyph(f, (unsigned char)s[i])->adv;
        if (n > 0 && s[i + 1]) {
            x += extra * (i + 1) / n - extra * i / n;
        }
    }
}

static void draw_rank_offline(uint32_t *o, int gx, int gy, int gw, int gh, uint32_t fr)
{
    /* 15 s cycle at the 56.4 Hz logic rate; the band rises for ~3 s. */
    enum { CYCLE = 846, SWEEP = 169, BAND = 28 };
    const char *a = "RANK SYSTEM";
    const char *b = "OFFLINE";
    const int fa = F26;
    int al, ar, bl, br, fb, extra, wa, gap, ha, hb, top, x0;
    Rgb c = C_G;
    int ph = (int)(fr % CYCLE);
    int k;

    ink_span(fa, a, 0, &al, &ar);
    wa = ar - al;
    /* Largest font whose plain OFFLINE fits, then letter spacing. */
    for (fb = F60; fb > F16; fb--) {
        ink_span(fb, b, 0, &bl, &br);
        if (br - bl <= wa) break;
    }
    ink_span(fb, b, 0, &bl, &br);
    extra = wa - (br - bl);
    ha = cap(fa);
    hb = cap(fb);
    gap = 14;
    /* Optical centre: block sits a hair above the geometric middle. */
    top = gy + (gh - (ha + gap + hb)) / 2 - 4;
    x0 = gx + (gw - wa) / 2;
    text(o, x0 - al, at_cap(fa, top), fa, a, c);
    text_spread(o, x0 - bl, at_cap(fb, top + ha + gap), fb, b, extra, c);
    /* CRT sweep: a soft bright band rising bottom to top. */
    if (ph < SWEEP) {
        int cy = gy + gh + BAND - (gh + 2 * BAND) * ph / SWEEP;

        for (k = -BAND; k <= BAND; k++) {
            int y = cy + k;
            int al2 = 64 * (BAND - (k < 0 ? -k : k)) / BAND;

            if (y <= gy || y >= gy + gh - 1 || al2 <= 0) continue;
            rect(o, gx + 1, y, gw - 2, 1, C_EDGE, al2);
        }
        if (cy > gy && cy < gy + gh - 1) {
            rect(o, gx + 1, cy, gw - 2, 1, C_W, 48);
        }
    }
}

static void draw_rank(uint32_t *o, int X, const RbPanelData *d)
{
    /* Axis labels sit inside the side margins: values right of X+MX,
     * the last time label centred on the graph's right end. */
    const int gx = X + MX + text_w(F20, "160") + 8;
    const int gy = 78;
    const int gw = X + 388 - MX - text_w(F20, "0:00") / 2 - gx;
    const int gh = 230;
    /* Fixed 60 s window that scrolls: the newest sample sits at the
     * right once the stage has run a minute; older data drops off. */
    const int total = 60;
    const int s0 = d->rank_n > total + 1 ? d->rank_n - (total + 1) : 0;
    const int step = 60;
    char buf[48];
    int v;
    int t;
    int i;

    {
        /* LOOP on, loop 2 and later: the loop number beside the title. */
        static char lp[12];

        snprintf(lp, sizeof lp, "LOOP %d", d->loop < 1 ? 1 : d->loop > 99 ? 99 : d->loop);
        box_shape(o, X, 16, 388, 392, "RANK", d->loop_on && d->loop > 1 ? lp : NULL, C_Y);
    }
    rect(o, gx, gy, gw, gh, C_BG, 255);
    for (v = 0; v <= 160; v += 40) {
        int y = gy + gh - v * gh / 160;

        if (d->rank_on) {
            rect(o, gx, y, gw, 1, scale(C_EDGE, 1, 2), 255);
        }
        if (d->rank_on && !d->rank_adv) {
            /* ADVANCED keeps the rank hidden: no values on the axis. */
            snprintf(buf, sizeof buf, "%d", v);
            text_r(o, gx - 8, y - 10, F20, buf, C_DIM);
        }
    }
    for (t = 0; d->rank_on && t <= total; t += step) {
        int x = gx + t * (gw - 1) / total;

        rect(o, x, gy + gh, 1, 6, C_DIM, 255);
        snprintf(buf, sizeof buf, "%d:%02d", (s0 + t) / 60, (s0 + t) % 60);
        text_c(o, x, gy + gh + 8, F20, buf, C_DIM);
    }
    frame(o, gx - 1, gy - 1, gw + 2, gh + 2, 1, C_EDGE);
    /* Area under the rank line, filled in translucent red. */
    for (i = s0 + 1; d->rank_on && i < d->rank_n; i++) {
        int x0 = gx + (i - 1 - s0) * (gw - 1) / total;
        int x1 = gx + (i - s0) * (gw - 1) / total;
        int y0 = gy + gh - 1 - d->rank_hist[i - 1] * (gh - 1) / 160;
        int y1 = gy + gh - 1 - d->rank_hist[i] * (gh - 1) / 160;
        int x;

        for (x = x0 + (i > s0 + 1 ? 1 : 0); x <= x1; x++) {
            int y = x1 > x0 ? y0 + (y1 - y0) * (x - x0) / (x1 - x0) : y1;

            rect(o, x, y, 1, gy + gh - y, C_R, 80);
        }
    }
    for (i = 0; d->rank_on && i < d->death_n; i++) {
        int x;

        if (d->deaths[i] < s0) {
            continue;
        }
        x = gx + (d->deaths[i] - s0) * (gw - 1) / total;

        rect(o, x, gy, 2, gh, scale(C_R, 2, 3), 255);
        skull(o, x + 1, gy - 13, C_W);
    }
    for (i = 0; d->rank_on && i < d->up_n; i++) {
        int x;

        if (d->ups[i] < s0) {
            continue;
        }
        x = gx + (d->ups[i] - s0) * (gw - 1) / total;

        rect(o, x, gy, 2, gh, scale(C_G, 2, 3), 255);
        up_arrow(o, x + 1, gy - 13, C_G);
    }
    for (i = s0 + 1; d->rank_on && i < d->rank_n; i++) {
        int x0 = gx + (i - 1 - s0) * (gw - 1) / total;
        int x1 = gx + (i - s0) * (gw - 1) / total;
        int y0 = gy + gh - 1 - d->rank_hist[i - 1] * (gh - 1) / 160;
        int y1 = gy + gh - 1 - d->rank_hist[i] * (gh - 1) / 160;

        line(o, x0, y0, x1, y1, C_R, 3);
    }
    if (!d->rank_on) {
        draw_rank_offline(o, gx, gy, gw, gh, d->frame);
    } else {
        int x = gx + (d->rank_n > 0 ? d->rank_n - 1 - s0 : 0) * (gw - 1) / total;
        int y = gy + gh - 1 - d->rank_now * (gh - 1) / 160;
        int a;
        int b;

        for (b = -5; b <= 5; b++) {
            for (a = -5; a <= 5; a++) {
                if (a * a + b * b <= 25) {
                    blend(o, x + a, y + b, C_Y, 255);
                }
            }
        }
    }
    if (d->rank_on && d->rank_adv) {
        /* ADVANCED: a discreet bar, no numbers. */
        int t0 = gy + gh + 8 + cap(F20) + 4;
        int top = t0 + (16 + 392 - 3 - t0 - cap(F34) + 1) / 2;
        int bx = X + MX, bw = 388 - 2 * MX, bh = 10;
        int by = top + (cap(F34) - bh) / 2;
        int fill = bw * (d->rank_bar < 0 ? 0 : d->rank_bar > 1000 ? 1000 : d->rank_bar) / 1000;

        /* Progress toward the next LEVEL band; full = maxed (top band
         * or ceiling), drawn brighter with an edge. */
        rect(o, bx, by, bw, bh, scale(C_Y, 1, 6), 255);
        rect(o, bx, by, fill, bh, d->rank_bar >= 1000 ? C_Y : scale(C_Y, 2, 3), 255);
        if (d->rank_bar >= 1000) {
            frame(o, bx - 2, by - 2, bw + 4, bh + 4, 1, C_Y);
        }
    } else if (d->rank_on) {
        /* NOW row: centred between the time labels and the frame. */
        int t0 = gy + gh + 8 + cap(F20) + 4;
        int top = t0 + (16 + 392 - 3 - t0 - cap(F34) + 1) / 2;
        int yb = at_cap(F20, top + cap(F34) - cap(F20));

        text(o, X + MX, yb, F20, "NOW", C_DIM);
        snprintf(buf, sizeof buf, "%d", d->rank_now);
        t = X + MX + text_w(F20, "NOW") + 12;
        text(o, t, at_cap(F34, top), F34, buf, C_Y);
        text(o, t + text_w(F34, buf) + 10, yb, F20, "/ 159", C_DIM);
        if (d->rank_mul > 0) {
            snprintf(buf, sizeof buf, "x%d.%02d", d->rank_mul / 100, d->rank_mul % 100);
            text_r(o, X + 388 - MX, yb, F20, buf, d->rank_mul > 100 ? C_Y : C_DIM);
        }
    }
}

/* ---- arcade stick and buttons ------------------------------------- */

/* Pixel art: shapes are computed on a 3 px grid, colours quantised. */
#define PIX 3

static void pix(uint32_t *o, int x, int y, Rgb c)
{
    rect(o, x, y, PIX, PIX, c, 255);
}

static Rgb shade(Rgb base, double l)
{
    /* Quantise to 5 tones for the pixel look. */
    int q = (int)(l * 4.0 + 0.5);
    static const int k[5] = {35, 60, 85, 110, 150};

    if (q < 0) q = 0;
    if (q > 4) q = 4;
    return scale(base, k[q], 100);
}

static void ellipse(uint32_t *o, int cx, int cy, int rx, int ry, Rgb c)
{
    int x;
    int y;

    for (y = -ry; y <= ry; y += PIX) {
        for (x = -rx; x <= rx; x += PIX) {
            double u = (x + 1.5) / (rx + 1.5);
            double v = (y + 1.5) / (ry + 1.5);

            if (u * u + v * v <= 1.0) {
                pix(o, cx + x, cy + y, c);
            }
        }
    }
}

static void draw_stick(uint32_t *o, int cx, int cy, uint16_t b)
{
    int dx = (b & RB_BTN_RIGHT ? 1 : 0) - (b & RB_BTN_LEFT ? 1 : 0);
    int dy = (b & RB_BTN_DOWN ? 1 : 0) - (b & RB_BTN_UP ? 1 : 0);
    double len = sqrt((double)(dx * dx + dy * dy));
    /* The lever pivots at the washer: length L, tilted by TH towards
     * the pressed direction. Sideways it moves L sin TH and drops
     * L (1 - cos TH); front/back motion is depth, seen at 0.6 in the
     * 3/4 view. */
    const double L = 49.0;
    const double TH = 25.0 * 3.14159265358979 / 180.0;
    int bx = cx;
    int by = cy - 54;
    Rgb plate = scale(C_EDGE, 60, 100);
    Rgb black = scale(C_EDGE, 33, 100);
    Rgb ball = C_R;
    int x;
    int y;
    int s;

    if (len > 0.0) {
        double h = L * sin(TH);

        bx += (int)lround(dx / len * h);
        by += (int)lround(L * (1.0 - cos(TH)) + dy / len * h * 0.6);
    }
    /* Base plate with screws, then the dust washer. */
    ellipse(o, cx, cy, 54, 24, scale(plate, 60, 100));
    ellipse(o, cx, cy - 3, 51, 21, plate);
    pix(o, cx - 42, cy - 3, scale(plate, 40, 100));
    pix(o, cx + 39, cy - 3, scale(plate, 40, 100));
    ellipse(o, cx, cy - 3, 33, 12, scale(black, 70, 100));
    ellipse(o, cx, cy - 5, 27, 9, black);
    /* Shaft from the washer to the ball. */
    for (s = 0; s <= 20; s++) {
        int x0 = cx + (bx - cx) * s / 20;
        int y0 = cy - 5 + (by + 18 - (cy - 5)) * s / 20;

        pix(o, x0 - 6, y0, scale(C_W, 45, 100));
        pix(o, x0 - 3, y0, scale(C_W, 70, 100));
        pix(o, x0, y0, scale(C_W, 55, 100));
        pix(o, x0 + 3, y0, scale(C_W, 35, 100));
    }
    /* Ball top: Lambert shading with a light from the upper left. */
    for (y = -33; y <= 33; y += PIX) {
        for (x = -33; x <= 33; x += PIX) {
            double u = (x + 1.5) / 34.5;
            double v = (y + 1.5) / 34.5;
            double r2 = u * u + v * v;
            double z;
            double l;

            if (r2 > 1.0) {
                continue;
            }
            z = sqrt(1.0 - r2);
            l = (-0.45 * u - 0.55 * v + 0.70 * z);
            if (l < 0.0) l = 0.0;
            if (r2 > 0.8) l *= 0.7;
            pix(o, bx + x, by + y, shade(ball, l));
        }
    }
    /* Specular highlight. */
    pix(o, bx - 15, by - 18, C_W);
    pix(o, bx - 12, by - 18, C_W);
    pix(o, bx - 18, by - 15, C_W);
    pix(o, bx - 15, by - 15, scale(C_W, 85, 100));
}

static void draw_button(uint32_t *o, int cx, int cy, Rgb hue, int lit, const char *key,
                        const char *name)
{
    const int r = 24;
    Rgb bezel = C_EDGE;
    int x;
    int y;

    for (y = -r - 6; y <= r + 6; y += PIX) {
        for (x = -r - 6; x <= r + 6; x += PIX) {
            double d = sqrt((x + 1.5) * (x + 1.5) + (y + 1.5) * (y + 1.5));

            if (d > r + 6) {
                continue;
            }
            if (d > r) {
                /* Bezel ring, lit on the upper left. */
                double l = 0.5 - 0.35 * (x + y) / (double)(r + 6);

                pix(o, cx + x, cy + y, shade(bezel, l));
            } else {
                /* Concave face: dark at the top, light at the bottom. */
                double l = 0.45 + 0.4 * y / (double)r - 0.15 * (d / r);
                Rgb base = lit ? hue : scale(C_EDGE, 70, 100);

                if (lit) l += 0.2;
                pix(o, cx + x, cy + y, shade(base, l));
            }
        }
    }
    if (lit) {
        /* Glow ring. */
        for (y = -r - 12; y <= r + 12; y++) {
            for (x = -r - 12; x <= r + 12; x++) {
                double d = sqrt((double)(x * x + y * y));

                if (d > r + 6 && d <= r + 12) {
                    blend(o, cx + x, cy + y, hue, (int)(120 * (r + 12 - d) / 6));
                }
            }
        }
    }
    text_c(o, cx, cy + r + 14, F20, key, hue);
    text_c(o, cx, cy + r + 36, F16, name, C_DIM);
}

static void draw_input(uint32_t *o, int X, int Y, uint16_t b)
{
    box(o, X, Y, 388, 280, "INPUT");
    /* Stick column 108 wide at the left margin, three equal button
     * columns over the rest; the whole group centred in the band
     * (ink from the top of the lit rings to the base plate). */
    const int top = band_top(Y, 280, 132);
    const int bx0 = X + MX + 108 + 8;
    const int cw = (X + 388 - MX - bx0) / 3;

    draw_stick(o, X + MX + 54, top + 132 - 24, b);
    draw_button(o, bx0 + cw / 2, top + 36, C_R, (b & RB_BTN_A) != 0, "Z", "SHOT");
    draw_button(o, bx0 + cw + cw / 2, top + 36, C_Y, (b & RB_BTN_B) != 0, "X", "MODE");
    draw_button(o, bx0 + 2 * cw + cw / 2, top + 36, C_G, (b & RB_BTN_SPEED) != 0, "C", "SPEED");
}

/* ---- columns ------------------------------------------------------- */

static void draw_left(uint32_t *o, const uint32_t *game, const RbPanelData *d)
{
    (void)game;
    const int X = 16;
    static const char *const names[3] = {"LOW", "MID", "MAX"};
    char buf[32];
    int i;
    int j;

    draw_rank(o, X, d);

    box(o, X, 448, 388, 176, "LOADOUT");
    {
        const int it = band_top(448, 176, 96);

        if (d->capsule) {
            for (j = 0; j < 96; j++) {
                for (i = 0; i < 96; i++) {
                    uint32_t p = d->capsule[(j / 3) * 32 + i / 3];

                    if (p & 0xFFu) {
                        blend(o, X + MX + i, it + j, unpack(p), 255);
                    }
                }
            }
        } else {
            /* No sub-weapon: an empty slot keeps the box balanced. */
            cham_outline(o, X + MX + 8, it + 8, 80, 80, 14, scale(C_EDGE, 2, 3));
        }
    }
    {
        /* One block per level, level 1 included (from the game data:
         * the shot script branches on levels 0..3, so SHOT has 4; the
         * capsule script branches on sub levels 0..2 for every type, so
         * VULCAN, MISSILE and LASER have 3). Level 1 fills the first
         * block; the next SHOT block fills item by item towards its
         * level (2, 10 and 18 power-up items). Two rows (label, then
         * value with its bar on the value's cap line), centred as a
         * group on the icon. */
        static const int at[4] = {0, 2, 10, 18};
        const char *wn = !d->sub_on ? "NO SUB" : d->weapon == 'L' ? "LASER"
                         : d->weapon == 'M' ? "MISSILE" : d->weapon == 'V' ? "VULCAN" : "-";
        const int rowh = cap(F20) + 10 + cap(F26);
        const int gap = 24;
        const int top = band_top(448, 176, 2 * rowh + gap);
        int lx = X + MX + 96 + 16;
        int bx = X + 236;
        int bw = X + 388 - MX - bx;
        int fill[4];
        int k;
        int r;

        fill[0] = 256;
        for (k = 1; k < 4; k++) {
            if (d->shot_lv > k) {
                fill[k] = 256;
            } else if (d->shot_lv == k) {
                int n = d->shot_items - at[k - 1];

                fill[k] = n <= 0 ? 0 : n * 256 / (at[k] - at[k - 1]);
                if (fill[k] > 255) fill[k] = 255;
            } else {
                fill[k] = 0;
            }
        }
        for (r = 0; r < 2; r++) {
            int ry = top + r * (rowh + gap);
            int vy = ry + cap(F20) + 10;

            if (r == 1) {
                for (k = 0; k < 3; k++) {
                    fill[k] = d->sub_on && d->weapon_lv >= k + 1 ? 256 : 0;
                }
                snprintf(buf, sizeof buf, !d->sub_on ? "-" : d->weapon_lv >= 3 ? "MAX" : "LV %d",
                         d->weapon_lv);
            } else {
                snprintf(buf, sizeof buf, d->shot_lv >= 4 ? "MAX" : "LV %d", d->shot_lv);
            }
            text(o, lx, at_cap(F20, ry), F20, r ? wn : "SHOT", C_DIM);
            text(o, lx, at_cap(F26, vy), F26, buf, C_Y);
            step_bar(o, bx, vy, bw, cap(F26), r ? 3 : 4, fill, r ? C_G : C_Y);
        }
    }

    /* Lives: ship icon and count, centred together in the band. */
    box(o, X, 664, 190, 120, "EXTENDS");
    {
        int x0 = 32, y0 = 48, x1 = -1, y1 = -1;
        int iw = 0;
        int ih = 0;
        int gw;
        int gx;

        if (d->ship) {
            for (j = 0; j < 48; j++) {
                for (i = 0; i < 32; i++) {
                    if (d->ship[j * 32 + i] & 0xFFu) {
                        if (i < x0) x0 = i;
                        if (i > x1) x1 = i;
                        if (j < y0) y0 = j;
                        if (j > y1) y1 = j;
                    }
                }
            }
            if (x1 >= x0) {
                iw = x1 - x0 + 1;
                ih = y1 - y0 + 1;
            }
        }
        int lf = F34;
        int gap = 16;
        int tw0;

        snprintf(buf, sizeof buf, "x %d", d->lives);
        /* Two or more digits must still fit inside the frame: tighten
         * the gap, then drop to the smaller font. */
        tw0 = tab_w(lf, buf) > tab_w(lf, "x 8") ? tab_w(lf, buf) : tab_w(lf, "x 8");
        gw = (iw ? iw * 56 / ih + gap : 0) + tw0;
        if (gw > 190 - 2 * MX) {
            gap = 8;
            gw -= 8;
        }
        if (gw > 190 - 2 * MX) {
            lf = F26;
            tw0 = tab_w(lf, buf);
            gw = (iw ? iw * 56 / ih + gap : 0) + tw0;
        }
        gx = X + (190 - gw) / 2;
        if (iw) {
            /* Scaled to 56 dots tall (nearest dot), the same height as
             * the I-FRAMES pie beside it. */
            const int th = 56;
            int tw = iw * th / ih;
            int px = gx;
            int py = band_top(664, 120, th);

            for (j = 0; j < th; j++) {
                for (i = 0; i < tw; i++) {
                    uint32_t p = d->ship[(y0 + j * ih / th) * 32 + x0 + i * iw / tw];

                    if (p & 0xFFu) {
                        blend(o, px + i, py + j, unpack(p), 255);
                    }
                }
            }
            gx += tw + gap;
        }
        tab(o, gx, at_cap(lf, band_top(664, 120, cap(lf))), lf, buf, C_Y);
    }
    /* I-FRAMES: the ship's invulnerability timer (global DF, one count
     * per logic frame) as a pie: a full disc when granted, a wedge
     * opening clockwise from 12 o'clock as it runs out, then only the
     * faint outline. Yellow after a power-up item, violet after a
     * respawn. Pie and count are centred together in the band. */
    box(o, X + 198, 664, 190, 120, "I-FRAMES");
    {
        const int ro = 28;
        const int nw = tab_w(F26, "888");
        const int gx = X + 198 + (190 - (2 * ro + 16 + nw)) / 2;
        const int cx = gx + ro;
        const int cy = band_top(664, 120, 2 * ro) + ro;
        double f = d->iframe_max > 0 ? (double)d->iframe / d->iframe_max : 0.0;
        Rgb c = d->iframe_respawn ? C_C : C_Y;

        if (f > 1.0) f = 1.0;
        for (j = -ro; j < ro; j += PIX) {
            for (i = -ro; i < ro; i += PIX) {
                double r = sqrt((i + 1.5) * (i + 1.5) + (j + 1.5) * (j + 1.5));
                double a;

                if (r > ro) {
                    continue;
                }
                /* Clockwise from the top. */
                a = atan2(i + 1.5, -(j + 1.5)) / 6.283185307179586;
                if (a < 0.0) a += 1.0;
                if (a >= 1.0 - f) {
                    pix(o, cx + i, cy + j, c);
                } else if (r > ro - 4.0) {
                    pix(o, cx + i, cy + j, scale(c, 1, 4));
                }
            }
        }
        snprintf(buf, sizeof buf, "%d", d->iframe);
        tab_c(o, gx + 2 * ro + 16 + nw / 2, at_cap(F26, cy - cap(F26) / 2), F26, buf,
              d->iframe ? C_Y : C_DIM);
    }

    box(o, X, 824, 388, 88, "SPEED");
    {
        /* The lit block dashes to the new step: 4 frames with a hard
         * ease-out, leaving fading afterimages that clear 4 frames
         * after it lands. */
        enum { DASH = 4, FADE = 4 };
        static int cur = -1;
        static int from;
        static uint32_t t0;
        int dt;
        int pos; /* block x, in 1/256 px offsets from the LOW slot */
        enum { BW = 104, BH = 32 };
        int dxw = BW + 14;
        int base = X + MX;
        const int by = band_top(824, 88, BH);
        int k;

        if (cur < 0 || d->frame < t0) {
            cur = d->speed;
            from = d->speed;
            t0 = d->frame - (DASH + FADE);
        }
        if (d->speed != cur) {
            /* A new press mid-dash starts from where the block is. */
            from = cur;
            cur = d->speed;
            t0 = d->frame;
        }
        dt = (int)(d->frame - t0);
        for (i = 0; i < 3; i++) {
            int px = base + i * dxw;

            rect(o, px, by, BW, BH, scale(C_EDGE, 1, 2), 160);
        }
        {
            double u = dt >= DASH ? 1.0 : (double)dt / DASH;
            double e = 1.0 - (1.0 - u) * (1.0 - u) * (1.0 - u);
            double fx = from * dxw + (cur - from) * dxw * e;

            pos = (int)(fx + 0.5);
            if (dt < DASH + FADE && from != cur) {
                /* Afterimages: copies back along the path toward the
                 * start, each fainter, the whole trail fading out. */
                int life = dt < DASH ? 255 : 255 * (DASH + FADE - dt) / FADE;
                int dir = cur > from ? 1 : -1;
                int span = dt < DASH ? (int)(dxw * (1.2 - 0.6 * u)) : dxw * 6 / 10;

                for (k = 5; k >= 1; k--) {
                    int gx = base + pos - dir * span * k / 5;
                    int a = life * (6 - k) / 9;
                    int lo = base + (from < cur ? from : cur) * dxw;
                    int hi = base + (from > cur ? from : cur) * dxw;

                    if (gx < lo) gx = lo;
                    if (gx > hi) gx = hi;
                    rect(o, gx, by + k, BW, BH - 2 * k, C_G, a * 2 / 3);
                }
                /* Speed lines through the trail. */
                for (k = 0; k < 3; k++) {
                    int y = by + 6 + k * 10;
                    int x0 = base + pos + (dir > 0 ? 0 : BW);
                    int x1 = x0 - dir * span;

                    if (x1 > x0) {
                        int tt = x0;

                        x0 = x1;
                        x1 = tt;
                    }
                    rect(o, x0, y, x1 - x0, 2, C_W, life / 3);
                }
            }
            rect(o, base + pos, by, BW, BH, C_G, 255);
        }
        for (i = 0; i < 3; i++) {
            int px = base + i * dxw;
            int on = px + BW / 2 >= base + pos && px + BW / 2 < base + pos + BW;

            text_c(o, px + BW / 2, mid(F20, by, BH), F20, names[i], on ? C_BG : C_DIM);
        }
    }

    draw_logo(o, X, 967, 388);  /* subtitle baseline on the right column's bottom edge */
}

static void draw_right(uint32_t *o, const RbPanelData *d)
{
    const int X = 1516;
    static const char *const diffs[5] = {"VERY EASY", "EASY", "NORMAL", "HARD", "VERY HARD"};
    char buf[48];
    int i;

    box(o, X, 16, 388, 168, "SCORE");
    {
        /* Score over the high score, both flush right on the margin;
         * HI shares the high score's baseline. */
        const int top = band_top(16, 168, cap(F60) + 22 + cap(F34));
        const int hy = top + cap(F60) + 22;

        snprintf(buf, sizeof buf, "%u", (unsigned)d->score);
        tab_r(o, X + 388 - MX, at_cap(F60, top), F60, buf, C_Y);
        text(o, X + MX, at_cap(F20, hy + cap(F34) - cap(F20)), F20, "HI", C_DIM);
        snprintf(buf, sizeof buf, "%u", (unsigned)d->hiscore);
        tab_r(o, X + 388 - MX, at_cap(F34, hy), F34, buf, C_Y);
    }

    /* Below SCORE, equal gaps down to INPUT (box heights 168, 120,
     * 136, 64, 80 and 280 from y 16 to 1064, the same 40-dot gutter
     * as the left column). Every value sits on the centre line of
     * its box's band, inside the common MX side margin. */
    {
        enum { GAP = 40 };
        const int yh = 16 + 168 + GAP;
        const int ym = yh + 120 + GAP;
        const int yl = ym + 136 + GAP;
        const int yc = yl + 64 + GAP;
        const int yi = yc + 80 + GAP;

        /* HIT RATIO: enemies destroyed out of the stage's fixed enemy
         * count, with its bar; HIT COUNT the same two numbers. */
        box(o, X, yh, 190, 120, "HIT RATIO");
        box(o, X + 198, yh, 190, 120, "HIT COUNT");
        {
            int tot = d->foes_total > 0 ? d->foes_total : d->foes;
            int pc = tot > 0 ? (d->foes_hit * 100 + tot / 2) / tot : 0;
            const int bw = 142; /* 10% under the old 158, centred */
            const int top = band_top(yh, 120, cap(F34) + 14 + 16);
            int f;

            if (pc > 100) pc = 100;
            snprintf(buf, sizeof buf, "%d%%", pc);
            tab_c(o, X + 95, at_cap(F34, top), F34, buf, C_Y);
            rect(o, X + (190 - bw) / 2, top + cap(F34) + 14, bw, 16, scale(C_C, 1, 6), 255);
            rect(o, X + (190 - bw) / 2, top + cap(F34) + 14, bw * pc / 100, 16, C_C, 255);
            snprintf(buf, sizeof buf, "%d / %d", d->foes_hit, tot);
            for (f = F34; f > F16 && tab_w(f, buf) > 190 - 2 * MX; f--) {
            }
            tab_c(o, X + 198 + 95, at_cap(f, band_top(yh, 120, cap(f))), f, buf, C_Y);
        }

        if (d->medal_spots >= 0) {
            const int it = band_top(ym, 136, 64);
            const int tx = X + MX + 64 + 16;
            const int top = band_top(ym, 136, cap(F34) + 14 + 16);
            int j;

            box(o, X, ym, 388, 136, "MEDALS");
            if (d->medal) {
                for (j = 0; j < 64; j++) {
                    for (i = 0; i < 64; i++) {
                        uint32_t p = d->medal[(j / 2) * 32 + i / 2];

                        if (p & 0xFFu) {
                            blend(o, X + MX + i, it + j, unpack(p), 255);
                        }
                    }
                }
            }
            snprintf(buf, sizeof buf, "%d / %d", d->medals, d->medal_spots);
            tab(o, tx, at_cap(F34, top), F34, buf, C_Y);
            {
                int n = d->medal_spots > 0 ? d->medal_spots : 1;
                int bw = X + 388 - MX - tx;
                int by = top + cap(F34) + 14;

                rect(o, tx, by, bw, 16, scale(C_Y, 1, 6), 255);
                rect(o, tx, by, bw * (d->medals < n ? d->medals : n) / n, 16, C_Y, 255);
            }
        }

        /* LEVEL shows the difficulty in effect (it steps up with rank). */
        box(o, X, yl, 190, 64, "LEVEL");
        i = d->difficulty < 0 ? 0 : d->difficulty > 4 ? 4 : d->difficulty;
        {
            /* With the VERY HARD (or NORMAL) extra: VERY EASY and VERY
             * HARD shorten to V.EASY / V.HARD so the value fits. */
            static const char *const shorts[5] = {"V.EASY", "EASY", "NORMAL", "HARD", "V.HARD"};
            char lv[32];
            int f;
            int ty;

            if (d->level_pct > 0) {
                snprintf(lv, sizeof lv, "%s +%d%%", shorts[i], d->level_pct);
            } else {
                snprintf(lv, sizeof lv, "%s", diffs[i]);
            }
            f = text_w(F20, lv) > 190 - 2 * MX ? F16 : F20;
            ty = at_cap(f, band_top(yl, 64, cap(f)));
            if (d->level_flash > 0) {
                /* Step-up flash, pixel art on the game's 3 px grid
                 * (640x400 units): hard edges, no alpha. Only the dark
                 * area behind the value lights up (never the frame or
                 * the header): a 4x4 Bayer dither of white / cyan that
                 * thins in four steps, plus small 4-point stars inside
                 * it. The value is drawn on top. */
                static const uint8_t bayer[4][4] = {
                    {0, 8, 2, 10}, {12, 4, 14, 6}, {3, 11, 1, 9}, {15, 7, 13, 5}};
                const Rgb cy = {120, 240, 255};
                const int bw = 190, bh = 64, t = 3, td = 5;
                int lf = d->level_flash > 61 ? 61 : d->level_flash;
                int el = 61 - lf;
                int lvl = lf > 50 ? 12 : lf > 40 ? 8 : lf > 30 ? 4 : lf > 20 ? 2 : 0;
                int px, py, k;

#define LV_DARK(px_, py_) ((py_) >= HEAD && (px_) >= t && (px_) < bw - t && (py_) < bh - t && \
                           cham_in((px_), (py_), bw, bh, CHAMF) && \
                           (bw - 1 - (px_)) + (bh - 1 - (py_)) >= CHAMF + td && (px_) + (py_) >= CHAMF + td)
                for (py = HEAD; lvl && py < bh; py++) {
                    for (px = 0; px < bw; px++) {
                        int gx = (X + px) / PIX, gy = (yl + py) / PIX;

                        if (LV_DARK(px, py) && bayer[gy & 3][gx & 3] < lvl) {
                            blend(o, X + px, yl + py, (gx + gy) % 5 ? C_W : cy, 255);
                        }
                    }
                }
                if (lf > 14 && (el / 4) % 3 != 2) {
                    /* Small stars (5x5 / 3x3 cells) near the inner
                     * corners of the dark area, clipped to it. */
                    const int sx[4] = {6, bw - 8, 6, bw - 12};
                    const int sy[4] = {HEAD + 3, HEAD + 3, bh - 9, bh - 9};
                    int n = (el / 4) % 3 == 0 ? 2 : 1;

                    for (k = 0; k < 4; k++) {
                        int j, q;

                        for (j = -n; j <= n; j++) {
                            for (q = 0; q < PIX * PIX; q++) {
                                int ax = (sx[k] / PIX + j) * PIX + q % PIX, ay = (sy[k] / PIX) * PIX + q / PIX;
                                int bx = (sx[k] / PIX) * PIX + q % PIX, by2 = (sy[k] / PIX + j) * PIX + q / PIX;
                                Rgb sc = j ? C_Y : C_W;

                                if (LV_DARK(ax, ay)) blend(o, X + ax, yl + ay, sc, 255);
                                if (LV_DARK(bx, by2)) blend(o, X + bx, yl + by2, sc, 255);
                            }
                        }
                    }
                }
#undef LV_DARK
                /* Text (on top): dark on the dense dither, then white / yellow on
                 * alternate 3-frame steps, then the green / yellow. */
                text_c(o, X + 95, ty, f, lv,
                       lf > 40 ? C_BG : lf > 18 ? ((lf / 3) % 2 ? C_W : C_Y) :
                       ((lf / 4) % 2 ? C_G : C_Y));
            } else {
                text_c(o, X + 95, ty, f, lv, C_W);
            }
        }
        box(o, X + 198, yl, 190, 64, "TIME");
        snprintf(buf, sizeof buf, "%02u:%02u.%02u", (unsigned)(d->time_cs / 6000u % 100u),
                 (unsigned)(d->time_cs / 100u % 60u), (unsigned)(d->time_cs % 100u));
        tab_c(o, X + 198 + 95, at_cap(F20, band_top(yl, 64, cap(F20))), F20, buf, C_Y);

        /* NO DEATH: YES until the first life lost this run, then NO
         * (the count stays internal). Training shows in the header. */
        box_shape(o, X, yc, 388, 80, "NO DEATH", d->training ? "TRAINING" : NULL, C_G);
        {
            enum { PH2 = 26 };
            int pw = (388 - 2 * MX - 10) / 2;
            int ry = band_top(yc, 80, PH2);
            int ok = d->run_deaths == 0;
            const char *nb = "NO";
            rect(o, X + MX, ry, pw, PH2, ok ? C_G : scale(C_EDGE, 1, 2), ok ? 255 : 160);
            text_c(o, X + MX + pw / 2, mid(F20, ry, PH2), F20, "YES", ok ? C_BG : C_DIM);
            rect(o, X + MX + pw + 10, ry, pw, PH2, !ok ? C_R : scale(C_EDGE, 1, 2),
                 !ok ? 255 : 160);
            text_c(o, X + MX + pw + 10 + pw / 2, mid(F20, ry, PH2), F20, nb,
                   !ok ? C_BG : C_DIM);
        }

        draw_input(o, X, yi, d->buttons);
    }
}

/* test hook: redraw each box empty on the bare background and
 * flag every content dot closer than INSET to the frame's inner edge,
 * the title strip or a cut. Offenders are painted magenta and counted
 * on stderr; the safe area is outlined in cyan. */
static void pad_check(uint32_t *out)
{
    static uint32_t ref[PW * PH];
    int k;

    memcpy(ref, g_back, sizeof g_back);
    for (k = 0; k < g_nbox; k++) {
        const PBox *b = &g_boxes[k];
        int nb = g_nbox;

        box_shape(ref, b->x, b->y, b->w, b->h, b->label, b->extra, b->extra_c);
        g_nbox = nb;
    }
    for (k = 0; k < g_nbox; k++) {
        const PBox *b = &g_boxes[k];
        int top = (b->head ? HEAD : 3) + INSET;
        int bad = 0;
        int px;
        int py;

        for (py = 0; py < b->h; py++) {
            for (px = 0; px < b->w; px++) {
                size_t at = (size_t)(b->y + py) * PW + (size_t)(b->x + px);
                int safe = px >= 3 + INSET && px < b->w - 3 - INSET && py >= top &&
                           py < b->h - 3 - INSET &&
                           px + py >= CHAMF + 5 + INSET * 3 / 2 - 100 * !b->head &&
                           (b->w - 1 - px) + (b->h - 1 - py) >= CHAMF + 5 + INSET * 3 / 2;
                int edge = safe && (px == 3 + INSET || px == b->w - 4 - INSET ||
                                    py == top || py == b->h - 4 - INSET);

                if (!cham_in(px, py, b->w, b->h, CHAMF)) continue;
                if (out[at] != ref[at] && !safe) {
                    if (!bad) fprintf(stderr, "padcheck first %s at +%d,+%d\n", b->label ? b->label : "-", px, py);
                    out[at] = 0xFF00FFFFu;
                    bad++;
                } else if (edge && out[at] == ref[at]) {
                    out[at] = 0x00FFFFFFu;
                }
            }
        }
        fprintf(stderr, "padcheck %-10s %4d,%4d %3dx%3d bad=%d\n",
                b->label ? b->label : "-", b->x, b->y, b->w, b->h, bad);
    }
}

/* ASPECT: 1 = 4:3 screen (the 480 visible game columns fill FIELD_W x PH,
 * pixels 5:6 like a PC-98 monitor), 0 = 8:5 (square pixels: 2.25 x 2.25,
 * FIELD_W x 900, centred with black above and below). */
static int g_panels_43 = 1;

void panels_set_aspect(int a43)
{
    g_panels_43 = a43 ? 1 : 0;
}

void panels_render(uint32_t *out, const uint32_t *game, const RbPanelData *d)
{
    static int map_x[FIELD_W];
    static int map_y[PH];
    static int maps = -1;
    int x;
    int y;

    if (maps != g_panels_43) {
        int fh = g_panels_43 ? PH : 400 * FIELD_W / GAME_W;
        int top = (PH - fh) / 2;

        for (x = 0; x < FIELD_W; x++) {
            map_x[x] = GAME_X0 + x * GAME_W / FIELD_W;
        }
        for (y = 0; y < PH; y++) {
            map_y[y] = y < top || y >= top + fh ? -1 : (y - top) * 400 / fh;
        }
        maps = g_panels_43;
    }
    if (g_back_ok != g_have_title) {
        build_back();
    }
    memcpy(out, g_back, sizeof g_back);
    for (y = 0; y < PH; y++) {
        uint32_t *dst = out + y * PW + FIELD_X;

        if (map_y[y] < 0) {
            for (x = 0; x < FIELD_W; x++) {
                dst[x] = 0x000000FFu;
            }
            continue;
        }
        {
            const uint32_t *src = game + map_y[y] * 640;

            for (x = 0; x < FIELD_W; x++) {
                dst[x] = src[map_x[x]];
            }
        }
    }
    rect(out, FIELD_X - 3, 0, 3, PH, C_EDGE, 255);
    rect(out, FIELD_X + FIELD_W, 0, 3, PH, C_EDGE, 255);
    g_nbox = 0;
    g_frame = d->frame;
    draw_left(out, game, d);
    draw_right(out, d);
    if (((char *)0)) {
        pad_check(out);
    }
}

/* ---- TATE (vertical) layout ---------------------------------------- */
/* 1080x1920 portrait frame: a top marquee with the logo and SCORE /
 * LOADOUT / NO DEATH over SPEED, the playfield 1080x1080 in the middle
 * and a bottom bar with RANK, the small boxes, MEDALS and INPUT. The
 * boxes are the wide layout's ones, reduced to 84% with 3x3 area
 * sampling, keyed against the side art so only their own pixels land. */
#define TW RB_TATE_W
#define TH RB_TATE_H
#define T_TOP 487
#define T_GAME 1080
#define T_BOT (TH - T_TOP - T_GAME)
#define T_SC_NUM 84
#define T_SC_DEN 100

static int tsc(int v)
{
    return (v * T_SC_NUM + T_SC_DEN / 2) / T_SC_DEN;
}

static const PBox *find_box(const char *label)
{
    int k;

    for (k = 0; k < g_nbox; k++) {
        if (g_boxes[k].label && strcmp(g_boxes[k].label, label) == 0) {
            return &g_boxes[k];
        }
    }
    return NULL;
}

/* Side art for a TATE pixel: the same column tiling as build_back,
 * mirrored about the centre. */
static uint32_t tate_back(int x, int y)
{
    int xx = x < TW / 2 ? x : TW - 1 - x;

    if (!g_have_title) {
        return pack(C_BG);
    }
    {
        int sy = (y / 3) % 400;
        int u = xx + 48;
        int tile = u / 132;
        int tx = (u % 132) / 3;
        int sx = (tile & 1) ? 43 - tx : tx;
        Rgb c;
        Rgb b = C_BG;

        if (sx > 43) sx = 43;
        c = unpack(g_title[sy * 640 + sx]);
        b.r += (c.r - b.r) * 45 / 100;
        b.g += (c.g - b.g) * 45 / 100;
        b.b += (c.b - b.b) * 45 / 100;
        return pack(b);
    }
}

/* Copy wide rows [r0,r1) of box b (rows in skip[] left out) to dx,dy. */
static void tate_box(uint32_t *o, const uint32_t *wide, const PBox *b, int dx, int dy,
                     const int *cut, int ncut)
{
    int rows[1080];
    int nr = 0;
    int r;
    int dw;
    int dh;
    int i;
    int j;

    if (!b) {
        return;
    }
    for (r = 0; r < b->h; r++) {
        int k;
        int skip = 0;

        for (k = 0; k < ncut; k++) {
            if (r >= cut[2 * k] && r < cut[2 * k + 1]) skip = 1;
        }
        if (!skip) rows[nr++] = r;
    }
    dw = tsc(b->w);
    dh = tsc(nr);
    for (j = 0; j < dh; j++) {
        for (i = 0; i < dw; i++) {
            int sr = 0, sg = 0, sb = 0, n = 0;
            int a;
            int b2;

            for (b2 = 0; b2 < 3; b2++) {
                int ry = rows[((j * 3 + b2) * nr * 2 + nr) / (dh * 6)];
                int sy = b->y + ry;

                for (a = 0; a < 3; a++) {
                    int sx = b->x + ((i * 3 + a) * b->w * 2 + b->w) / (dw * 6);
                    uint32_t p = wide[sy * PW + sx];

                    if (p != g_back[sy * PW + sx]) {
                        Rgb c = unpack(p);

                        sr += c.r; sg += c.g; sb += c.b; n++;
                    }
                }
            }
            if (n) {
                Rgb c = {sr / n, sg / n, sb / n};

                {
                    int x = dx + i;
                    int y = dy + j;

                    if (x >= 0 && x < TW && y >= 0 && y < TH) {
                        Rgb u = unpack(o[y * TW + x]);
                        int al = n * 255 / 9;

                        u.r += (c.r - u.r) * al / 255;
                        u.g += (c.g - u.g) * al / 255;
                        u.b += (c.b - u.b) * al / 255;
                        o[y * TW + x] = pack(u);
                    }
                }
            }
        }
    }
}

/* Marquee art: the title's cut-out frame (lip rows 84-95, body rows
 * 104-244, the lip mirrored below), nearest-scaled to w x h. */
static void tate_logo(uint32_t *o, int x0, int y0, int w, int h)
{
    int i;
    int j;

    if (!g_have_title) {
        return; /* no title art yet: the marquee stays plain */
    }
    for (j = 0; j < h; j++) {
        int v = j * 163 / h;
        int sy = v < 12 ? 84 + v : v < 153 ? 104 + (v - 12) : 95 - (v - 153);

        for (i = 0; i < w; i++) {
            o[(y0 + j) * TW + x0 + i] = g_title[sy * 640 + i * 640 / w];
        }
    }
}

void panels_render_tate(uint32_t *out, const uint32_t *wide, const uint32_t *game)
{
    static int map_x[TW];
    static int map_y[T_GAME];
    static int maps = -1;
    static const int in_cut[] = {38, 58, 230, 248};
    const int X[3] = {25, 376, 727};
    int x;
    int y;
    const PBox *b;
    const PBox *nd;
    const PBox *md;

    if (maps != g_panels_43) {
        int fh = g_panels_43 ? T_GAME : 400 * TW / GAME_W;
        int top = (T_GAME - fh) / 2;

        for (x = 0; x < TW; x++) map_x[x] = GAME_X0 + x * GAME_W / TW;
        for (y = 0; y < T_GAME; y++) {
            map_y[y] = y < top || y >= top + fh ? -1 : (y - top) * 400 / fh;
        }
        maps = g_panels_43;
    }
    for (y = 0; y < TH; y++) {
        if (y >= T_TOP && y < T_TOP + T_GAME) {
            int my = map_y[y - T_TOP];
            uint32_t *dst = out + y * TW;

            for (x = 0; x < TW; x++) {
                dst[x] = my < 0 ? 0x000000FFu : game[my * 640 + map_x[x]];
            }
            continue;
        }
        for (x = 0; x < TW; x++) {
            out[y * TW + x] = tate_back(x, y >= T_TOP + T_GAME ? y - T_TOP - T_GAME : y);
        }
    }
    tate_logo(out, (TW - 916) / 2, 19, 916, 280);
    /* Top row y 318: SCORE, LOADOUT, NO DEATH over SPEED. */
    tate_box(out, wide, find_box("SCORE"), X[0], 318, NULL, 0);
    tate_box(out, wide, find_box("LOADOUT"), X[1], 318, NULL, 0);
    nd = find_box("NO DEATH");
    tate_box(out, wide, nd, X[2], 318, NULL, 0);
    tate_box(out, wide, find_box("SPEED"), X[2], nd ? 318 + tsc(nd->h) + 12 : 318, NULL, 0);
    /* Bottom bar: RANK, three rows of pairs, MEDALS over INPUT. */
    {
        int ch = tsc(392);
        int cy = T_TOP + T_GAME + (T_BOT - ch) / 2;
        int sw = tsc(190);
        int x1 = X[1] + tsc(388) - sw;
        int hs[3] = {tsc(120), tsc(64), tsc(120)};
        int g2 = (ch - hs[0] - hs[1] - hs[2]) / 2;
        int ih;

        tate_box(out, wide, find_box("RANK"), X[0], cy, NULL, 0);
        tate_box(out, wide, find_box("HIT RATIO"), X[1], cy, NULL, 0);
        tate_box(out, wide, find_box("HIT COUNT"), x1, cy, NULL, 0);
        tate_box(out, wide, find_box("LEVEL"), X[1], cy + hs[0] + g2, NULL, 0);
        tate_box(out, wide, find_box("TIME"), x1, cy + hs[0] + g2, NULL, 0);
        tate_box(out, wide, find_box("EXTENDS"), X[1], cy + ch - hs[2], NULL, 0);
        tate_box(out, wide, find_box("I-FRAMES"), x1, cy + ch - hs[2], NULL, 0);
        md = find_box("MEDALS");
        b = find_box("INPUT");
        ih = b ? tsc(b->h - (in_cut[1] - in_cut[0]) - (in_cut[3] - in_cut[2])) : 0;
        tate_box(out, wide, md, X[2], cy, NULL, 0);
        tate_box(out, wide, b, X[2], cy + ch - ih, in_cut, 2);
    }
}

/* ---- universal window size ----------------------------------------- */

static uint32_t *ref_buf(int w, int h, uint32_t **slot, int *sw, int *sh)
{
    if (*sw != w || *sh != h || !*slot) {
        free(*slot);
        *slot = (uint32_t *)calloc((size_t)w * (size_t)h, sizeof(uint32_t));
        *sw = *slot ? w : 0;
        *sh = *slot ? h : 0;
    }
    return *slot;
}

static void blit_ink(uint32_t *out, int ow, int oh, const uint32_t *ref, int rw,
                     const uint32_t *bg, int sx, int sy, int sw, int sh, int dx, int dy,
                     int dw, int dh)
{
    int j;
    int i;

    if (dw < 1) {
        dw = 1;
    }
    if (dh < 1) {
        dh = 1;
    }
    for (j = 0; j < dh; j++) {
        int ry = sy + (sh > 0 ? j * sh / dh : 0);
        int y = dy + j;

        if (ry < 0 || y < 0 || y >= oh) {
            continue;
        }
        for (i = 0; i < dw; i++) {
            int rx = sx + (sw > 0 ? i * sw / dw : 0);
            int x = dx + i;
            uint32_t p;

            if (rx < 0 || x < 0 || x >= ow) {
                continue;
            }
            p = ref[(size_t)ry * rw + rx];
            if (!bg || p != bg[(size_t)ry * rw + rx]) {
                out[(size_t)y * ow + x] = p;
            }
        }
    }
}

static int box_less(const void *a, const void *b)
{
    const PBox *pa = a;
    const PBox *pb = b;

    return pa->y - pb->y;
}

/* Lay the reference column's boxes into the side strip, same
 * proportions, leftover space shared by the gaps. */
static int place_column(uint32_t *out, int ow, int oh, int side_x, int side_w,
                        PBox *boxes, int n, int col0, const uint32_t *ref,
                        const uint32_t *bg, RbPanelGeom *geom)
{
    PBox col[32];
    int nc = 0;
    int i;
    int gaps[33];
    int ng;
    int scaled_h = 0;
    int scaled_g = 0;
    int left;
    int extra;
    int y;
    int gi;
    double s;
    int col_pix;
    int origin;

    for (i = 0; i < n && nc < 32; i++) {
        int cx = boxes[i].x + boxes[i].w / 2;

        if (cx >= col0 && cx < col0 + 420) {
            col[nc++] = boxes[i];
        }
    }
    if (!nc || side_w < 8) {
        return 0;
    }
    qsort(col, (size_t)nc, sizeof col[0], box_less);
    s = (double)side_w / 420.0;
    if (s > (double)oh / 1080.0) {
        s = (double)oh / 1080.0;
    }
    if (s < 0.05) {
        s = 0.05;
    }
    gaps[0] = col[0].y;
    for (i = 0; i < nc; i++) {
        if (i + 1 < nc) {
            int g = col[i + 1].y - (col[i].y + col[i].h);

            gaps[i + 1] = g > 0 ? g : 0;
        }
    }
    gaps[nc] = 1080 - (col[nc - 1].y + col[nc - 1].h);
    if (gaps[nc] < 0) {
        gaps[nc] = 0;
    }
    ng = nc + 1;
    for (i = 0; i < nc; i++) {
        scaled_h += (int)(col[i].h * s + 0.5);
    }
    for (i = 0; i < ng; i++) {
        scaled_g += (int)(gaps[i] * s + 0.5);
    }
    left = oh - scaled_h - scaled_g;
    if (left < 0) {
        left = 0;
    }
    extra = left / ng;
    col_pix = (int)(420.0 * s + 0.5);
    if (col_pix > side_w) {
        col_pix = side_w;
    }
    origin = side_x + (side_w - col_pix) / 2;
    y = (int)(gaps[0] * s + 0.5) + extra;
    gi = 1;
    for (i = 0; i < nc; i++) {
        int dw = (int)(col[i].w * s + 0.5);
        int dh = (int)(col[i].h * s + 0.5);
        int dx = origin + (int)((col[i].x - col0) * s + 0.5);

        if (dw < 1) {
            dw = 1;
        }
        if (dh < 1) {
            dh = 1;
        }
        if (dx < side_x) {
            dx = side_x;
        }
        if (dx + dw > side_x + side_w) {
            dw = side_x + side_w - dx;
        }
        if (y < 0) {
            y = 0;
        }
        if (y + dh > oh) {
            dh = oh - y;
        }
        if (dw > 0 && dh > 0) {
            blit_ink(out, ow, oh, ref, PW, bg, col[i].x, col[i].y, col[i].w, col[i].h, dx, y,
                     dw, dh);
            if (geom && geom->n < 40) {
                geom->x[geom->n] = dx;
                geom->y[geom->n] = y;
                geom->w[geom->n] = dw;
                geom->h[geom->n] = dh;
                geom->n++;
            }
        }
        y += dh;
        if (gi < ng) {
            y += (int)(gaps[gi] * s + 0.5) + extra;
            gi++;
        }
    }
    return nc;
}

static void fit_field(int w, int h, int *fx, int *fy, int *fw, int *fh)
{
    int aw = g_panels_43 ? 4 : 6;
    int ah = g_panels_43 ? 3 : 5;

    if ((long)w * ah > (long)h * aw) {
        *fh = h;
        *fw = (int)((long)h * aw / ah);
    } else {
        *fw = w;
        *fh = (int)((long)w * ah / aw);
    }
    if (*fw < 1) {
        *fw = 1;
    }
    if (*fh < 1) {
        *fh = 1;
    }
    if (*fw > w) {
        *fw = w;
    }
    if (*fh > h) {
        *fh = h;
    }
    *fx = (w - *fw) / 2;
    *fy = (h - *fh) / 2;
}

static void blit_game(uint32_t *out, int ow, int fx, int fy, int fw, int fh,
                      const uint32_t *game)
{
    int y;
    int x;

    for (y = 0; y < fh; y++) {
        int sy = y * 400 / fh;
        uint32_t *dst = out + (size_t)(fy + y) * ow + fx;
        const uint32_t *src = game + (size_t)sy * 640;

        for (x = 0; x < fw; x++) {
            dst[x] = src[80 + x * 480 / fw];
        }
    }
}

void panels_compose(uint32_t *out, int w, int h, int tate, const uint32_t *game,
                    const RbPanelData *d, RbPanelGeom *geom)
{
    static uint32_t *ref;
    static int ref_w, ref_h;
    static uint32_t *ref_t;
    static int rt_w, rt_h;
    int fx, fy, fw, fh;
    int i;
    int wide;
    RbPanelData dummy;

    if (!out || w <= 0 || h <= 0) {
        return;
    }
    if (!d) {
        memset(&dummy, 0, sizeof dummy);
        dummy.weapon = 'V';
        dummy.weapon_lv = 1;
        dummy.shot_lv = 1;
        dummy.lives = 3;
        dummy.medal_spots = 0;
        d = &dummy;
    }
    if (!game) {
        return;
    }
    if (geom) {
        memset(geom, 0, sizeof *geom);
    }
    tate = tate ? 1 : 0;
    fit_field(w, h, &fx, &fy, &fw, &fh);
    wide = !tate && (long)w * 10 >= (long)h * 16 && fx >= 32;
    if (g_back_ok != g_have_title) {
        build_back();
    }
    for (i = 0; i < w * h; i++) {
        int x = i % w;
        int y = i / w;

        out[i] = panels_plate_at(x, y);
    }
    blit_game(out, w, fx, fy, fw, fh, game);
    if (geom) {
        geom->field_x = fx;
        geom->field_y = fy;
        geom->field_w = fw;
        geom->field_h = fh;
        geom->widgets = 0;
    }
    /* Narrower than 16:10 (4:3, 5:4): the bars stay the plate alone. */
    if (!wide && !tate) {
        return;
    }
    if (!ref_buf(PW, PH, &ref, &ref_w, &ref_h)) {
        return;
    }
    panels_render(ref, game, d);
    if (tate) {
        int top = fy;
        int bot = h - (fy + fh);
        int y0 = T_TOP + T_GAME;

        if (!ref_buf(TW, TH, &ref_t, &rt_w, &rt_h)) {
            return;
        }
        panels_render_tate(ref_t, ref, game);
        if (top > 0) {
            blit_ink(out, w, h, ref_t, TW, NULL, 0, 0, TW, T_TOP, 0, 0, w, top);
        }
        if (bot > 0) {
            blit_ink(out, w, h, ref_t, TW, NULL, 0, y0, TW, TH - y0, 0, fy + fh, w, bot);
        }
        /* The plate under the copy was scaled with the widgets. Paint
         * the plate back, then the ink only, so the metal stays 1:1. */
        for (i = 0; i < w * h; i++) {
            int x = i % w;
            int y = i / w;

            if (x >= fx && x < fx + fw && y >= fy && y < fy + fh) {
                continue;
            }
            out[i] = panels_plate_at(x, y);
        }
        if (top > 0) {
            blit_ink(out, w, h, ref_t, TW, NULL, 0, 0, TW, T_TOP, 0, 0, w, top);
        }
        /* Second pass kept the plate pixels too. Re-copy ink by
         * comparing with tate_back. */
        for (i = 0; i < w * top; i++) {
            int x = i % w;
            int y = i / w;
            int rx = x * TW / w;
            int ry = y * T_TOP / (top > 0 ? top : 1);
            uint32_t p = ref_t[(size_t)ry * TW + rx];

            if (p != tate_back(rx, ry)) {
                out[(size_t)y * w + x] = p;
            }
        }
        if (bot > 0) {
            for (i = 0; i < w * bot; i++) {
                int x = i % w;
                int y = i / w;
                int rx = x * TW / w;
                int ry = y0 + y * (TH - y0) / bot;
                uint32_t p = ref_t[(size_t)ry * TW + rx];

                if (p != tate_back(rx, ry)) {
                    out[(size_t)(fy + fh + y) * w + x] = p;
                }
            }
        }
        if (geom) {
            geom->widgets = 1;
            if (top > 0 && geom->n < 40) {
                geom->x[geom->n] = 0;
                geom->y[geom->n] = 0;
                geom->w[geom->n] = w;
                geom->h[geom->n] = top;
                geom->n++;
            }
            if (bot > 0 && geom->n < 40) {
                geom->x[geom->n] = 0;
                geom->y[geom->n] = fy + fh;
                geom->w[geom->n] = w;
                geom->h[geom->n] = bot;
                geom->n++;
            }
        }
        return;
    }
    {
        PBox boxes[32];
        int n = g_nbox < 32 ? g_nbox : 32;
        PBox logo;

        memcpy(boxes, g_boxes, (size_t)n * sizeof boxes[0]);
        logo.x = 16;
        logo.y = 950;
        logo.w = 388;
        logo.h = 120;
        logo.label = "LOGO";
        if (n < 32) {
            boxes[n++] = logo;
        }
        if (geom) {
            geom->widgets = 1;
        }
        place_column(out, w, h, 0, fx, boxes, n, 0, ref, g_back, geom);
        place_column(out, w, h, fx + fw, w - (fx + fw), boxes, n, 1500, ref, g_back, geom);
    }
}
