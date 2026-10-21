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

struct RbGfx {
    uint8_t *pix;
    uint16_t map[MAP_N];
    const uint8_t *bank[256];
    uint32_t blen[256];
    int16_t pal[16][3];
    int16_t dpal[16][3];
    int active;
};

RbGfx *gfx_create(void)
{
    RbGfx *gfx = (RbGfx *)calloc(1, sizeof(*gfx));

    if (!gfx) {
        return NULL;
    }
    gfx->pix = (uint8_t *)calloc((size_t)RB_GFX_W * (size_t)RB_GFX_H, 1);
    if (!gfx->pix) {
        free(gfx);
        return NULL;
    }
    return gfx;
}

void gfx_destroy(RbGfx *gfx)
{
    if (!gfx) {
        return;
    }
    free(gfx->pix);
    free(gfx);
}

void gfx_reset(RbGfx *gfx)
{
    if (!gfx) {
        return;
    }
    memset(gfx->pix, 0, (size_t)RB_GFX_W * (size_t)RB_GFX_H);
    memset(gfx->map, 0, sizeof gfx->map);
    memset(gfx->bank, 0, sizeof gfx->bank);
    memset(gfx->blen, 0, sizeof gfx->blen);
    memset(gfx->pal, 0, sizeof gfx->pal);
    memset(gfx->dpal, 0, sizeof gfx->dpal);
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

static void put_idx(RbGfx *gfx, int x, int y, uint8_t idx)
{
    if ((unsigned)x >= (unsigned)RB_GFX_W || (unsigned)y >= (unsigned)RB_GFX_H) {
        return;
    }
    gfx->pix[(size_t)y * (size_t)RB_GFX_W + (size_t)x] = idx;
}

static int wrap_y(int y)
{
    while (y < 0) {
        y += RB_GFX_H;
    }
    while (y >= RB_GFX_H) {
        y -= RB_GFX_H;
    }
    return y;
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
        uint32_t off = (uint32_t)di + (uint32_t)row * 80u;
        uint16_t b;

        off %= 32000u;
        for (b = 0; b < nbytes; b++) {
            uint32_t at = (off + b) % 32000u;
            int x = (int)(at % 80u) * 8;
            int y = (int)(at / 80u);
            int p;

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
            put_idx(gfx, x + col, wrap_y(y + row), idx);
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
    for (row = 0; row < 16; row++) {
        uint8_t mask[24];
        uint8_t b[24];
        uint8_t r[24];
        uint8_t g[24];
        int col;
        int py = wrap_y(y + row);

        decode_plane(src + row * 2, shift, mask);
        if (!mono) {
            decode_plane(src + 32 + row * 2, shift, b);
            decode_plane(src + 64 + row * 2, shift, r);
            decode_plane(src + 96 + row * 2, shift, g);
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
            put_idx(gfx, x + col, py, idx);
        }
    }
    mark_dirty(gfx, x, wrap_y(y), wide);
}

void gfx_draw_entities(RbGfx *gfx, const uint8_t *ent, int ent_n, int stride,
                       const uint8_t *const *chunk, const uint32_t *clen,
                       const uint8_t *cof, int scroll76)
{
    int i;

    if (!gfx || !ent || !chunk || !clen || !cof) {
        return;
    }
    for (i = 0; i < ent_n; i++) {
        const uint8_t *slot = ent + (size_t)i * (size_t)stride;
        uint16_t type = (uint16_t)slot[0] | ((uint16_t)slot[1] << 8);
        int16_t x;
        int16_t y;
        int wcells;
        int hcells;
        int sy;
        uint16_t pat;
        uint16_t seg;
        uint8_t shift;
        uint8_t attr;
        const uint8_t *script;
        uint32_t slen;
        const uint8_t *sheet;
        int row;

        if (type == 0 || (type & 0x0200u) != 0u) {
            continue;
        }
        wcells = slot[0x4C];
        hcells = slot[0x4D];
        if (wcells == 0 || hcells == 0) {
            continue;
        }
        if (wcells > 8) {
            wcells = 8;
        }
        if (hcells > 8) {
            hcells = 8;
        }
        x = (int16_t)((uint16_t)slot[0x22] | ((uint16_t)slot[0x23] << 8));
        y = (int16_t)((uint16_t)slot[0x24] | ((uint16_t)slot[0x25] << 8));
        x = (int16_t)(x - (int16_t)(wcells << 3));
        y = (int16_t)(y - (int16_t)(hcells << 3));
        sy = (int)y + scroll76;
        if (sy >= 0x190) {
            sy -= 0x190;
        }
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
            int ycell = (int)(y >> 4) + row;
            int draw = (ycell >= 0 && ycell < 0x18);
            int col;

            for (col = 0; col < wcells; col++) {
                int xcell = (int)(x >> 4) + col;
                unsigned poff = (unsigned)pat + (unsigned)row * (unsigned)wcells +
                                (unsigned)col;
                uint8_t index;
                uint32_t off;
                int px;
                int py;

                if (poff >= slen) {
                    continue;
                }
                index = script[poff];
                if (!draw || xcell < 4 || xcell >= 0x23) {
                    continue;
                }
                off = pattern_off(index);
                if (off >= gfx->blen[seg]) {
                    continue;
                }
                px = ((int)(x >> 3) + col * 2) * 8;
                py = sy + row * 16;
                blit_sprite(gfx, sheet + off, gfx->blen[seg] - off, px, py,
                            (int)shift, attr);
            }
        }
    }
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

int gfx_active(const RbGfx *gfx)
{
    return gfx && gfx->active;
}

void gfx_present(const RbGfx *gfx, int scroll76, uint32_t *rgba, int w, int h)
{
    int y;
    int scroll;

    if (!gfx || !rgba || w < RB_GFX_W || h < RB_GFX_H) {
        return;
    }
    scroll = scroll76;
    if (scroll < 0) {
        scroll = 0;
    }
    scroll %= RB_GFX_H;
    for (y = 0; y < RB_GFX_H; y++) {
        int src = y + scroll;
        int x;

        if (src >= RB_GFX_H) {
            src -= RB_GFX_H;
        }
        for (x = 0; x < RB_GFX_W; x++) {
            uint8_t idx = gfx->pix[(size_t)src * RB_GFX_W + (size_t)x];
            int ch;
            uint32_t px = 0xFFu;

            for (ch = 0; ch < 3; ch++) {
                int v = gfx->pal[idx][ch] >> 4;

                if (v < 0) {
                    v = 0;
                }
                if (v > 15) {
                    v = 15;
                }
                px |= (uint32_t)(v * 17) << (8 + (2 - ch) * 8);
            }
            /* pal[][0] is red, [1] green, [2] blue, matching ports AC/AA/AE. */
            rgba[(size_t)y * (size_t)w + (size_t)x] = px;
        }
    }
}
