/**
 * Software GVRAM. Plane order follows the tile blitter at 0x4B21
 * (A800, B000, then the green plane, intensity cleared) and the sprite
 * blitter at 0x481B (mask, then blue, red, green through GRCG).
 */
#include "gfx.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAP_COLS 30
#define MAP_ROWS 25
#define MAP_N    (MAP_COLS * MAP_ROWS)
/* A replaced 16-line row at intro speed 15 sits up to 29 lines before
 * the GDC start address. Those lines are the bottom of the picture. */
#define SCROLL_TAIL 32

static int g_title_frame;
/* Enemy bullet colour: 0 original, then PINK, LILAC, HOT PINK. Row 0
 * replaces index 11 (the bright red), row 1 index 8 (the dark red). */
static int g_bullet;
static int g_bulletlog = -1;
static unsigned g_bl_frames;
static struct { uint16_t type, seg, pat; int tint; unsigned n; } g_bl[64];
static int g_bl_n;

static void bulletlog_note(uint16_t type, uint16_t seg, uint16_t pat, int tint)
{
    int i;

    for (i = 0; i < g_bl_n; i++) {
        if (g_bl[i].type == type && g_bl[i].seg == seg && g_bl[i].pat == pat) {
            g_bl[i].n++;
            return;
        }
    }
    if (g_bl_n < 64) {
        g_bl[g_bl_n].type = type;
        g_bl[g_bl_n].seg = seg;
        g_bl[g_bl_n].pat = pat;
        g_bl[g_bl_n].tint = tint;
        g_bl[g_bl_n].n = 1;
        g_bl_n++;
    }
}

static void bulletlog_frame(void)
{
    int i;

    if (++g_bl_frames % 600u != 0 || !g_bl_n) {
        return;
    }
    for (i = 0; i < g_bl_n; i++) {
        fprintf(stderr, "bullet f=%u type=%04X seg=%u pat=%u %s n=%u\n", g_bl_frames,
                g_bl[i].type, g_bl[i].seg, g_bl[i].pat,
                g_bl[i].tint ? "solid->tint" : "shootable", g_bl[i].n);
    }
    g_bl_n = 0;
}

struct RbGfx {
    uint8_t *pix;
    /* Background page. Fills, tiles, and GRCG blits land here. Each
     * entity draw copies this over pix and stamps sprites only onto pix,
     * which is what the PC-98 does by rebuilding the hidden page (GRCG
     * tile restore plus the 0xA4/0xA6 page flip) before the sprites. */
    uint8_t *bg;
    /* Previous pixels of a scroll row that was just replaced. Present
     * keeps them on the lines the second GDC partition still shows. */
    uint8_t *hold;
    uint8_t hold_on[MAP_ROWS];
    int paint_bg;
    uint16_t map[MAP_N];
    const uint8_t *bank[256];
    uint32_t blen[256];
    int16_t pal[16][3];
    int16_t dpal[16][3];
    uint8_t glyph[512][32];
    uint8_t glyph_on[512];
    int active;
    uint8_t tint[RB_GFX_W * RB_GFX_H]; /* 1: enemy bullet pixel in a shot colour */
    int tint_cur;          /* sprite being stamped: 1 enemy bullet, 2 player shot */
    int tint_gold;         /* orange/gold pixels in the bullet being drawn */
    int tint_red;          /* red pixels (8, 9, 11) in it */
    int tint_n;            /* pixels marked for the bullet being drawn */
    uint32_t tint_at[1024];
};

RbGfx *gfx_create(void)
{
    RbGfx *gfx = (RbGfx *)calloc(1, sizeof(*gfx));

    if (!gfx) {
        return NULL;
    }
    gfx->pix = (uint8_t *)calloc((size_t)RB_GFX_W * (size_t)RB_GFX_H, 1);
    gfx->bg = (uint8_t *)calloc((size_t)RB_GFX_W * (size_t)RB_GFX_H, 1);
    gfx->hold = (uint8_t *)calloc((size_t)RB_GFX_W * (size_t)RB_GFX_H, 1);
    if (!gfx->pix || !gfx->bg || !gfx->hold) {
        free(gfx->pix);
        free(gfx->bg);
        free(gfx->hold);
        free(gfx);
        return NULL;
    }
    gfx->paint_bg = 1;
    return gfx;
}

void gfx_destroy(RbGfx *gfx)
{
    if (!gfx) {
        return;
    }
    free(gfx->pix);
    free(gfx->bg);
    free(gfx->hold);
    free(gfx);
}

void gfx_reset(RbGfx *gfx)
{
    if (!gfx) {
        return;
    }
    memset(gfx->pix, 0, (size_t)RB_GFX_W * (size_t)RB_GFX_H);
    memset(gfx->bg, 0, (size_t)RB_GFX_W * (size_t)RB_GFX_H);
    if (gfx->hold) {
        memset(gfx->hold, 0, (size_t)RB_GFX_W * (size_t)RB_GFX_H);
    }
    memset(gfx->hold_on, 0, sizeof gfx->hold_on);
    gfx->paint_bg = 1;
    memset(gfx->map, 0, sizeof gfx->map);
    memset(gfx->bank, 0, sizeof gfx->bank);
    memset(gfx->blen, 0, sizeof gfx->blen);
    memset(gfx->pal, 0, sizeof gfx->pal);
    memset(gfx->dpal, 0, sizeof gfx->dpal);
    memset(gfx->glyph, 0, sizeof gfx->glyph);
    memset(gfx->glyph_on, 0, sizeof gfx->glyph_on);
    gfx->active = 0;
}

void gfx_bind(RbGfx *gfx, uint8_t slot, const uint8_t *data, uint32_t len)
{
    if (!gfx) {
        return;
    }
    gfx->bank[slot] = data;
    gfx->blen[slot] = len;
    gfx->active = 1;
}

static size_t pix_at(int x, int y)
{
    return (size_t)y * (size_t)RB_GFX_W + (size_t)x;
}

static void put_idx(RbGfx *gfx, int x, int y, uint8_t idx)
{
    size_t at;

    if ((unsigned)x >= (unsigned)RB_GFX_W || (unsigned)y >= (unsigned)RB_GFX_H) {
        return;
    }
    at = pix_at(x, y);
    gfx->pix[at] = idx;
    gfx->tint[at] = 0;
    if (gfx->tint_cur == 1) {
        /* Every pixel of a bullet that cannot be shot is recoloured,
         * however large (boss lasers run the full screen height); the
         * shade is picked from its own brightness when shown. */
        gfx->tint[at] = 4;
        if (gfx->tint_n < 1024) {
            gfx->tint_at[gfx->tint_n] = (uint32_t)at;
        }
        gfx->tint_n++;
        gfx->tint_red++;
    }
    if (gfx->paint_bg) {
        gfx->bg[at] = idx;
    }
}

/* GRCG read-modify-write of one pixel. `planes` is the index bits. */
static void rmw_idx(RbGfx *gfx, int x, int y, uint8_t planes, int set)
{
    size_t at;
    uint8_t v;

    if ((unsigned)x >= (unsigned)RB_GFX_W || (unsigned)y >= (unsigned)RB_GFX_H) {
        return;
    }
    at = pix_at(x, y);
    v = gfx->paint_bg ? gfx->bg[at] : gfx->pix[at];
    if (set) {
        v = (uint8_t)(v | planes);
    } else {
        v = (uint8_t)(v & (uint8_t)~planes);
    }
    gfx->pix[at] = v;
    if (gfx->paint_bg) {
        gfx->bg[at] = v;
    }
}

/* Scanned page is 400*80 = 0x7D00 bytes. 0x7D00..0xFFFF is the gap and
 * the rest of the segment; the GDC does not scan it. A 16-bit wrap that
 * lands back under 0x7D00 is the top of the page. Folding the gap with
 * `% 32000` paints it on the seam, which is the stray strip. */
static int plane_xy(uint32_t addr, int *x, int *y)
{
    uint16_t at = (uint16_t)addr;

    if (at >= 0x7D00u) {
        return 0;
    }
    *x = (int)(at % 80u) * 8;
    *y = (int)(at / 80u);
    return 1;
}

/* GRCG fill at loader 0x77D4. The color byte is inverted, then each
 * low bit selects a tile register of 0x00 or 0xFF. */
void gfx_fill(RbGfx *gfx, uint16_t di, uint16_t nbytes, uint16_t rows, uint8_t color)
{
    uint8_t ah;
    uint8_t idx;
    uint16_t row;
    int i;

    if (!gfx || nbytes == 0 || rows == 0) {
        return;
    }
    ah = (uint8_t)~color;
    idx = 0;
    for (i = 0; i < 4; i++) {
        if ((ah & 1u) == 0u) {
            idx = (uint8_t)(idx | (uint8_t)(1u << i));
        }
        ah = (uint8_t)(ah >> 1);
    }
    for (row = 0; row < rows; row++) {
        uint16_t b;

        for (b = 0; b < nbytes; b++) {
            int x;
            int y;
            int p;

            /* 16-bit DI, same as the GRCG fill. Bytes in the gap stay
             * off the scanned page instead of wrapping onto line 0. */
            if (!plane_xy((uint32_t)di + (uint32_t)row * 80u + b, &x, &y)) {
                continue;
            }
            for (p = 0; p < 8; p++) {
                put_idx(gfx, x + p, y, idx);
            }
        }
    }
    gfx->active = 1;
}

void gfx_copy_row(RbGfx *gfx, uint16_t screen_row, uint16_t src_row)
{
    const uint8_t *map;
    uint32_t src;
    int col;

    if (!gfx || screen_row >= MAP_ROWS) {
        return;
    }
    map = gfx->bank[0x16];
    if (!map) {
        return;
    }
    src = (uint32_t)src_row * 0x3Cu;
    if (src + 0x3Cu > gfx->blen[0x16]) {
        return;
    }
    /* 0x108E ORs the new cell into the slot selected by [bp+0x76].
     * That slot is the one just before the scan start, so partition 2
     * puts it at the bottom of the screen. The incoming cell belongs
     * at the top once the start address reaches it. Until then the
     * bottom has to keep scrolling the cell that was already there.
     * The first fill of an empty ring has nothing to keep. */
    if (gfx->hold) {
        int live = 0;

        for (col = 0; col < MAP_COLS; col++) {
            if (gfx->map[(size_t)screen_row * MAP_COLS + (size_t)col] != 0) {
                live = 1;
                break;
            }
        }
        if (live) {
            size_t off = (size_t)screen_row * 16u * (size_t)RB_GFX_W;

            memcpy(gfx->hold + off, gfx->bg + off, 16u * (size_t)RB_GFX_W);
            gfx->hold_on[screen_row] = 1;
        }
    }
    for (col = 0; col < MAP_COLS; col++) {
        uint16_t w = (uint16_t)map[src] | ((uint16_t)map[src + 1] << 8);

        gfx->map[(size_t)screen_row * MAP_COLS + (size_t)col] =
            (uint16_t)(w | 0x8000u);
        src += 2u;
    }
}

static void blit_tile(RbGfx *gfx, const uint8_t *src, int x, int y)
{
    int row;

    for (row = 0; row < 16; row++) {
        int plane;
        uint8_t bits[3][16];
        int col;

        for (plane = 0; plane < 3; plane++) {
            const uint8_t *p = src + plane * 32 + row * 2;
            int i;

            for (i = 0; i < 8; i++) {
                bits[plane][i] = (uint8_t)((p[0] >> (7 - i)) & 1u);
                bits[plane][8 + i] = (uint8_t)((p[1] >> (7 - i)) & 1u);
            }
        }
        for (col = 0; col < 16; col++) {
            uint8_t idx = 0;

            if (bits[0][col]) {
                idx |= 1u;
            }
            if (bits[1][col]) {
                idx |= 2u;
            }
            if (bits[2][col]) {
                idx |= 4u;
            }
            /* 0x4B21 steps 16 lines and does not subtract 0x7D00.
             * A row past the page is the gap. put_idx drops it. */
            put_idx(gfx, x + col, y + row, idx);
        }
    }
}

void gfx_compose(RbGfx *gfx)
{
    const uint8_t *tiles;
    int row;
    int col;

    if (!gfx) {
        return;
    }
    tiles = gfx->bank[0x26];
    if (!tiles) {
        return;
    }
    for (row = 0; row < MAP_ROWS; row++) {
        for (col = 0; col < MAP_COLS; col++) {
            uint16_t w = gfx->map[(size_t)row * MAP_COLS + (size_t)col];
            uint16_t index;
            uint32_t off;

            if ((w & 0xC000u) == 0u) {
                continue;
            }
            index = (uint16_t)(w & 0x1FFu);
            off = (uint32_t)index * 96u;
            if (off + 96u <= gfx->blen[0x26]) {
                blit_tile(gfx, tiles + off, 80 + col * 16, row * 16);
            }
            w = (uint16_t)((w & 0x3FFFu) | ((w & 0x8000u) >> 1));
            gfx->map[(size_t)row * MAP_COLS + (size_t)col] = w;
        }
    }
    gfx->active = 1;
}

static void decode_plane(const uint8_t *p, int shift, uint8_t out[24])
{
    int i;

    memset(out, 0, 24);
    for (i = 0; i < 8; i++) {
        if ((p[0] >> (7 - i)) & 1u) {
            if (i + shift < 24) {
                out[i + shift] = 1;
            }
        }
        if ((p[1] >> (7 - i)) & 1u) {
            if (8 + i + shift < 24) {
                out[8 + i + shift] = 1;
            }
        }
    }
}

static void mark_dirty(RbGfx *gfx, int x, int y, int w)
{
    int x0 = x - 80;
    int x1 = x + w - 1 - 80;
    int y0 = y;
    int y1 = y + 15;
    int c0;
    int c1;
    int r0;
    int r1;
    int r;
    int c;

    if (x0 < 0) {
        x0 = 0;
    }
    c0 = x0 / 16;
    c1 = x1 / 16;
    r0 = y0 / 16;
    r1 = y1 / 16;
    for (r = r0; r <= r1; r++) {
        int rr = r;

        if (rr < 0 || rr >= MAP_ROWS) {
            continue;
        }
        for (c = c0; c <= c1; c++) {
            if (c < 0 || c >= MAP_COLS) {
                continue;
            }
            gfx->map[(size_t)rr * MAP_COLS + (size_t)c] |= 0x8000u;
        }
    }
}

/* Pattern slots are 128 bytes. The CH==2 blitter reads only the first 32. */
static uint32_t pattern_off(uint8_t index)
{
    return (uint32_t)index * 128u;
}

static void blit_sprite(RbGfx *gfx, const uint8_t *src, uint32_t len,
                        int x, int y, int shift, uint8_t attr)
{
    int mono = (attr == 2);
    int bright = (attr & 1u) == 0u;
    int wide = 16 + (shift ? 8 : 0);
    int row;
    int ring = 0;
    uint32_t need = mono ? 32u : 128u;

    if (!src || len < need) {
        return;
    }
    if (shift < 0) {
        shift = 0;
    }
    if (shift > 7) {
        shift = 7;
    }
    /* 0x11D9 subtracts 0x7D00 once when the cell origin is past the
     * scanned page. 0x483B then takes the ring (0x49C5) only when that
     * origin is still past line 384. A cell at line 384 or earlier
     * writes 16 scanlines with no per-line wrap; those rows fit, and
     * wrapping a gap address back to y=0 is the stray strip. */
    {
        int bx = (x >= 0) ? (x >> 3) : 0;
        int di;

        if (y >= 0) {
            di = y * 80 + bx;
            if (di >= 0x7D00) {
                y -= 400;
                di -= 0x7D00;
            }
            ring = di > 0x7800;
        }
    }
    for (row = 0; row < 16; row++) {
        uint8_t mask[24];
        uint8_t b[24];
        uint8_t r[24];
        uint8_t g[24];
        int col;
        int yy = y + row;

        decode_plane(src + row * 2, shift, mask);
        if (!mono) {
            decode_plane(src + 32 + row * 2, shift, b);
            decode_plane(src + 64 + row * 2, shift, r);
            decode_plane(src + 96 + row * 2, shift, g);
        }
        /* Negative lines are not in the ring. On the ring path, one
         * subtract of 400 matches 0x49C5's single SUB 0x7D00. A line
         * that is still past the page is the gap and is not drawn. */
        if (yy < 0) {
            continue;
        }
        if (yy >= RB_GFX_H) {
            if (!ring) {
                continue;
            }
            yy -= RB_GFX_H;
            if (yy < 0 || yy >= RB_GFX_H) {
                continue;
            }
        }
        for (col = 0; col < wide; col++) {
            uint8_t idx;

            if (!mask[col]) {
                continue;
            }
            if (mono) {
                idx = 11;
            } else {
                idx = 0;
                if (b[col]) {
                    idx |= 1u;
                }
                if (r[col]) {
                    idx |= 2u;
                }
                if (g[col]) {
                    idx |= 4u;
                }
                if (bright) {
                    idx |= 8u;
                }
            }
            put_idx(gfx, x + col, yy, idx);
        }
    }
    if (y >= 0) {
        mark_dirty(gfx, x, y >= RB_GFX_H ? y - RB_GFX_H : y, wide);
    }
}

static int wrap_line(int v)
{
    while (v < 0) {
        v += RB_GFX_H;
    }
    while (v >= RB_GFX_H) {
        v -= RB_GFX_H;
    }
    return v;
}

/* Entity sprites are 16x16 cells. The PC-98 blitter at 0x1132 drops a
 * whole cell when its world row is outside 0..23 or its column is
 * outside 4..34, so a large sprite steps onto the screen in bands.
 * A cell it keeps is written in full at the GVRAM address 0x1132
 * computes (world Y plus [bp+0x76], wrapped into the page); there is
 * no per-line clip. Which lines the GDC then shows is decided by the
 * SAD at present time, not here.
 *
 * pr9h: the stage path used to clip each line against a guessed
 * screen line (world - speed + 1). By the time present runs, [bp+0x76]
 * has already moved by the scroll speed, so on the fast stage 5 scroll
 * (13..15 lines a frame) that guess was off by the whole speed. Lines
 * of a boss rising from below the playfield were kept, landed on GVRAM
 * lines the next SAD shows at the top, and painted over the HUD band.
 * The cell rule below is the original's, and it never lets a cell
 * outside rows 0..23 reach the page. The title (no scroll) keeps the
 * per-line path it always had. */
static void blit_sprite_smooth(RbGfx *gfx, const uint8_t *src, uint32_t len,
                               int px, int shift, uint8_t attr, int world_y,
                               int scroll76, int speed, int blank_sides)
{
    /* attr 2 is the one-frame hit flash. The score native stores 2 in
     * the draw byte when HP changes, and the next call replaces it.
     * The mono plane is painted with palette index 11. */
    int mono = (attr == 2);
    int bright = (attr & 1u) == 0u;
    int wide = 16 + (shift ? 8 : 0);
    int row;
    uint32_t need = mono ? 32u : 128u;
    int x0 = blank_sides ? 80 : 0;
    int x1 = blank_sides ? 560 : RB_GFX_W;
    int marked_low = 0;
    int marked_high = 0;

    if (!src || len < need) {
        return;
    }
    if (shift < 0) {
        shift = 0;
    }
    if (shift > 7) {
        shift = 7;
    }
    for (row = 0; row < 16; row++) {
        uint8_t mask[24];
        uint8_t b[24];
        uint8_t r[24];
        uint8_t g[24];
        int col;
        int world = world_y + row;
        int screen = blank_sides ? (world + 1) : world;
        int gv;
        int drew = 0;

        /* pr9k: clip per line exactly at the visible window, Y signed,
         * never wrapped. Entities are drawn with [bp+0x76] before the
         * frame's scroll step; present then runs with 0x76 lowered by
         * [bp+0x84] and SAD = 0x76 + 0x84 - 1, i.e. the draw-time 0x76
         * minus 1 (measured: delta is -speed on every stage frame). So
         * the line present shows is world + 1 for every sprite, whatever
         * the speed. pr9g used world - speed + 1 (off by the speed: the
         * boss wrapped and lines were cut at the bottom); pr9j added a
         * world < 384 cut that also ate the bottom of the screen. A line
         * at screen >= 400 would wrap into the top and is dropped. The
         * HUD text is painted over the sprites in present. */
        (void)speed;
        if (screen < 0 || screen >= RB_GFX_H) {
            continue;
        }
        gv = wrap_line(world + scroll76);
        decode_plane(src + row * 2, shift, mask);
        if (!mono) {
            decode_plane(src + 32 + row * 2, shift, b);
            decode_plane(src + 64 + row * 2, shift, r);
            decode_plane(src + 96 + row * 2, shift, g);
        }
        for (col = 0; col < wide; col++) {
            int sx = px + col;
            uint8_t idx;

            if (sx < x0 || sx >= x1 || !mask[col]) {
                continue;
            }
            if (mono) {
                idx = 11;
            } else {
                idx = 0;
                if (b[col]) {
                    idx |= 1u;
                }
                if (r[col]) {
                    idx |= 2u;
                }
                if (g[col]) {
                    idx |= 4u;
                }
                if (bright) {
                    idx |= 8u;
                }
            }
            put_idx(gfx, sx, gv, idx);
            drew = 1;
        }
        if (!drew) {
            continue;
        }
        if (gv >= 384) {
            if (!marked_high) {
                mark_dirty(gfx, px, gv, wide);
                marked_high = 1;
            }
        } else if (!marked_low) {
            mark_dirty(gfx, px, gv, wide);
            marked_low = 1;
        }
    }
}

void gfx_draw_entities(RbGfx *gfx, const uint8_t *ent, int ent_n, int stride,
                       const uint8_t *const *chunk, const uint32_t *clen,
                       const uint8_t *cof, int scroll76, int scroll_speed,
                       int blank_sides)
{
    int i;

    if (!gfx || !ent || !chunk || !clen || !cof) {
        return;
    }
    /* Rebuild the shown page from the sprite-free background, then stamp
     * this frame's sprites. Scroll lives in the GVRAM address, so a sprite
     * that holds still on screen still moves through the buffer. */
    memcpy(gfx->pix, gfx->bg, (size_t)RB_GFX_W * (size_t)RB_GFX_H);
    memset(gfx->tint, 0, sizeof gfx->tint);
    gfx->paint_bg = 0;
    if (g_bulletlog < 0) {
        g_bulletlog = getenv("RUDEBR_BULLETLOG") ? atoi(getenv("RUDEBR_BULLETLOG")) : 0;
    }
    if (g_bulletlog) {
        bulletlog_frame();
    }
    for (i = 0; i < ent_n; i++) {
        const uint8_t *slot = ent + (size_t)i * (size_t)stride;
        uint16_t type = (uint16_t)slot[0] | ((uint16_t)slot[1] << 8);
        int16_t x;
        int16_t y;
        int wcells;
        int hcells;
        uint16_t pat;
        uint16_t seg;
        uint8_t shift;
        uint8_t attr;
        const uint8_t *script;
        uint32_t slen;
        const uint8_t *sheet;
        int row;

        /* Opcode 0x4E (0x420E): the low byte of the type must be nonzero,
         * and bit 0x200 hides the sprite. Options set that bit while they
         * are parked (mode byte off, and for the whole respawn after a
         * death) so the original does not draw them. */
        if ((type & 0x00FFu) == 0 || (type & 0x0200u) != 0) {
            continue;
        }
        wcells = slot[0x4C];
        hcells = slot[0x4D];
        if (wcells == 0 || hcells == 0) {
            continue;
        }
        x = (int16_t)((uint16_t)slot[0x22] | ((uint16_t)slot[0x23] << 8));
        y = (int16_t)((uint16_t)slot[0x24] | ((uint16_t)slot[0x25] << 8));
        x = (int16_t)(x - (int16_t)(wcells << 3));
        y = (int16_t)(y - (int16_t)(hcells << 3));
        pat = (uint16_t)slot[0x38] | ((uint16_t)slot[0x39] << 8);
        seg = (uint16_t)slot[0x3A] | ((uint16_t)slot[0x3B] << 8);
        shift = (uint8_t)((uint8_t)x & slot[0x4E]);
        attr = slot[0x4F];
        if (seg > 255u || !gfx->bank[seg]) {
            continue;
        }
        if (cof[i] >= 40u) {
            continue;
        }
        /* Enemy bullets (0x0080) that cannot be shot (no 0x0020 or
         * 0x0040 bit, which the player's shots test). */
        gfx->tint_cur = g_bullet && (type & 0x0080u) && !(type & 0x0060u) ? 1 : 0;
        gfx->tint_gold = 0;
        gfx->tint_red = 0;
        gfx->tint_n = 0;
        sheet = gfx->bank[seg];
        script = chunk[cof[i]];
        slen = clen[cof[i]];
        if (!script) {
            continue;
        }
        for (row = 0; row < hcells; row++) {
            int col;

            for (col = 0; col < wcells; col++) {
                unsigned poff = (unsigned)pat + (unsigned)row * (unsigned)wcells +
                                (unsigned)col;
                uint8_t index;
                uint32_t off;
                int px;

                if (poff >= slen) {
                    continue;
                }
                index = script[poff];
                off = pattern_off(index);
                if (off >= gfx->blen[seg]) {
                    continue;
                }
                px = ((int)(x >> 3) + col * 2) * 8;
                blit_sprite_smooth(gfx, sheet + off, gfx->blen[seg] - off, px,
                                   (int)shift, attr, (int)y + row * 16,
                                   scroll76, scroll_speed, blank_sides);
            }
        }
        if (g_bulletlog && getenv("RUDEBR_ENTDUMP") &&
            g_bl_frames >= (unsigned)atoi(getenv("RUDEBR_ENTDUMP"))) {
            fprintf(stderr, "ent f=%u i=%d type=%04X seg=%u pat=%u x=%d y=%d w=%d h=%d tint=%d\n", g_bl_frames, i,
                    type, seg, pat, x, y, wcells, hcells, gfx->tint_cur);
        }
        if (g_bulletlog && (g_bulletlog > 1 || (type & 0x0180u))) {
            bulletlog_note(type, seg, pat, gfx->tint_cur);
        }
        gfx->tint_cur = 0;
    }
    gfx->tint_cur = 0;
    gfx->paint_bg = 1;
}

static int pal_byte(const uint8_t **pp, const uint8_t *end, int *out)
{
    if (*pp >= end) {
        return -1;
    }
    *out = *(*pp)++;
    return 0;
}

void gfx_palette_abs(RbGfx *gfx, const uint8_t *p, uint32_t n)
{
    const uint8_t *end;

    if (!gfx || !p) {
        return;
    }
    end = p + n;
    for (;;) {
        int idx;
        int c;
        int v;

        if (pal_byte(&p, end, &idx) != 0 || idx > 15) {
            break;
        }
        for (c = 0; c < 3; c++) {
            if (pal_byte(&p, end, &v) != 0) {
                return;
            }
            gfx->pal[idx][c] = (int16_t)(v << 4);
            gfx->dpal[idx][c] = 0;
        }
    }
}

void gfx_palette_delta(RbGfx *gfx, const uint8_t *p, uint32_t n)
{
    const uint8_t *end;
    int i;
    int c0;

    if (!gfx || !p) {
        return;
    }
    /* 0x6CAB zeros the whole delta table before the stream. A later
     * step keeps adding those words, so a partial stream must not
     * leave the previous fade's deltas in the other colors. */
    for (i = 0; i < 16; i++) {
        for (c0 = 0; c0 < 3; c0++) {
            gfx->dpal[i][c0] = 0;
        }
    }
    end = p + n;
    for (;;) {
        int idx;
        int c;
        int v;

        if (pal_byte(&p, end, &idx) != 0 || idx > 15) {
            break;
        }
        for (c = 0; c < 3; c++) {
            int target;
            int cur;

            if (pal_byte(&p, end, &v) != 0) {
                return;
            }
            target = v << 4;
            cur = gfx->pal[idx][c];
            gfx->dpal[idx][c] = (int16_t)((target - cur) >> 4);
        }
    }
}

/* 0x6C0B samples the palette out to the DAC. It does not write the
 * words back: a fade with shift 0 accumulates in the low nibble, and
 * storing (low >> 4) << 4 would clear that nibble every frame. */
void gfx_dac_flush(RbGfx *gfx)
{
    (void)gfx;
}

static int dac_level(int16_t w)
{
    if ((w & 0xFF00) != 0) {
        return 15;
    }
    return (int)(((uint16_t)w & 0xFFu) >> 4);
}

/* Modern grade, one row per settled palette the stages actually hold.
 * src is the 4-bit PC-98 level. A 0 modern channel with a 0 src channel
 * is black; graded entries are marked in `use` (bit per index). Indexes
 * that are not marked stay on level * 17. Fades scale the modern color
 * by the current level over that settled level, per channel, so the
 * picture still fades instead of popping. It is not one channel curve:
 * stage 4 index 0 red level 3 becomes 32, and index 1 red level 3 stays 51. */
#define GRADE_N 6
static const uint8_t GRADE_SRC[GRADE_N][16][3] = {
    { /* stage 1 */
        {2,1,2},{9,4,3},{3,4,4},{4,5,6},{6,5,5},{8,7,7},{13,9,4},{11,11,10},
        {9,4,4},{0,0,0},{8,7,10},{14,6,6},{15,12,7},{8,5,12},{12,12,15},{15,15,15}
    },
    { /* stage 2 */
        {2,1,1},{12,10,5},{4,4,4},{7,5,5},{9,7,6},{11,9,9},{10,3,4},{12,11,11},
        {9,4,4},{0,0,0},{8,7,10},{14,6,6},{15,12,7},{8,5,12},{12,12,15},{15,15,15}
    },
    { /* stage 3 */
        {2,3,2},{11,8,2},{7,8,7},{10,10,8},{4,5,4},{3,4,5},{5,5,6},{6,7,8},
        {9,4,4},{0,0,0},{8,7,10},{14,6,6},{15,12,7},{8,5,12},{12,12,15},{15,15,15}
    },
    { /* stage 4 */
        {3,3,2},{3,4,3},{12,2,2},{4,5,4},{4,6,6},{6,8,7},{9,9,7},{10,10,9},
        {9,4,4},{0,0,0},{8,7,10},{14,6,6},{15,12,7},{8,5,12},{12,12,15},{15,15,15}
    },
    { /* stage 5 */
        {3,3,3},{3,4,5},{12,2,2},{5,5,6},{6,7,8},{9,10,9},{11,11,9},{12,12,11},
        {9,4,4},{0,0,0},{8,7,10},{14,6,6},{15,12,7},{8,5,12},{12,12,15},{15,15,15}
    },
    { /* stage 5 boss */
        {3,3,3},{11,6,3},{12,2,2},{11,8,5},{12,11,10},{9,10,9},{11,11,9},{12,12,11},
        {9,4,4},{0,0,0},{8,7,10},{14,6,6},{15,12,7},{8,5,12},{12,12,15},{15,15,15}
    }
};
/* Bit i set: index i uses GRADE_DST instead of level * 17. */
static const uint16_t GRADE_USE[GRADE_N] = {
    (1u << 0) | (1u << 10) | (1u << 11),
    (1u << 0) | (1u << 10) | (1u << 11),
    (1u << 0) | (1u << 6) | (1u << 7) | (1u << 10) | (1u << 11),
    (1u << 0) | (1u << 4) | (1u << 5) | (1u << 6) | (1u << 10) | (1u << 11),
    (1u << 1) | (1u << 3) | (1u << 4) | (1u << 10) | (1u << 11),
    (1u << 1) | (1u << 3) | (1u << 4) | (1u << 10) | (1u << 11)
};
static const uint8_t GRADE_DST[GRADE_N][16][3] = {
    { {12,1,13},{0},{0},{0},{0},{0},{0},{0},{0},{0},{125,110,162},{231,92,88} },
    { {11,1,1},{0},{0},{0},{0},{0},{0},{0},{0},{0},{125,110,162},{231,92,88} },
    { {36,48,48},{0},{0},{0},{0},{0},{77,86,105},{86,104,124},{0},{0},{125,110,162},{231,92,88} },
    { {32,48,32},{0},{0},{0},{53,86,88},{113,129,105},{160,161,125},{0},{0},{0},{125,110,162},{231,92,88} },
    { {0},{31,49,69},{0},{69,69,87},{86,104,124},{0},{0},{0},{0},{0},{125,110,162},{231,92,88} },
    { {0},{176,88,29},{0},{185,150,113},{198,179,160},{0},{0},{0},{0},{0},{125,110,162},{231,92,88} }
};

static int g_modern;
/* Per preset: main (idx 11), dark (8, 9), highlight (12 inside a red
 * bullet). Cave-style pinks and lilacs; none of them is in the palette. */
static const uint8_t k_bullet[5][3][3] = {
    {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
    {{255, 96, 208}, {176, 32, 136}, {255, 200, 236}},
    {{216, 160, 255}, {128, 72, 200}, {240, 226, 255}},
    {{80, 200, 255}, {24, 88, 220}, {220, 248, 255}},
    {{120, 255, 200}, {0, 150, 120}, {230, 255, 244}},
};

void gfx_set_bullet_color(int preset)
{
    g_bullet = preset < 0 || preset > 4 ? 0 : preset;
}

void gfx_set_grade(int modern)
{
    g_modern = modern ? 1 : 0;
}

/* Which settled palette the current DAC levels are fading toward.
 * -1 keeps every index on the original lookup (title, intro). */
static int grade_scene(const RbGfx *gfx)
{
    int level[16][3];
    int idx;
    int ch;
    int best = -1;
    int best_dist = 0x7fffffff;
    int scene;

    if (!gfx) {
        return -1;
    }
    for (idx = 0; idx < 16; idx++) {
        for (ch = 0; ch < 3; ch++) {
            level[idx][ch] = dac_level(gfx->pal[idx][ch]);
        }
    }
    for (scene = 0; scene < GRADE_N; scene++) {
        int dist = 0;
        int ok = 1;

        for (idx = 0; idx < 16 && ok; idx++) {
            for (ch = 0; ch < 3; ch++) {
                int src = GRADE_SRC[scene][idx][ch];
                int cur = level[idx][ch];

                if (cur > src) {
                    ok = 0;
                    break;
                }
                dist += src - cur;
            }
        }
        if (ok && dist < best_dist) {
            best_dist = dist;
            best = scene;
        }
    }
    if (best >= 0) {
        return best;
    }
    /* The Compile logo holds the stage 1 field colors (indexes 0-7)
     * with its own shades in some ungraded shared indexes (13, 14).
     * Those keep level * 17 either way, so match on the field colors
     * and the graded indexes only. The logo then starts on the same
     * grade the game uses instead of switching to it when the shared
     * indexes are reloaded after the logo. */
    for (scene = 0; scene < GRADE_N; scene++) {
        int dist = 0;
        int ok = 1;

        /* Index 0 (the field color) must sit on the scene's own
         * settled value, which keeps black screens such as the
         * rising title on the original lookup. */
        for (ch = 0; ch < 3; ch++) {
            if (level[0][ch] != GRADE_SRC[scene][0][ch]) {
                ok = 0;
            }
        }
        for (idx = 0; idx < 16 && ok; idx++) {
            if (idx >= 8 && !(GRADE_USE[scene] & (1u << idx))) {
                continue;
            }
            for (ch = 0; ch < 3; ch++) {
                int src = GRADE_SRC[scene][idx][ch];
                int cur = level[idx][ch];

                if (cur > src) {
                    ok = 0;
                    break;
                }
                dist += src - cur;
            }
        }
        if (ok && dist < best_dist) {
            best_dist = dist;
            best = scene;
        }
    }
    return best;
}

static void expand_index(int scene, int idx, int r4, int g4, int b4,
                         int *R, int *G, int *B)
{
    int ch;
    int cur[3];
    int out[3];

    cur[0] = r4;
    cur[1] = g4;
    cur[2] = b4;
    if (scene < 0 || idx < 0 || idx > 15 ||
        (GRADE_USE[scene] & (1u << idx)) == 0) {
        *R = r4 * 17;
        *G = g4 * 17;
        *B = b4 * 17;
        return;
    }
    for (ch = 0; ch < 3; ch++) {
        int src = GRADE_SRC[scene][idx][ch];

        if (src <= 0 || cur[ch] >= src) {
            out[ch] = GRADE_DST[scene][idx][ch];
        } else {
            out[ch] = (GRADE_DST[scene][idx][ch] * cur[ch]) / src;
        }
    }
    *R = out[0];
    *G = out[1];
    *B = out[2];
}

void gfx_palette_rgb(const RbGfx *gfx, uint8_t rgb[16][3])
{
    int idx;
    int ch;

    if (!rgb) {
        return;
    }
    for (idx = 0; idx < 16; idx++) {
        for (ch = 0; ch < 3; ch++) {
            int v = gfx ? dac_level(gfx->pal[idx][ch]) : 0;

            rgb[idx][ch] = (uint8_t)(v * 17);
        }
    }
}

void gfx_palette_step(RbGfx *gfx, uint8_t shift)
{
    int idx;
    int c;

    if (!gfx) {
        return;
    }
    if (shift > 15) {
        shift = 15;
    }
    for (idx = 0; idx < 16; idx++) {
        for (c = 0; c < 3; c++) {
            gfx->pal[idx][c] = (int16_t)(gfx->pal[idx][c] +
                                        (gfx->dpal[idx][c] << shift));
        }
    }
}

void gfx_define_glyph(RbGfx *gfx, uint16_t code, const uint8_t *bits, uint32_t n)
{
    unsigned id;

    if (!gfx || !bits || n < 32u || code < 0x7600u || code > 0x77FFu) {
        return;
    }
    id = (unsigned)code - 0x7600u;
    memcpy(gfx->glyph[id], bits, 32);
    gfx->glyph_on[id] = 1;
}

const uint8_t *gfx_bank_data(const RbGfx *gfx, uint8_t slot, uint32_t *len)
{
    if (!gfx) {
        if (len) {
            *len = 0;
        }
        return NULL;
    }
    if (len) {
        *len = gfx->blen[slot];
    }
    return gfx->bank[slot];
}

void gfx_clear_frame(RbGfx *gfx)
{
    if (!gfx) {
        return;
    }
    memset(gfx->pix, 0, (size_t)RB_GFX_W * (size_t)RB_GFX_H);
    memset(gfx->bg, 0, (size_t)RB_GFX_W * (size_t)RB_GFX_H);
    gfx->active = 1;
}

void gfx_merge_plane(RbGfx *gfx, int plane, const uint8_t *src, uint32_t n)
{
    uint32_t i;
    uint8_t bit;

    if (!gfx || !src || plane < 0 || plane > 3) {
        return;
    }
    bit = (uint8_t)(1u << plane);
    if (n > 32000u) {
        n = 32000u;
    }
    for (i = 0; i < n; i++) {
        int x = (int)(i % 80u) * 8;
        int y = (int)(i / 80u);
        int p;

        if (y >= RB_GFX_H) {
            break;
        }
        /* 0x6ACD writes the bytes it has and leaves the rest of the
         * plane. A 0 bit clears that plane; a 1 bit sets it. */
        for (p = 0; p < 8; p++) {
            int on = (src[i] >> (7 - p)) & 1;

            rmw_idx(gfx, x + p, y, bit, on);
        }
    }
    gfx->active = 1;
}

static int di_xy(uint16_t di, int *x, int *y)
{
    return plane_xy(di, x, y);
}

static void blit_tile_shift(RbGfx *gfx, const uint8_t *src, int x, int y, int shift)
{
    int row;

    if (shift <= 0) {
        blit_tile(gfx, src, x, y);
        return;
    }
    if (shift > 7) {
        shift = 7;
    }
    for (row = 0; row < 16; row++) {
        int plane;
        uint8_t bits[3][24];
        int col;

        memset(bits, 0, sizeof bits);
        for (plane = 0; plane < 3; plane++) {
            const uint8_t *p = src + plane * 32 + row * 2;
            int i;

            for (i = 0; i < 8; i++) {
                if ((p[0] >> (7 - i)) & 1) {
                    bits[plane][i + shift] = 1;
                }
                if ((p[1] >> (7 - i)) & 1) {
                    bits[plane][8 + i + shift] = 1;
                }
            }
        }
        for (col = 0; col < 16 + shift; col++) {
            uint8_t idx = 0;

            if (bits[0][col]) {
                idx |= 1u;
            }
            if (bits[1][col]) {
                idx |= 2u;
            }
            if (bits[2][col]) {
                idx |= 4u;
            }
            put_idx(gfx, x + col, y + row, idx);
        }
    }
}

void gfx_blit_grid(RbGfx *gfx, const uint8_t *bank, uint32_t blen,
                   const uint8_t *cells, uint32_t cell_n,
                   uint16_t di, int cols, int rows, int kind, int shift)
{
    int row;
    uint32_t ncell = 0;
    uint16_t row_di;

    if (!gfx || !bank || !cells || cols <= 0 || rows <= 0) {
        return;
    }
    row_di = di;
    for (row = 0; row < rows; row++) {
        int col;
        uint16_t at = row_di;

        for (col = 0; col < cols; col++) {
            uint8_t index;
            int x;
            int y;

            if (ncell >= cell_n) {
                gfx->active = 1;
                return;
            }
            index = cells[ncell++];
            if (!di_xy(at, &x, &y)) {
                at = (uint16_t)(at + 2u);
                continue;
            }
            if (kind == 1) {
                uint32_t off = (uint32_t)index * 128u;

                if (off < blen) {
                    blit_sprite(gfx, bank + off, blen - off, x, y, 0, 0);
                }
            } else {
                uint32_t off = (uint32_t)index * 160u + 32u;

                if (off + 96u <= blen) {
                    blit_tile_shift(gfx, bank + off, x, y, shift);
                }
            }
            at = (uint16_t)(at + 2u);
        }
        row_di = (uint16_t)(row_di + 0x500u);
    }
    gfx->active = 1;
}

/* GRCG RMW: a CPU 1-bit writes `tile` into the selected planes and a
 * 0-bit leaves the pixel alone. Tile 0 therefore clears; tile 1 sets. */
static void grcg_bit(RbGfx *gfx, int x, int y, uint8_t planes, int set)
{
    rmw_idx(gfx, x, y, planes, set);
}

static void span_skip(const uint8_t **p, const uint8_t *end, uint32_t n)
{
    if ((uint32_t)(end - *p) <= n) {
        *p = end;
    } else {
        *p += n;
    }
}

/* One row of 0x7541 / 0x7795. shift 0 writes row_bytes bytes. A shift of
 * 1..7 inserts that many zero pixels on the left (bit 7 is leftmost) and
 * spills into one extra byte, which is what 0x7795 stores. A solid row
 * (src_row == NULL) is the 0xFF rectangle cleared under tile 0. */
static void grcg_row(RbGfx *gfx, uint16_t di, const uint8_t *src_row,
                     int row_bytes, int shift, uint8_t planes, int set)
{
    int out_bits = row_bytes * 8 + shift;
    int i;

    for (i = 0; i < out_bits; i++) {
        int src_i = i - shift;
        int bit = 0;
        uint16_t at;
        int x;
        int y;

        if (src_i >= 0) {
            if (!src_row) {
                bit = 1;
            } else if ((src_i >> 3) < row_bytes) {
                bit = (src_row[src_i >> 3] >> (7 - (src_i & 7))) & 1;
            }
        }
        if (!bit) {
            continue;
        }
        at = (uint16_t)(di + (uint16_t)(i >> 3));
        /* Past 0x7D00 is the gap. 16-bit wrap under that is visible. */
        if (at >= 0x7D00u) {
            continue;
        }
        x = (int)(at % 80u) * 8 + (i & 7);
        y = (int)(at / 80u);
        grcg_bit(gfx, x, y, planes, set);
    }
}

/* 0x7541. AL==0 and both mask bits is the unshifted erase-then-paint.
 * AL!=0 is the shifted form at 0x7679. The original skip multiplies by
 * CX while CX still holds (row_bytes<<8)|shift; the copy at 0x7795
 * consumes row_bytes per row, and that is the count used here. */
static void blit_masked(RbGfx *gfx, const uint8_t *src, uint32_t slen,
                        uint16_t di, uint16_t row_bytes, uint16_t rows,
                        uint8_t src_planes, uint8_t dst_planes, int shift)
{
    static const uint8_t bit[4] = {0x01u, 0x02u, 0x04u, 0x08u};
    const uint8_t *p = src;
    const uint8_t *end = src + slen;
    uint8_t row[96];
    int mask_src;
    int plane;
    uint16_t row_i;

    if (row_bytes > sizeof row) {
        return;
    }
    if (shift < 0) {
        shift = 0;
    }
    if (shift > 7) {
        shift &= 7;
    }
    mask_src = (src_planes & 0x10u) && (dst_planes & 0x10u);
    if ((src_planes & 0x10u) && !mask_src) {
        span_skip(&p, end, (uint32_t)row_bytes * rows);
    }
    for (row_i = 0; row_i < rows; row_i++) {
        uint16_t at = (uint16_t)(di + (uint16_t)(row_i * 0x50u));

        if (mask_src) {
            uint16_t b;

            for (b = 0; b < row_bytes; b++) {
                row[b] = (p < end) ? *p++ : 0;
            }
            grcg_row(gfx, at, row, (int)row_bytes, shift, 0x0Fu, 0);
        } else {
            grcg_row(gfx, at, NULL, (int)row_bytes, shift, 0x0Fu, 0);
        }
    }
    for (plane = 0; plane < 4; plane++) {
        if (dst_planes & bit[plane]) {
            if (src_planes & bit[plane]) {
                for (row_i = 0; row_i < rows; row_i++) {
                    uint16_t b;
                    uint16_t at = (uint16_t)(di + (uint16_t)(row_i * 0x50u));

                    for (b = 0; b < row_bytes; b++) {
                        row[b] = (p < end) ? *p++ : 0;
                    }
                    grcg_row(gfx, at, row, (int)row_bytes, shift, bit[plane], 1);
                }
            }
        } else if (src_planes & bit[plane]) {
            span_skip(&p, end, (uint32_t)row_bytes * rows);
        }
    }
    gfx->active = 1;
}

void gfx_blit_span(RbGfx *gfx, const uint8_t *src, uint32_t slen,
                   uint16_t di, uint16_t row_bytes, uint16_t rows,
                   uint8_t src_planes, uint8_t dst_planes, uint8_t shift)
{
    static const uint8_t bit[4] = {0x01u, 0x02u, 0x04u, 0x08u};
    uint32_t si = 0;
    int plane;

    if (!gfx || !src || row_bytes == 0 || rows == 0) {
        return;
    }
    /* 0x7428: AL!=0, or both BH and BL carrying bit 0x10, enters 0x7541. */
    if (shift != 0 || ((src_planes & 0x10u) && (dst_planes & 0x10u))) {
        blit_masked(gfx, src, slen, di, row_bytes, rows,
                    src_planes, dst_planes, (int)(shift & 7u));
        return;
    }
    if (src_planes & 0x10u) {
        uint32_t skip = (uint32_t)row_bytes * rows;

        if (si + skip > slen) {
            skip = slen - si;
        }
        si += skip;
    }
    for (plane = 0; plane < 4; plane++) {
        uint32_t bytes = (uint32_t)row_bytes * rows;

        if ((src_planes & bit[plane]) == 0) {
            continue;
        }
        if ((dst_planes & bit[plane]) == 0) {
            if (si + bytes > slen) {
                si = slen;
            } else {
                si += bytes;
            }
            continue;
        }
        {
            uint16_t row;
            uint16_t base = di;

            for (row = 0; row < rows; row++) {
                uint16_t b;

                for (b = 0; b < row_bytes; b++) {
                    uint8_t byte;
                    int x;
                    int y;
                    int p;

                    if (si >= slen) {
                        gfx->active = 1;
                        return;
                    }
                    byte = src[si++];
                    if (!plane_xy((uint32_t)base + b, &x, &y)) {
                        continue;
                    }
                    for (p = 0; p < 8; p++) {
                        rmw_idx(gfx, x + p, y, bit[plane],
                                (byte >> (7 - p)) & 1);
                    }
                }
                base = (uint16_t)(base + 0x50u);
            }
        }
    }
    gfx->active = 1;
}

int gfx_active(const RbGfx *gfx)
{
    return gfx && gfx->active;
}

static const uint8_t TEXT_RGB[8][3] = {
    {0, 0, 0},
    {0, 0, 255},
    {255, 0, 0},
    {255, 0, 255},
    {0, 255, 0},
    {0, 255, 255},
    {255, 255, 0},
    {255, 255, 255}
};

static void paint_glyph_half(uint32_t *rgba, int w, int x, int y,
                             const uint8_t *bits, int right, uint32_t rgb)
{
    int row;

    for (row = 0; row < 16; row++) {
        uint8_t line = bits[row * 2 + (right ? 1 : 0)];
        int col;

        for (col = 0; col < 8; col++) {
            int px;
            int py;

            if (((line >> (7 - col)) & 1) == 0) {
                continue;
            }
            px = x + col;
            py = y + row;
            if ((unsigned)px >= (unsigned)RB_GFX_W ||
                (unsigned)py >= (unsigned)RB_GFX_H) {
                continue;
            }
            rgba[(size_t)py * (size_t)w + (size_t)px] = rgb;
        }
    }
}

/* Boot-font stand-in for the PC-98 kanji the text layer would show.
 * The phrases are the JIS codes draw_text leaves in VRAM. */
#include "text_en.inc"

static const char *const KANA[] = {
    "A", "A", "I", "I", "U", "U", "E", "E", "O", "O",
    "KA", "GA", "KI", "GI", "KU", "GU", "KE", "GE", "KO", "GO",
    "SA", "ZA", "SHI", "JI", "SU", "ZU", "SE", "ZE", "SO", "ZO",
    "TA", "DA", "CHI", "DI", "", "TSU", "DU", "TE", "DE", "TO", "DO",
    "NA", "NI", "NU", "NE", "NO",
    "HA", "BA", "PA", "HI", "BI", "PI", "FU", "BU", "PU", "HE", "BE", "PE",
    "HO", "BO", "PO",
    "MA", "MI", "MU", "ME", "MO",
    "YA", "YA", "YU", "YU", "YO", "YO",
    "RA", "RI", "RU", "RE", "RO",
    "WA", "WA", "WI", "WE", "WO", "N"
};

static int fold_jis(uint16_t jis, char *dst, int cap)
{
    unsigned j1 = (unsigned)(jis >> 8);
    unsigned j2 = (unsigned)(jis & 0xFFu);
    const char *s = NULL;
    char one[2];

    if (jis == 0x2121u || jis == 0x2126u) {
        s = " ";
    } else if (jis == 0x2132u) {
        s = "_";
    } else if (j1 == 0x23u && j2 >= 0x21u && j2 < 0x7Fu) {
        one[0] = (char)j2;
        if (one[0] >= 'a' && one[0] <= 'z') {
            one[0] = (char)(one[0] - 32);
        }
        one[1] = '\0';
        s = one;
    } else if ((j1 == 0x24u || j1 == 0x25u) &&
               j2 >= 0x21u && j2 < 0x21u + (sizeof KANA / sizeof KANA[0])) {
        s = KANA[j2 - 0x21u];
    }
    if (!s) {
        return -1;
    }
    {
        int n = (int)strlen(s);

        if (n >= cap) {
            return -1;
        }
        memcpy(dst, s, (size_t)n);
        return n;
    }
}

static void paint_en(const RbGfx *gfx, uint32_t *rgba, int w, int x, int y,
                     const char *s, uint32_t rgb)
{
    for (; *s; s++) {
        int ch = (unsigned char)*s;
        uint16_t code = 0;
        unsigned id;
        int col;

        if (x >= w - 1) {
            return;
        }
        if (ch >= 'a' && ch <= 'z') {
            ch -= 32;
        }
        if (ch == ' ') {
            x += 16;
            continue;
        }
        if (ch == '_') {
            for (col = 1; col < 14; col++) {
                int px = x + col;
                int py = y + 14;

                if ((unsigned)px < (unsigned)w && (unsigned)py < (unsigned)RB_GFX_H) {
                    rgba[(size_t)py * (size_t)w + (size_t)px] = rgb;
                }
            }
            x += 16;
            continue;
        }
        if (ch >= '0' && ch <= '9') {
            code = (uint16_t)(0x7621u + (unsigned)(ch - '0'));
        } else if (ch >= 'A' && ch <= 'Z') {
            code = (uint16_t)(0x7630u + (unsigned)(ch - 'A'));
        } else if (ch == '!') {
            code = 0x764Au;
        } else if (ch == '-' || ch == '/') {
            code = 0x764Cu;
        } else if (ch == '.') {
            code = 0x762Eu;
        } else {
            x += 16;
            continue;
        }
        id = (unsigned)code - 0x7600u;
        if (id < 512u && gfx->glyph_on[id]) {
            paint_glyph_half(rgba, w, x, y, gfx->glyph[id], 0, rgb);
            paint_glyph_half(rgba, w, x + 8, y, gfx->glyph[id], 1, rgb);
        }
        x += 16;
    }
}

static const char *phrase_en(const uint16_t *jis, int n)
{
    size_t i;
    int a = 0;
    int b = n;

    /* Ending lines pad the phrase with an ideographic space. The
     * dictionary keys are the words themselves. */
    while (a < b && (jis[a] == 0x2121u || jis[a] == 0x2126u)) {
        a++;
    }
    while (b > a && (jis[b - 1] == 0x2121u || jis[b - 1] == 0x2126u)) {
        b--;
    }
    n = b - a;
    jis += a;
    for (i = 0; i < sizeof k_phrase / sizeof k_phrase[0]; i++) {
        if (k_phrase[i].n == (unsigned char)n &&
            memcmp(k_phrase[i].jis, jis, (size_t)n * sizeof(uint16_t)) == 0) {
            return k_phrase[i].en;
        }
    }
    return NULL;
}

/* Standard JIS in the text layer (CONFIG, ranking names, the ending
 * message). Gaiji 0x76/0x77 stays with the glyph painter above. */
static void overlay_english(const RbGfx *gfx, const uint8_t *text,
                            uint32_t *rgba, int w)
{
    int row;

    for (row = 0; row < 25; row++) {
        int col = 0;

        while (col < 80) {
            unsigned off = (unsigned)row * 0xA0u + (unsigned)col * 2u;
            uint8_t b0 = text[off];
            uint8_t b1 = text[off + 1];
            unsigned low;
            uint16_t run[40];
            int n = 0;
            int start = col;
            int color;
            uint32_t rgb;
            const char *en;
            char folded[96];
            int flen = 0;
            int k;

            if ((b0 & 0x80u) != 0) {
                col++;
                continue;
            }
            if (b1 == 0 && (b0 == 0 || b0 == 0x20u)) {
                col++;
                continue;
            }
            low = (unsigned)(b0 & 0x7Fu) + 0x20u;
            if ((low == 0x76u || low == 0x77u) && b1 != 0) {
                col += 2;
                continue;
            }
            while (col < 80 && n < 40) {
                unsigned o = (unsigned)row * 0xA0u + (unsigned)col * 2u;
                uint8_t c0 = text[o];
                uint8_t c1 = text[o + 1];
                unsigned j1;

                if ((c0 & 0x80u) != 0) {
                    break;
                }
                if (c1 == 0 && (c0 == 0 || c0 == 0x20u)) {
                    break;
                }
                j1 = (unsigned)(c0 & 0x7Fu) + 0x20u;
                if ((j1 == 0x76u || j1 == 0x77u) && c1 != 0) {
                    break;
                }
                run[n++] = (uint16_t)((j1 << 8) | c1);
                col += 2;
            }
            if (n == 0) {
                col++;
                continue;
            }
            en = phrase_en(run, n);
            if (!en) {
                int ok = 1;

                for (k = 0; k < n; k++) {
                    int got = fold_jis(run[k], folded + flen, (int)sizeof folded - flen);

                    if (got < 0) {
                        ok = 0;
                        break;
                    }
                    flen += got;
                }
                if (!ok || flen == 0) {
                    continue;
                }
                folded[flen] = '\0';
                en = folded;
            }
            color = (text[0x2000u + (unsigned)row * 0xA0u + (unsigned)start * 2u] >> 5) & 7;
            if (color == 0) {
                color = 7;
            }
            rgb = 0xFFu |
                  ((uint32_t)TEXT_RGB[color][0] << 24) |
                  ((uint32_t)TEXT_RGB[color][1] << 16) |
                  ((uint32_t)TEXT_RGB[color][2] << 8);
            paint_en(gfx, rgba, w, start * 8, row * 16, en, rgb);
        }
    }
}


/* Compact stage HUD (row 1). The original row has no room for a speed
 * readout, so when the SCORE / HI labels are on that row it is redrawn
 * with the same glyphs packed to their ink: icon, lives, SCORE and the
 * score field on the left edge of the playfield, HI and the hiscore
 * field on the right edge, and SPD centered in the gap between them.
 * Level 0 leaves the row exactly as the original draws it. */
static int g_hud_speed;

void gfx_set_hud_speed(int level)
{
    g_hud_speed = level < 0 ? 0 : (level > 3 ? 3 : level);
}

#define HUD_ROW   1
#define HUD_LEFT  80
#define HUD_RIGHT 560
#define HUD_GAP   1
#define HUD_DIGIT 14

static uint16_t hud_cell(const uint8_t *text, int col)
{
    unsigned off = (unsigned)HUD_ROW * 0xA0u + (unsigned)col * 2u;

    return (uint16_t)(text[off] | (text[off + 1u] << 8));
}

static uint32_t hud_rgb(const uint8_t *text, int col)
{
    unsigned off = 0x2000u + (unsigned)HUD_ROW * 0xA0u + (unsigned)col * 2u;
    int color = (text[off] >> 5) & 7;

    return 0xFFu |
           ((uint32_t)TEXT_RGB[color][0] << 24) |
           ((uint32_t)TEXT_RGB[color][1] << 16) |
           ((uint32_t)TEXT_RGB[color][2] << 8);
}

/* Glyph id behind a left-half gaiji cell, or -1 for a blank cell. */
static int hud_glyph(const RbGfx *gfx, const uint8_t *text, int col)
{
    uint16_t c = hud_cell(text, col);
    uint8_t b0 = (uint8_t)c;
    uint8_t b1 = (uint8_t)(c >> 8);
    unsigned low = (unsigned)(b0 & 0x7Fu) + 0x20u;

    if (b1 == 0 || low != 0x76u || (b0 & 0x80u) || !gfx->glyph_on[b1]) {
        return -1;
    }
    return b1;
}

static void hud_ink(const RbGfx *gfx, int id, int *x0, int *x1, int *y0, int *y1)
{
    int r;
    int c;

    *x0 = 16; *x1 = -1; *y0 = 16; *y1 = -1;
    for (r = 0; r < 16; r++) {
        unsigned line = ((unsigned)gfx->glyph[id][r * 2] << 8) |
                        gfx->glyph[id][r * 2 + 1];

        for (c = 0; c < 16; c++) {
            if (line & (0x8000u >> c)) {
                if (c < *x0) *x0 = c;
                if (c > *x1) *x1 = c;
                if (r < *y0) *y0 = r;
                if (r > *y1) *y1 = r;
            }
        }
    }
}

static int hud_width(const RbGfx *gfx, int id)
{
    int x0, x1, y0, y1;

    hud_ink(gfx, id, &x0, &x1, &y0, &y1);
    return x1 < x0 ? 0 : x1 - x0 + 1;
}

/* Paints glyph id with its first ink column at x. Returns ink width. */
static int hud_put(const RbGfx *gfx, uint32_t *rgba, int w, int x, int id,
                   uint32_t rgb)
{
    int x0, x1, y0, y1;

    hud_ink(gfx, id, &x0, &x1, &y0, &y1);
    if (x1 < x0) {
        return 0;
    }
    paint_glyph_half(rgba, w, x - x0, HUD_ROW * 16, gfx->glyph[id], 0, rgb);
    paint_glyph_half(rgba, w, x - x0 + 8, HUD_ROW * 16, gfx->glyph[id], 1, rgb);
    return x1 - x0 + 1;
}

/* Packed run of label glyphs (cols first..last step 2). */
static int hud_label_width(const RbGfx *gfx, const uint8_t *text, int first, int last)
{
    int col;
    int n = 0;
    int width = 0;

    for (col = first; col <= last; col += 2) {
        int id = hud_glyph(gfx, text, col);

        if (id >= 0) {
            width += (n ? HUD_GAP : 0) + hud_width(gfx, id);
            n++;
        }
    }
    return width;
}

static int hud_label(const RbGfx *gfx, const uint8_t *text, uint32_t *rgba, int w,
                     int x, int first, int last)
{
    int col;
    int n = 0;

    for (col = first; col <= last; col += 2) {
        int id = hud_glyph(gfx, text, col);

        if (id >= 0) {
            x += n ? HUD_GAP : 0;
            x += hud_put(gfx, rgba, w, x, id, hud_rgb(text, col));
            n++;
        }
    }
    return x;
}

/* Eight digit cells, each centered in a fixed slot so the number does
 * not jitter as it changes. */
static int hud_digits(const RbGfx *gfx, const uint8_t *text, uint32_t *rgba, int w,
                      int x, int first)
{
    int i;

    for (i = 0; i < 8; i++) {
        int id = hud_glyph(gfx, text, first + i * 2);

        if (id >= 0) {
            int gw = hud_width(gfx, id);

            hud_put(gfx, rgba, w, x + (HUD_DIGIT - 1 - gw + 1) / 2, id,
                    hud_rgb(text, first + i * 2));
        }
        x += HUD_DIGIT;
    }
    return x - 1;
}

static int hud_present(const RbGfx *gfx, const uint8_t *text)
{
    return g_hud_speed > 0 &&
           hud_cell(text, 18) == 0x4256u && hud_cell(text, 50) == 0x3756u &&
           hud_cell(text, 52) == 0x3856u && gfx->glyph_on[0x21];
}

static int g_panel_on;

/* With the side panels up, the row keeps half a character (4 dots)
 * clear of each playfield edge so nothing meets the panel frames. */
static int g_hud_inset;
#define HUD_INSET (g_hud_inset ? 4 : 0)

void gfx_set_hud_inset(int on)
{
    g_hud_inset = on;
}

static void hud_row(const RbGfx *gfx, const uint8_t *text, uint32_t *rgba, int w)
{
    int x = HUD_LEFT + HUD_INSET;
    int left_end;
    int hi_x;
    int spd_w;
    int cx;
    int i;
    uint32_t green = hud_rgb(text, 18);
    uint32_t yellow = hud_rgb(text, 28);
    /* S, P, D in the gaiji alphabet (0x7630 + letter). */
    static const int spd[3] = {0x42, 0x3F, 0x33};

    /* Ship icon and lives. */
    x = hud_label(gfx, text, rgba, w, x, 10, 10);
    x += 4;
    x = hud_label(gfx, text, rgba, w, x, 14, 14);
    x += 9;
    x = hud_label(gfx, text, rgba, w, x, 18, 26);
    x += 5;
    left_end = hud_digits(gfx, text, rgba, w, x, 28);

    /* Right group: hiscore field flush with the playfield edge. */
    hi_x = HUD_RIGHT - HUD_INSET - 8 * HUD_DIGIT + 1;
    hud_digits(gfx, text, rgba, w, hi_x, 54);
    if (g_hud_inset) {
        /* Panel mode: HI sits against its first digit, not the empty
         * slots, so the three groups are spaced evenly. */
        int lead = 0;

        while (lead < 7 && hud_glyph(gfx, text, 54 + lead * 2) < 0) {
            lead++;
        }
        hi_x += lead * HUD_DIGIT;
    }
    hi_x -= 5 + hud_label_width(gfx, text, 50, 52);
    hud_label(gfx, text, rgba, w, hi_x, 50, 52);

    /* SPD plus LOW / MID / MAX, centered in the gap. */
    {
        static const char *const word[3] = {"LOW", "MID", "MAX"};
        const char *wd = word[g_hud_speed - 1];
        int j;

        spd_w = 0;
        for (i = 0; i < 3; i++) {
            spd_w += hud_width(gfx, spd[i]) + (i ? HUD_GAP : 0);
        }
        spd_w += 5;
        for (j = 0; wd[j]; j++) {
            spd_w += hud_width(gfx, 0x30 + (wd[j] - 'A')) + (j ? HUD_GAP : 0);
        }
        cx = left_end + 1 + ((hi_x - 1) - left_end - spd_w) / 2;
        for (i = 0; i < 3; i++) {
            if (!gfx->glyph_on[spd[i]]) {
                return;
            }
            cx += (i ? HUD_GAP : 0);
            cx += hud_put(gfx, rgba, w, cx, spd[i], green);
        }
        cx += 5;
        for (j = 0; wd[j]; j++) {
            int id = 0x30 + (wd[j] - 'A');

            if (!gfx->glyph_on[id]) {
                return;
            }
            cx += (j ? HUD_GAP : 0);
            cx += hud_put(gfx, rgba, w, cx, id, yellow);
        }
    }
}



/* Gaiji alphabet: digits at 0x21, letters at 0x30. -1 for a space. */
static int brand_id(char c)
{
    if (c >= '0' && c <= '9') {
        return 0x21 + (c - '0');
    }
    if (c >= 'A' && c <= 'Z') {
        return 0x30 + (c - 'A');
    }
    return -1;
}

/* Title branding. The (c) COMPILE 1996 line is part of the title art
 * (logo screen and title screen, same place). It is found by sampling
 * points that are ink on both screens and points that are always
 * blank; checking only that ink matches ink and differs from blank
 * keeps the test true through the palette fades. */
static const int16_t BRAND_ON[][2] = {
    {239, 290}, {333, 298}, {234, 289}, {283, 292}, {402, 289}, {270, 297},
    {235, 296}, {317, 296}, {390, 299}, {408, 294}, {271, 291}, {307, 289},
    {220, 297}, {305, 288}, {218, 295}, {400, 295}, {243, 299}, {235, 288},
    {320, 300}, {428, 295}, {320, 292}, {405, 300}, {375, 291}, {407, 298}
};
static const int16_t BRAND_OFF[][2] = {
    {297, 285}, {363, 296}, {200, 283}, {377, 282}, {403, 282}, {270, 282},
    {439, 300}, {429, 290}, {284, 302}, {432, 282}, {414, 305}, {237, 291},
    {414, 302}, {398, 304}, {387, 291}, {209, 298}, {359, 291}, {202, 291},
    {347, 303}, {198, 295}, {371, 282}, {236, 302}, {279, 285}, {315, 288}
};

/* 12-row title credit font (3-dot stems, like the game's own letters). */
typedef struct { char c; uint8_t w; uint16_t rows[12]; } BrandGlyph;
static const BrandGlyph BRAND_FONT[] = {
    {'C', 9, {0x1FE, 0x1FF, 0x007, 0x007, 0x007, 0x007, 0x007, 0x007, 0x007, 0x007, 0x1FF, 0x1FE}},
    {'O', 9, {0x0FE, 0x1FF, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1FF, 0x0FE}},
    {'0', 9, {0x0FE, 0x1FF, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1FF, 0x0FE}},
    {'M', 9, {0x1C7, 0x1EF, 0x1FF, 0x1D7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7}},
    {'P', 9, {0x0FF, 0x1FF, 0x1C7, 0x1C7, 0x1C7, 0x1FF, 0x0FF, 0x007, 0x007, 0x007, 0x007, 0x007}},
    {'I', 3, {0x007, 0x007, 0x007, 0x007, 0x007, 0x007, 0x007, 0x007, 0x007, 0x007, 0x007, 0x007}},
    {'L', 9, {0x007, 0x007, 0x007, 0x007, 0x007, 0x007, 0x007, 0x007, 0x007, 0x007, 0x1FF, 0x1FF}},
    {'E', 9, {0x1FF, 0x1FF, 0x007, 0x007, 0x007, 0x0FF, 0x0FF, 0x007, 0x007, 0x007, 0x1FF, 0x1FF}},
    {'S', 9, {0x1FE, 0x1FF, 0x007, 0x007, 0x007, 0x0FF, 0x1FE, 0x1C0, 0x1C0, 0x1C0, 0x1FF, 0x0FF}},
    {'H', 9, {0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1FF, 0x1FF, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7}},
    {'U', 9, {0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1FF, 0x0FE}},
    {'B', 9, {0x0FF, 0x1FF, 0x1C7, 0x1C7, 0x1C7, 0x0FF, 0x0FF, 0x1C7, 0x1C7, 0x1C7, 0x1FF, 0x0FF}},
    {'R', 9, {0x0FF, 0x1FF, 0x1C7, 0x1C7, 0x1C7, 0x1FF, 0x0FF, 0x077, 0x0E7, 0x1C7, 0x1C7, 0x1C7}},
    {'1', 6, {0x03C, 0x03E, 0x03F, 0x03C, 0x038, 0x038, 0x038, 0x038, 0x038, 0x038, 0x038, 0x038}},
    {'2', 9, {0x0FF, 0x1FF, 0x1C0, 0x1C0, 0x1C0, 0x1FE, 0x0FF, 0x007, 0x007, 0x007, 0x1FF, 0x1FF}},
    {'6', 9, {0x1FE, 0x1FF, 0x007, 0x007, 0x007, 0x0FF, 0x1FF, 0x1C7, 0x1C7, 0x1C7, 0x1FF, 0x0FE}},
    {'9', 9, {0x0FE, 0x1FF, 0x1C7, 0x1C7, 0x1C7, 0x1FF, 0x1FE, 0x1C0, 0x1C0, 0x1C0, 0x1FF, 0x0FF}},
    {'3', 9, {0x0FF, 0x1FF, 0x1C0, 0x1C0, 0x1C0, 0x1FE, 0x1FE, 0x1C0, 0x1C0, 0x1C0, 0x1FF, 0x0FF}},
    {'T', 9, {0x1FF, 0x1FF, 0x038, 0x038, 0x038, 0x038, 0x038, 0x038, 0x038, 0x038, 0x038, 0x038}},
    {'A', 9, {0x0FE, 0x1FF, 0x1C7, 0x1C7, 0x1C7, 0x1FF, 0x1FF, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7}},
    {'D', 9, {0x0FF, 0x1FF, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1FF, 0x0FF}},
    {'K', 9, {0x1C7, 0x0E7, 0x077, 0x03F, 0x01F, 0x00F, 0x00F, 0x01F, 0x03F, 0x077, 0x0E7, 0x1C7}},
    {'N', 9, {0x1C7, 0x1CF, 0x1DF, 0x1FF, 0x1FF, 0x1F7, 0x1E7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7}},
    {'V', 9, {0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x0EE, 0x0EE, 0x07C, 0x07C, 0x038}},
    {'Y', 9, {0x1C7, 0x1C7, 0x1C7, 0x1C7, 0x1FF, 0x0FE, 0x038, 0x038, 0x038, 0x038, 0x038, 0x038}},
    {'@', 11, {0x0F8, 0x1FC, 0x306, 0x673, 0x61B, 0x61B, 0x61B, 0x61B, 0x673, 0x306, 0x1FC, 0x0F8}},
};

static const BrandGlyph *brand_glyph(char c)
{
    size_t i;

    for (i = 0; i < sizeof BRAND_FONT / sizeof BRAND_FONT[0]; i++) {
        if (BRAND_FONT[i].c == c) {
            return &BRAND_FONT[i];
        }
    }
    return NULL;
}

/* Width of a credit line: 2 dots between letters, 7 for a space. */
static int credit_width(const char *s)
{
    int n = 0;
    int first = 1;

    for (; *s; s++) {
        const BrandGlyph *g = brand_glyph(*s);

        if (!g) {
            n += 7;
            first = 1;
            continue;
        }
        n += (first ? 0 : 2) + g->w;
        first = 0;
    }
    return n;
}

static void credit_text(uint32_t *rgba, int w, int x, int top,
                        const char *s, uint32_t rgb)
{
    int first = 1;

    for (; *s; s++) {
        const BrandGlyph *g = brand_glyph(*s);
        int r;
        int c;

        if (!g) {
            x += 7;
            first = 1;
            continue;
        }
        x += first ? 0 : 2;
        first = 0;
        for (r = 0; r < 12; r++) {
            if (top + r < 0 || top + r >= RB_GFX_H) {
                continue;
            }
            for (c = 0; c < g->w; c++) {
                if (g->rows[r] & (1u << c)) {
                    rgba[(size_t)(top + r) * (size_t)w + (size_t)(x + c)] = rgb;
                }
            }
        }
        x += g->w;
    }
}


/* ---- title effects ------------------------------------------------ */

static uint32_t g_clock; /* host frames, for the title effects */

void gfx_set_clock(uint32_t frame)
{
    g_clock = frame;
}

/* Halo strength 0..256 for a pulse that starts at phase 0 and lasts
 * GLOW_LEN frames: a quick rise and a slower fall. */
#define GLOW_PERIOD 300 /* 5 s */
#define GLOW_LEN    30

static int glow_at(int phase)
{
    if (phase < 0 || phase >= GLOW_LEN) return 0;
    if (phase < 8) return (phase + 1) * 256 / 8;
    return (GLOW_LEN - phase) * 256 / (GLOW_LEN - 8);
}

int gfx_glow_level(uint32_t frame)
{
    return glow_at((int)(frame % GLOW_PERIOD) - (GLOW_PERIOD - GLOW_LEN));
}

/* A soft halo around the bright (near-white) letters inside the box
 * [x0, x1] x [y0, y1], amt 0..256. Pixels within 2 dots of a letter
 * are tinted towards pale lilac, fading with distance. */
static int g_halo_r = 2;
static void halo_rgba(uint32_t *rgba, int w, int h, int x0, int y0, int x1, int y1, int amt);

/* The same halo on the 3x side panels: a 3x wider reach. */
void gfx_halo(uint32_t *rgba, int w, int h, int x0, int y0, int x1, int y1, int amt)
{
    g_halo_r = 6;
    halo_rgba(rgba, w, h, x0, y0, x1, y1, amt);
    g_halo_r = 2;
}

static void halo_rgba(uint32_t *rgba, int w, int h, int x0, int y0, int x1, int y1, int amt)
{
    enum { MW = 640, MH = 64 };
    const int R = g_halo_r;
    static uint8_t mask[MH][MW];
    int x;
    int y;

    if (amt <= 0) return;
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 >= w) x1 = w - 1;
    if (y1 >= h) y1 = h - 1;
    if (x1 - x0 + 1 > MW || y1 - y0 + 1 > MH || x1 < x0 || y1 < y0) return;
    for (y = y0; y <= y1; y++) {
        for (x = x0; x <= x1; x++) {
            uint32_t p = rgba[(size_t)y * (size_t)w + (size_t)x];
            mask[y - y0][x - x0] = (p >> 24) >= 200u && ((p >> 16) & 0xFFu) >= 200u &&
                                   ((p >> 8) & 0xFFu) >= 200u;
        }
    }
    for (y = y0; y <= y1; y++) {
        for (x = x0; x <= x1; x++) {
            int best = 9999;
            int a;
            int b;
            uint32_t *p;
            int r, g, bl, k;

            if (mask[y - y0][x - x0]) continue;
            for (b = -R; b <= R; b++) {
                for (a = -R; a <= R; a++) {
                    int xx = x + a, yy = y + b;
                    int d;

                    if (xx < x0 || xx > x1 || yy < y0 || yy > y1) continue;
                    if (!mask[yy - y0][xx - x0]) continue;
                    d = a * a + b * b;
                    if (d < best) best = d;
                }
            }
            if (best > R * R) continue;
            /* Fades with the square of the distance; at most ~40%. */
            k = amt * ((R + 1) * (R + 1) - best) / ((R + 1) * (R + 1)) * 2 / 5;
            p = &rgba[(size_t)y * (size_t)w + (size_t)x];
            r = (int)(*p >> 24);
            g = (int)((*p >> 16) & 0xFFu);
            bl = (int)((*p >> 8) & 0xFFu);
            r += (200 - r) * k / 256;
            g += (190 - g) * k / 256;
            bl += (255 - bl) * k / 256;
            *p = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)bl << 8) | 0xFFu;
        }
    }
}

/* PUSH TRIGGER: yellow and green from the panel palette, crossfading
 * every 10 s (a 1 s fade at each change). */
static uint32_t push_rgb(uint32_t frame)
{
    enum { HALF = 600, FADE = 60 };
    static const int yl[3] = {250, 230, 60};
    static const int gr[3] = {80, 240, 80};
    int ph = (int)(frame % (2 * HALF));
    int m; /* 0 = yellow, 256 = green */
    int c[3];
    int i;

    if (ph < HALF - FADE) m = 0;
    else if (ph < HALF) m = (ph - (HALF - FADE)) * 256 / FADE;
    else if (ph < 2 * HALF - FADE) m = 256;
    else m = 256 - (ph - (2 * HALF - FADE)) * 256 / FADE;
    /* Smoothstep. */
    m = m * m * (768 - 2 * m) / 65536;
    for (i = 0; i < 3; i++) c[i] = yl[i] + (gr[i] - yl[i]) * m / 256;
    return ((uint32_t)c[0] << 24) | ((uint32_t)c[1] << 16) | ((uint32_t)c[2] << 8) | 0xFFu;
}

/* Title layout, on an 8-dot baseline grid centred on x 320:
 *   logo ink ends at y 222 (right end x 594)
 *   30TH ANNIVERSARY EDITION  white, credit size, tracked to the logo ink
 *   PUSH TRIGGER  baseline 264 (moved up 6 dots from the game's row)
 *   (c) COMPILE 1996   baseline 288
 *   (c) SHMUPSBR 2026  baseline 304
 * The art's own (c) COMPILE 1996 line is erased and redrawn in the
 * smaller credit font so both credit lines match. */
#define TITLE_PUSH_DY   (-6)
#define TITLE_CREDIT1   277
#define TITLE_CREDIT2   293
#define TITLE_TAG_TOP   228
#define RANK_CREDIT_TOP 386 /* ranking screen: free text row 24 */
#define TITLE_TAG_RIGHT 586 /* 8 dots clear of the frame at x 595 */
#define TITLE_LOGO_L    47  /* the logo's ink, left and right */
#define TITLE_LOGO_R    592
static void card_line(uint32_t *rgba, int w, int cx, int top, int scale,
                      const char *s, uint32_t rgb);

/* The art's credit line found dy rows away from its settled place.
 * The title rises into view, so the art can sit anywhere on the page;
 * points that fall off the page are skipped, and enough of the line
 * must be visible for the test to count. */
static int brand_at(const uint32_t *rgba, int w, int dy, uint32_t *ink_out)
{
    uint32_t ink = 0;
    int have = 0;
    int seen = 0;
    size_t i;

    for (i = 0; i < sizeof BRAND_ON / sizeof BRAND_ON[0]; i++) {
        int y = BRAND_ON[i][1] + dy;
        uint32_t p;

        if (y < 0 || y >= RB_GFX_H) {
            continue;
        }
        p = rgba[(size_t)y * (size_t)w + (size_t)BRAND_ON[i][0]];
        if (!have) {
            if ((p & 0xF0F0F000u) == 0) {
                return 0;
            }
            ink = p;
            have = 1;
        } else if (p != ink) {
            return 0;
        }
        seen++;
    }
    if (seen < 4) {
        return 0;
    }
    for (i = 0; i < sizeof BRAND_OFF / sizeof BRAND_OFF[0]; i++) {
        int y = BRAND_OFF[i][1] + dy;

        if (y < 0 || y >= RB_GFX_H) {
            continue;
        }
        if (rgba[(size_t)y * (size_t)w + (size_t)BRAND_OFF[i][0]] == ink) {
            return 0;
        }
    }
    *ink_out = ink;
    return 1;
}

/* 1 when the text page holds anything besides row 16 (PUSH TRIGGER). */
static int text_besides_push(const uint8_t *text)
{
    int row;
    int col;

    if (!text) {
        return 0;
    }
    for (row = 0; row < 25; row++) {
        if (row == 16) {
            continue;
        }
        for (col = 0; col < 80; col++) {
            unsigned off = (unsigned)row * 0xA0u + (unsigned)col * 2u;

            if (!(text[off + 1] == 0 && (text[off] == 0 || text[off] == 0x20u))) {
                return 1;
            }
        }
    }
    return 0;
}

static void title_brand(const RbGfx *gfx, uint32_t *rgba, int w, int ranking)
{
    uint32_t ink = 0;
    uint32_t blank = 0;
    size_t i;
    int x;
    int y;
    int dy;
    int found = 0;
    int width;
    const char *line1 = "@ COMPILE 1996";
    const char *line2 = "@ SHMUPSBR 2026";
    const char *tag = "30TH ANNIVERSARY EDITION";

    if (g_panel_on || w < RB_GFX_W) {
        return;
    }
    for (i = 0; tag[i]; i++) {
        if (brand_id(tag[i]) >= 0 && !gfx->glyph_on[brand_id(tag[i])]) {
            return;
        }
    }
    /* Settled place first, then the rows the rising title passes. */
    if (brand_at(rgba, w, 0, &ink)) {
        dy = 0;
        found = 1;
    } else {
        for (dy = 1; dy < RB_GFX_H && !found; dy++) {
            if (brand_at(rgba, w, dy, &ink)) {
                found = 1;
                break;
            }
            if (brand_at(rgba, w, -dy, &ink)) {
                dy = -dy;
                found = 1;
                break;
            }
        }
    }
    if (!found) {
        return;
    }
    if (dy == 0) {
        g_title_frame = 1;
    }
    for (i = 0; i < sizeof BRAND_OFF / sizeof BRAND_OFF[0]; i++) {
        y = BRAND_OFF[i][1] + dy;
        if (y >= 0 && y < RB_GFX_H) {
            blank = rgba[(size_t)y * (size_t)w + (size_t)BRAND_OFF[i][0]];
            break;
        }
    }
    /* Erase the art's credit line. */
    for (y = 285 + dy; y <= 302 + dy; y++) {
        if (y < 0 || y >= RB_GFX_H) {
            continue;
        }
        for (x = 200; x <= 440; x++) {
            rgba[(size_t)y * (size_t)w + (size_t)x] = blank;
        }
    }
    /* Lift PUSH TRIGGER (text row 16, ink y 256-270) onto the grid.
     * Text is fixed on the page, so only the settled title has it. */
    if (dy == 0) {
        for (y = 254; y <= 272; y++) {
            for (x = 216; x <= 424; x++) {
                uint32_t *p = &rgba[(size_t)y * (size_t)w + (size_t)x];

                if (*p != blank) {
                    /* Recoloured, keeping the glyph's own shading. */
                    uint32_t q = *p;
                    uint32_t c = push_rgb(g_clock);
                    int lum = (int)(((q >> 24) + ((q >> 16) & 0xFFu) + ((q >> 8) & 0xFFu)) / 3u);
                    uint32_t r = (c >> 24) * (uint32_t)lum / 255u;
                    uint32_t gg = ((c >> 16) & 0xFFu) * (uint32_t)lum / 255u;
                    uint32_t b = ((c >> 8) & 0xFFu) * (uint32_t)lum / 255u;

                    rgba[(size_t)(y + TITLE_PUSH_DY) * (size_t)w + (size_t)x] =
                        (r << 24) | (gg << 16) | (b << 8) | 0xFFu;
                    *p = blank;
                }
            }
        }
    }
    if (ranking) {
        /* The ranking table fills the odd text rows 1-23, so both
         * credits share one line in the free row 24 (y 384-399), on a
         * black band over the frame art. 30TH AE keeps its place in
         * the free row 14, right of the NAME column. */
        int w1 = credit_width(line1);
        int w2 = credit_width(line2);
        int gap = 16;
        int total = w1 + gap + w2;
        int x0 = 320 - total / 2;

        for (y = RANK_CREDIT_TOP - 2; y < RB_GFX_H; y++) {
            for (x = x0 - 8; x < x0 + total + 8; x++) {
                rgba[(size_t)y * (size_t)w + (size_t)x] = 0x000000FFu;
            }
        }
        credit_text(rgba, w, x0, RANK_CREDIT_TOP, line1, ink);
        credit_text(rgba, w, x0 + w1 + gap, RANK_CREDIT_TOP, line2, ink);
        width = credit_width(tag);
        credit_text(rgba, w, TITLE_TAG_RIGHT - width + 1, TITLE_TAG_TOP, tag, ink);
        return;
    }
    width = credit_width(line1);
    credit_text(rgba, w, 320 - width / 2, TITLE_CREDIT1 + dy, line1, ink);
    width = credit_width(line2);
    credit_text(rgba, w, 320 - width / 2, TITLE_CREDIT2 + dy, line2, ink);
    /* Written in full, in white, across the logo's ink (x 47-592),
     * as on the panel logo and the intro card. */
    {
        /* It follows the palette fade: the white is scaled by how far
         * the art's own credit ink is from its settled brightness, and
         * the glow only runs while the title is fully up. */
        static unsigned full;
        unsigned m = ink >> 24;
        unsigned v;

        if (((ink >> 16) & 0xFFu) > m) m = (ink >> 16) & 0xFFu;
        if (((ink >> 8) & 0xFFu) > m) m = (ink >> 8) & 0xFFu;
        if (m > full) full = m;
        v = full ? 255u * m / full : 255u;
        card_line(rgba, w, (TITLE_LOGO_L + TITLE_LOGO_R + 1) / 2, TITLE_TAG_TOP + 2 + dy, 1, tag,
                  (v << 24) | (v << 16) | (v << 8) | 0xFFu);
        if (v >= 255u) {
            halo_rgba(rgba, w, RB_GFX_H, TITLE_LOGO_L, TITLE_TAG_TOP + 2 + dy - 4, TITLE_LOGO_R,
                      TITLE_TAG_TOP + 2 + dy + 12 + 3, gfx_glow_level(g_clock));
        }
    }
}

/* Gaiji letters on a text row, as ASCII (left halves only; anything
 * else reads as a space). */
static void text_row_ascii(const uint8_t *text, int row, char *out)
{
    int col;

    for (col = 0; col < 80; col++) {
        unsigned off = (unsigned)row * 0xA0u + (unsigned)col * 2u;
        uint8_t b0 = text[off];
        uint8_t b1 = text[off + 1];
        unsigned low = (unsigned)(b0 & 0x7Fu) + 0x20u;
        char c = ' ';

        if ((b0 & 0x80u) == 0 && (low == 0x76u || low == 0x77u) && b1 != 0) {
            unsigned id = ((low << 8) | b1) - 0x7600u;

            if (id >= 0x30u && id < 0x30u + 26u) {
                c = (char)('A' + (id - 0x30u));
            } else if (id >= 0x21u && id < 0x21u + 10u) {
                c = (char)('0' + (id - 0x21u));
            }
        }
        out[col] = c;
    }
    out[80] = 0;
}

/* Last ending card: PRESENTED BY / COMPILE 1996. Two more lines in
 * the same font and color go under the poster, on the black below it. */
static void ending_credit(const RbGfx *gfx, const uint8_t *text,
                          uint32_t *rgba, int w)
{
    static const char *const add[2] = {
        "30TH ANNIVERSARY EDITION", "BY SHMUPSBR 2026"
    };
    char line[81];
    char above[81];
    int row;
    int k;

    if (!text || w < RB_GFX_W) {
        return;
    }
    for (row = 2; row < 19; row++) {
        char *at;
        unsigned off;
        int color;
        uint32_t rgb;

        text_row_ascii(text, row, line);
        at = strstr(line, "C O M P I L E");
        if (!at) {
            continue;
        }
        text_row_ascii(text, row - 2, above);
        if (!strstr(above, "P R E S E N T E D")) {
            continue;
        }
        off = (unsigned)row * 0xA0u + (unsigned)(at - line) * 2u;
        color = (text[0x2000u + off] >> 5) & 7;
        rgb = 0xFFu |
              ((uint32_t)TEXT_RGB[color][0] << 24) |
              ((uint32_t)TEXT_RGB[color][1] << 16) |
              ((uint32_t)TEXT_RGB[color][2] << 8);
        for (k = 0; k < 2; k++) {
            const char *s = add[k];
            int n = (int)strlen(s);
            int x = 320 - n * 8;
            int y = (row + 7 + k * 2) * 16;
            int i;

            for (i = 0; i < n; i++, x += 16) {
                int id = brand_id(s[i]);

                if (id < 0 || !gfx->glyph_on[id]) {
                    continue;
                }
                paint_glyph_half(rgba, w, x, y, gfx->glyph[id], 0, rgb);
                paint_glyph_half(rgba, w, x + 8, y, gfx->glyph[id], 1, rgb);
            }
        }
        return;
    }
}

/* Width of s in credit-font dots at the given letter and word gaps. */
static int card_width_g(const char *s, int gap, int space)
{
    int x = 0;
    int first = 1;

    for (; *s; s++) {
        const BrandGlyph *g = brand_glyph(*s);

        if (!g) {
            x += space;
            first = 1;
            continue;
        }
        x += (first ? 0 : gap) + g->w;
        first = 0;
    }
    return x;
}

/* Credit font text at an integer scale, centered on cx, with the given
 * letter and word gaps (in unscaled dots). */
static void card_line_g(uint32_t *rgba, int w, int cx, int top, int scale,
                        int gap, int space, const char *s, uint32_t rgb)
{
    int x = cx - card_width_g(s, gap, space) * scale / 2;
    int first = 1;

    for (; *s; s++) {
        const BrandGlyph *g = brand_glyph(*s);
        int r;
        int c;

        if (!g) {
            x += space * scale;
            first = 1;
            continue;
        }
        x += (first ? 0 : gap) * scale;
        first = 0;
        for (r = 0; r < 12 * scale; r++) {
            int py = top + r;

            if (py < 0 || py >= RB_GFX_H) {
                continue;
            }
            for (c = 0; c < g->w * scale; c++) {
                int px = x + c;

                if (px >= 0 && px < w && (g->rows[r / scale] & (1u << (c / scale)))) {
                    rgba[(size_t)py * (size_t)w + (size_t)px] = rgb;
                }
            }
        }
        x += g->w * scale;
    }
}

/* Credit font text at an integer scale, centered on cx. */
static void card_line(uint32_t *rgba, int w, int cx, int top, int scale,
                      const char *s, uint32_t rgb)
{
    card_line_g(rgba, w, cx, top, scale, 2, 7, s, rgb);
}

/* A card line set to span [l, r] exactly: the largest whole scale
 * that fits, then the spare dots shared evenly between the letters,
 * as the panel logo's subtitle is. */
static int span_width(const char *s, int scale, int gap, int space)
{
    int x = 0;
    int first = 1;

    for (; *s; s++) {
        const BrandGlyph *g = brand_glyph(*s);

        if (!g) {
            x += space * scale;
            first = 1;
            continue;
        }
        x += ((first ? 0 : gap) + g->w) * scale;
        first = 0;
    }
    return x;
}

static void card_line_span(uint32_t *rgba, int w, int l, int rr, int top,
                           const char *s, uint32_t rgb, int maxscale)
{
    /* Double size when it fits (tightening letter and word gaps to one
     * and four dots if needed), else single size; the spare dots are
     * then shared evenly between the letters. */
    int n = (int)strlen(s);
    int avail = rr - l + 1;
    int scale = 2;
    int gap = 2;
    int space = 7;
    int spare;
    int x = l;
    int first = 1;
    int k;

    if (maxscale < 2) {
        scale = 1;
    } else if (span_width(s, 2, 2, 7) > avail) {
        gap = 1;
        space = 4;
        if (span_width(s, 2, 1, 4) > avail) {
            scale = 1;
            gap = 2;
            space = 7;
        }
    }
    spare = avail - span_width(s, scale, gap, space);
    if (spare < 0) spare = 0;
    for (k = 0; k < n; k++) {
        const BrandGlyph *g = brand_glyph(s[k]);
        int r;
        int c;

        if (k > 0 && n > 1) {
            x += spare * k / (n - 1) - spare * (k - 1) / (n - 1);
        }
        if (!g) {
            x += space * scale;
            first = 1;
            continue;
        }
        x += (first ? 0 : gap) * scale;
        first = 0;
        for (r = 0; r < 12 * scale; r++) {
            int py = top + r;

            if (py < 0 || py >= RB_GFX_H) {
                continue;
            }
            for (c = 0; c < g->w * scale; c++) {
                int px = x + c;

                if (px >= 0 && px < w && (g->rows[r / scale] & (1u << (c / scale)))) {
                    rgba[(size_t)py * (size_t)w + (size_t)px] = rgb;
                }
            }
        }
        x += g->w * scale;
    }
}

static uint32_t card_rgb(int r, int g, int b, int level)
{
    return 0xFFu | ((uint32_t)(r * level / 16) << 24) |
           ((uint32_t)(g * level / 16) << 16) | ((uint32_t)(b * level / 16) << 8);
}

static uint32_t hash32(uint32_t x)
{
    x ^= x >> 16; x *= 0x7feb352du; x ^= x >> 15; x *= 0x846ca68bu; x ^= x >> 16;
    return x;
}

static void card_px(uint32_t *rgba, int w, int h, int x, int y, int r, int g, int b)
{
    if (x >= 0 && y >= 0 && x < w && y < h) {
        rgba[y * w + x] = 0xFFu | ((uint32_t)(r > 255 ? 255 : r) << 24) |
                          ((uint32_t)(g > 255 ? 255 : g) << 16) |
                          ((uint32_t)(b > 255 ? 255 : b) << 8);
    }
}

/* 30th AE card shown before the Compile logo, silent. The real title
 * logo (from SGM1.DAT, 640x400 title frame; NULL falls back to a text
 * line) is centred at y 186. It forms out of 2x2 particles that fly in
 * from all sides, hot white cooling to the logo's own colours as they
 * land; a soft flash marks the moment it is whole. It holds, then
 * "30TH ANNIVERSARY EDITION" and the credit line slide up from below,
 * one after the other, and the card fades out. */
static int ease_out(int num, int den)
{
    /* 0..256, quadratic ease-out. */
    int u;

    if (num <= 0) return 0;
    if (num >= den) return 256;
    u = 256 - num * 256 / den;
    return 256 - u * u / 256;
}

void gfx_intro_card(uint32_t *rgba, int w, int h, int t, const uint32_t *title)
{
    const int sx0 = 45, sy0 = 118, sw = 553, sh = 106;
    const int dw = 420, dh = sh * 420 / sw;
    const int lx = 320 - dw / 2, ly = 186 - dh / 2;
    const int t_sub1 = CARD_FORM + CARD_HOLD1;
    const int t_sub2 = t_sub1 + CARD_SLIDE;
    const int t_out = t_sub2 + CARD_SLIDE + CARD_HOLD2;
    int level = 16;
    int i;
    int j;

    if (!rgba || w < RB_GFX_W || h < RB_GFX_H) {
        return;
    }
    for (i = 0; i < w * h; i++) {
        rgba[i] = 0x000000FFu;
    }
    if (t < 0) {
        return;
    }
    if (t >= t_out) {
        level = 16 - (t - t_out) * 16 / CARD_OUT;
        if (level <= 0) {
            return;
        }
    }
    if (!title) {
        card_line(rgba, w, 320, 186 - 46, 2, "RUDE BREAKER",
                  card_rgb(231, 92, 88, t < CARD_FORM ? t * 16 / CARD_FORM : level));
    } else if (t >= CARD_FORM) {
        int flash = t < CARD_FORM + 8 ? (CARD_FORM + 8 - t) * 14 : 0;

        for (j = 0; j < dh; j++) {
            for (i = 0; i < dw; i++) {
                uint32_t p = title[(sy0 + j * sh / dh) * 640 + sx0 + i * sw / dw];
                int r = (int)(p >> 24), g = (int)((p >> 16) & 255), b = (int)((p >> 8) & 255);

                if (r + g + b > 60) {
                    card_px(rgba, w, h, lx + i, ly + j, (r + flash) * level / 16,
                            (g + flash) * level / 16, (b + flash) * level / 16);
                }
            }
        }
    } else {
        for (j = 0; j < dh; j += 2) {
            for (i = 0; i < dw; i += 2) {
                uint32_t p = title[(sy0 + j * sh / dh) * 640 + sx0 + i * sw / dw];
                int r = (int)(p >> 24), g = (int)((p >> 16) & 255), b = (int)((p >> 8) & 255);
                uint32_t hsh;
                int start, dur, e, x, y, sxp, syp, heat, ang;

                if (r + g + b <= 60) {
                    continue;
                }
                hsh = hash32((uint32_t)(j * 1024 + i));
                /* Each particle leaves at its own time and lands by
                 * CARD_FORM - 2; the far ones start first. */
                start = (int)(hsh % 14u);
                dur = CARD_FORM - 2 - start;
                if (t < start) {
                    continue;
                }
                e = ease_out(t - start, dur);
                /* Start point: out along the ray from the centre, pushed
                 * past the screen edge, with a little sideways scatter. */
                ang = (int)((hsh >> 8) % 61u) - 30;
                sxp = 320 + ((lx + i) - 320) * (5 + (int)((hsh >> 16) % 4u)) + ang * 6;
                syp = 186 + ((ly + j) - 186) * (7 + (int)((hsh >> 20) % 5u)) - ang * 3;
                x = sxp + ((lx + i) - sxp) * e / 256;
                y = syp + ((ly + j) - syp) * e / 256;
                /* Hot while flying, the logo's colour on landing. */
                heat = 256 - e;
                r = r + (255 - r) * heat / 256;
                g = g + (230 - g) * heat / 256;
                b = b + (180 - b) * heat / 256;
                if (e < 64) {
                    r = r * (e + 64) / 128;
                    g = g * (e + 64) / 128;
                    b = b * (e + 64) / 128;
                }
                card_px(rgba, w, h, x, y, r, g, b);
                card_px(rgba, w, h, x + 1, y, r, g, b);
                card_px(rgba, w, h, x, y + 1, r, g, b);
                card_px(rgba, w, h, x + 1, y + 1, r, g, b);
            }
        }
    }
    /* Credit lines slide up from below the screen, one after the other:
     * 30TH ANNIVERSARY EDITION in white across the logo's exact width
     * (as on the panel logo), then the credit line under it. */
    {
        int k;
        int il = lx;
        int ir = lx + dw - 1;

        if (title) {
            int found = 0;

            for (i = 0; i < dw && !found; i++) {
                for (j = 0; j < dh; j++) {
                    uint32_t p = title[(sy0 + j * sh / dh) * 640 + sx0 + i * sw / dw];

                    if ((int)(p >> 24) + (int)((p >> 16) & 255) + (int)((p >> 8) & 255) > 60) {
                        il = lx + i;
                        found = 1;
                        break;
                    }
                }
            }
            found = 0;
            for (i = dw - 1; i >= 0 && !found; i--) {
                for (j = 0; j < dh; j++) {
                    uint32_t p = title[(sy0 + j * sh / dh) * 640 + sx0 + i * sw / dw];

                    if ((int)(p >> 24) + (int)((p >> 16) & 255) + (int)((p >> 8) & 255) > 60) {
                        ir = lx + i;
                        found = 1;
                        break;
                    }
                }
            }
        }
        for (k = 0; k < 2; k++) {
            int t0 = k ? t_sub2 : t_sub1;
            int ty = ly + dh + (k ? 56 : 12);
            int e;
            int y;
            int lv;

            if (t < t0) {
                continue;
            }
            e = ease_out(t - t0, CARD_SLIDE);
            y = ty + (h + 20 - ty) * (256 - e) / 256;
            lv = level * (64 + e * 3 / 4) / 256;
            if (lv > 16) lv = 16;
            if (k) {
                card_line(rgba, w, 320, y, 1, "BY SHMUPSBR 2026", card_rgb(204, 204, 255, lv));
            } else {
                card_line_span(rgba, w, il, ir, y, "30TH ANNIVERSARY EDITION",
                               card_rgb(255, 255, 255, lv), 2);
                /* One glow once the credit line has landed. */
                halo_rgba(rgba, w, h, il - 4, y - 4, ir + 4, y + 27,
                          glow_at(t - (t_sub2 + CARD_SLIDE + 8)) * lv / 16);
            }
        }
    }
}

static void overlay_text(const RbGfx *gfx, const uint8_t *text, uint32_t *rgba, int w)
{
    int row;
    int col;
    int hud;

    if (!text) {
        return;
    }
    hud = hud_present(gfx, text);
    for (row = 0; row < 25; row++) {
        if (hud && row == HUD_ROW) {
            hud_row(gfx, text, rgba, w);
            continue;
        }
        for (col = 0; col < 80; col++) {
            unsigned off = (unsigned)row * 0xA0u + (unsigned)col * 2u;
            uint8_t b0;
            uint8_t b1;
            unsigned low;
            uint16_t code;
            unsigned id;
            int color;
            uint32_t rgb;

            b0 = text[off];
            b1 = text[off + 1];
            if (b1 == 0 && (b0 == 0 || b0 == 0x20u)) {
                continue;
            }
            low = (unsigned)(b0 & 0x7Fu) + 0x20u;
            if (!((low == 0x76u || low == 0x77u) && b1 != 0)) {
                continue;
            }
            code = (uint16_t)((low << 8) | b1);
            id = (unsigned)code - 0x7600u;
            if (id >= 512u || !gfx->glyph_on[id]) {
                continue;
            }
            color = (text[0x2000u + off] >> 5) & 7;
            rgb = 0xFFu |
                  ((uint32_t)TEXT_RGB[color][0] << 24) |
                  ((uint32_t)TEXT_RGB[color][1] << 16) |
                  ((uint32_t)TEXT_RGB[color][2] << 8);
            paint_glyph_half(rgba, w, col * 8, row * 16, gfx->glyph[id],
                             (b0 & 0x80u) != 0, rgb);
        }
    }
    overlay_english(gfx, text, rgba, w);
}

static int sad_dist(int scroll, int src)
{
    int dist = scroll - src;

    if (dist < 0) {
        dist += RB_GFX_H;
    }
    return dist;
}

static int row_in_tail(int scroll, int row)
{
    int line;
    int base = row * 16;

    for (line = 0; line < 16; line++) {
        int dist = sad_dist(scroll, base + line);

        if (dist >= 1 && dist <= SCROLL_TAIL) {
            return 1;
        }
    }
    return 0;
}


static int g_panel[4];

void gfx_set_panel(int on, int x0, int y0, int x1, int y1)
{
    g_panel_on = on;
    g_panel[0] = x0 < 0 ? 0 : x0;
    g_panel[1] = y0 < 0 ? 0 : y0;
    g_panel[2] = x1 > RB_GFX_W ? RB_GFX_W : x1;
    g_panel[3] = y1 > RB_GFX_H ? RB_GFX_H : y1;
}

static void dim_panel(uint32_t *rgba, int w)
{
    int x;
    int y;

    if (!g_panel_on) {
        return;
    }
    for (y = g_panel[1]; y < g_panel[3]; y++) {
        for (x = g_panel[0]; x < g_panel[2]; x++) {
            uint32_t p = rgba[(size_t)y * (size_t)w + (size_t)x];
            uint32_t r = (p >> 24) & 0xFFu;
            uint32_t g = (p >> 16) & 0xFFu;
            uint32_t b = (p >> 8) & 0xFFu;

            /* About 18 percent, with a slight blue so it is not flat. */
            r = (r * 46u) >> 8;
            g = (g * 46u) >> 8;
            b = ((b * 46u) >> 8) + 6u;
            rgba[(size_t)y * (size_t)w + (size_t)x] =
                (r << 24) | (g << 16) | (b << 8) | 0xFFu;
        }
    }
}

int gfx_title_frame(void)
{
    return g_title_frame;
}

int gfx_sprite_rgba(const RbGfx *gfx, uint8_t slot, int index, uint32_t out[256])
{
    const uint8_t *src;
    int scene;
    int y;
    int x;

    if (!gfx || !gfx->bank[slot] || index < 0 ||
        (uint32_t)(index + 1) * 128u > gfx->blen[slot]) {
        return 0;
    }
    src = gfx->bank[slot] + (size_t)index * 128u;
    scene = g_modern ? grade_scene(gfx) : -1;
    for (y = 0; y < 16; y++) {
        for (x = 0; x < 16; x++) {
            int byte = y * 2 + (x >> 3);
            int bit = 7 - (x & 7);
            int idx = 8;
            int lv[3];
            int rgb[3];
            int ch;

            if (!((src[byte] >> bit) & 1)) {
                out[y * 16 + x] = 0;
                continue;
            }
            idx |= ((src[32 + byte] >> bit) & 1) | (((src[64 + byte] >> bit) & 1) << 1) |
                   (((src[96 + byte] >> bit) & 1) << 2);
            for (ch = 0; ch < 3; ch++) {
                lv[ch] = dac_level(gfx->pal[idx][ch]);
            }
            expand_index(scene, idx, lv[0], lv[1], lv[2], &rgb[0], &rgb[1], &rgb[2]);
            out[y * 16 + x] = ((uint32_t)rgb[0] << 24) | ((uint32_t)rgb[1] << 16) |
                              ((uint32_t)rgb[2] << 8) | 0xFFu;
        }
    }
    return 1;
}

void gfx_present(RbGfx *gfx, int scroll76, int scroll_speed,
                 const uint8_t *text, uint32_t *rgba, int w, int h,
                 int blank_sides)
{
    int y;
    int scroll;

    if (!gfx || !rgba || w < RB_GFX_W || h < RB_GFX_H) {
        return;
    }
    /* 0x0FB9. On a stage, partition 1 starts at
     * SAD = wrap([bp+0x76] + [bp+0x84] - 1) and runs to the end of the
     * 32000-byte page. Partition 2 continues from address 0. The logo
     * and title leave both scroll words at the init zero, where that
     * formula wraps to line 399 and rotates the frame by one row.
     * Those screens are not a scrolling field, so they start at 0. */
    if (blank_sides) {
        scroll = scroll76 + scroll_speed - 1;
        while (scroll < 0) {
            scroll += RB_GFX_H;
        }
        while (scroll >= RB_GFX_H) {
            scroll -= RB_GFX_H;
        }
    } else {
        scroll = scroll76;
        if (scroll < 0) {
            scroll = 0;
        }
        while (scroll >= RB_GFX_H) {
            scroll -= RB_GFX_H;
        }
    }
    /* One scene for the whole frame. The grade scales each palette
     * index from that scene's settled 4-bit color, so a fade still
     * tracks the script. Original mode leaves scene at -1. */
    {
    int scene = g_modern ? grade_scene(gfx) : -1;

    for (y = 0; y < RB_GFX_H; y++) {
        int src = y + scroll;
        int x;
        int tail = 0;

        if (src >= RB_GFX_H) {
            src -= RB_GFX_H;
        }
        /* Partition 2 continues from line 0, so a GVRAM line just
         * before SAD is the bottom row. A held row still has the cell
         * from before this slot was replaced. Sprite pixels differ
         * from the background page and stay. */
        if (blank_sides && gfx->hold && gfx->hold_on[src >> 4]) {
            int dist = sad_dist(scroll, src);

            if (dist >= 1 && dist <= SCROLL_TAIL) {
                tail = 1;
            }
        }
        for (x = 0; x < RB_GFX_W; x++) {
            uint8_t idx;
            int ch;
            uint32_t px = 0xFFu;
            size_t at;

            /* Stage captures keep the 80 px side columns pure black.
             * GVRAM is 640 wide and index 0 is the stage color, so those
             * columns would otherwise show the background. The logo,
             * intro, title, and menus are drawn across the full width,
             * so the blanking stays off there. Text is painted after. */
            if (blank_sides && (x < 80 || x >= 560)) {
                rgba[(size_t)y * (size_t)w + (size_t)x] = 0x000000FFu;
                continue;
            }
            at = (size_t)src * RB_GFX_W + (size_t)x;
            idx = gfx->pix[at];
            if (tail && idx == gfx->bg[at]) {
                idx = gfx->hold[at];
            }
            {
                int lv[3];
                int rgb[3];

                for (ch = 0; ch < 3; ch++) {
                    lv[ch] = dac_level(gfx->pal[idx][ch]);
                }
                /* pal[][0] is red, [1] green, [2] blue, matching ports AC/AA/AE.
                 * The grade, when on, replaces level*17 after the DAC sample. */
                expand_index(scene, (int)idx, lv[0], lv[1], lv[2],
                             &rgb[0], &rgb[1], &rgb[2]);
                if (g_bullet && gfx->tint[at] && gfx->pix[at] == idx) {
                    /* Luma of the original colour picks dark, main or
                     * highlight, so cores stay bright and rims dark. */
                    int lum = (rgb[0] * 3 + rgb[1] * 6 + rgb[2]) / 10;
                    const uint8_t *c = k_bullet[g_bullet][lum < 110 ? 1 : lum < 215 ? 0 : 2];

                    rgb[0] = c[0];
                    rgb[1] = c[1];
                    rgb[2] = c[2];
                }
                px |= (uint32_t)rgb[0] << 24;
                px |= (uint32_t)rgb[1] << 16;
                px |= (uint32_t)rgb[2] << 8;
            }
            rgba[(size_t)y * (size_t)w + (size_t)x] = px;
        }
    }
    }
    if (blank_sides) {
        int row;

        for (row = 0; row < MAP_ROWS; row++) {
            if (gfx->hold_on[row] && !row_in_tail(scroll, row)) {
                gfx->hold_on[row] = 0;
            }
        }
    }
    dim_panel(rgba, w);
    overlay_text(gfx, text, rgba, w);
    /* The ranking table is text drawn over the same title art; it keeps
     * the original look, so the 30th AE credits are left off there. */
    g_title_frame = 0;
    if (!text_besides_push(text)) {
        title_brand(gfx, rgba, w, 0);
    }
    ending_credit(gfx, text, rgba, w);
}
