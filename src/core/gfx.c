/**
 * Software GVRAM. Plane order follows the tile blitter at 0x4B21
 * (A800, B000, then the green plane, intensity cleared) and the sprite
 * blitter at 0x481B (mask, then blue, red, green through GRCG).
 */
#include "gfx.h"

#include <stdlib.h>
#include <string.h>

#define MAP_COLS 30
#define MAP_ROWS 25
#define MAP_N    (MAP_COLS * MAP_ROWS)
/* A replaced 16-line row at intro speed 15 sits up to 29 lines before
 * the GDC start address. Those lines are the bottom of the picture. */
#define SCROLL_TAIL 32

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
 * A line that is already on screen keeps the GVRAM address 0x1132
 * used (world Y plus the scroll, wrapped into the page). The clip is
 * the line present will show. A stage starts at SAD = scroll + speed
 * - 1, so that line is world - speed + 1. The title starts at
 * scroll76, so the line is the world Y itself. */
static void blit_sprite_smooth(RbGfx *gfx, const uint8_t *src, uint32_t len,
                               int px, int shift, uint8_t attr, int world_y,
                               int scroll76, int speed, int blank_sides)
{
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
        int screen = blank_sides ? (world - speed + 1) : world;
        int gv;
        int drew = 0;

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
    gfx->paint_bg = 0;
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
    }
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

static void overlay_text(const RbGfx *gfx, const uint8_t *text, uint32_t *rgba, int w)
{
    int row;
    int col;

    if (!text) {
        return;
    }
    for (row = 0; row < 25; row++) {
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
            for (ch = 0; ch < 3; ch++) {
                int v = dac_level(gfx->pal[idx][ch]);

                px |= (uint32_t)(v * 17) << (8 + (2 - ch) * 8);
            }
            /* pal[][0] is red, [1] green, [2] blue, matching ports AC/AA/AE. */
            rgba[(size_t)y * (size_t)w + (size_t)x] = px;
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
    overlay_text(gfx, text, rgba, w);
}
