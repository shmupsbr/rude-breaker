/* Deterministic ZX filter checks. No game data: the frames are built
 * the way the real masks come out of gfx (sprite id, kind, own, shade). */
#include "cfilter.h"

#include <stdio.h>
#include <string.h>

#define W 640
#define H 400
#define N (W * H)

static uint32_t px[N];
static uint16_t sid[N];
static uint8_t bgi[N], kind[N], own[N], shade[N];
static int fails;

static void clear_frame(void)
{
    int i;

    memset(px, 0, sizeof px);
    memset(sid, 0, sizeof sid);
    memset(bgi, 0, sizeof bgi);
    memset(kind, 0, sizeof kind);
    memset(own, 0, sizeof own);
    memset(shade, 0, sizeof shade);
    for (i = 0; i < N; i++) {
        px[i] = 0x000000FFu;
    }
}

static void put(int x, int y, uint32_t rgb, int k, int id, int ow, int sh, int bi)
{
    size_t at;

    if (x < 80 || x >= 560 || y < 0 || y >= H) {
        return;
    }
    at = (size_t)y * W + (size_t)x;
    px[at] = (rgb & 0xFFFFFF00u) | 0xFFu;
    kind[at] = (uint8_t)k;
    sid[at] = (uint16_t)id;
    own[at] = (uint8_t)ow;
    shade[at] = (uint8_t)sh;
    bgi[at] = (uint8_t)bi;
}

static void fill(int x0, int y0, int x1, int y1, uint32_t rgb, int k, int id, int ow, int sh, int bi)
{
    int x, y;

    for (y = y0; y < y1; y++) {
        for (x = x0; x < x1; x++) {
            put(x, y, rgb, k, id, ow, sh, bi);
        }
    }
}

static uint32_t at(int x, int y)
{
    return px[(size_t)y * W + (size_t)x] | 0xFFu;
}

static void expect(const char *name, int cond)
{
    if (!cond) {
        fprintf(stderr, "FAIL %s\n", name);
        fails++;
    } else {
        printf("ok %s\n", name);
    }
}

static void apply(int stage, int shot)
{
    cfilter_set_world(sid, bgi, 0);
    cfilter_set_kind(kind);
    cfilter_set_text(NULL, NULL);
    cfilter_set_layer(NULL);
    cfilter_set_stage(stage);
    cfilter_set_shade(shade);
    cfilter_set_player(own, NULL, 10, shot);
    cfilter_apply(px, W, H, CF_ZX);
}

int main(void)
{
    int x, y, white = 0, blue = 0, green = 0, other = 0;
    uint32_t ship_hi, ship_mid, ship_lo, tip, opt_hi, opt_lo;

    /* Stage 2: two real large sprites. Scan order is top first. */
    clear_frame();
    fill(120, 16, 200, 96, 0xF0F0F000u, 1, 1001, 0, 240, 0);
    fill(120, 140, 200, 220, 0xF0F0F000u, 1, 1002, 0, 240, 0);
    /* ship: light body, mid grey panel, dark line, 1 px wing tip */
    fill(300, 300, 332, 332, 0xFFFFFF00u, 1, 0xFFF8, 1, 240, 0);
    for (y = 304; y < 328; y++) {
        put(316, y, 0x33333300u, 1, 0xFFF8, 1, 50, 0);
    }
    put(338, 316, 0xC0C0C000u, 1, 0xFFF8, 1, 160, 0);
    for (x = 304; x < 328; x++) {
        put(x, 316, 0x44444400u, 1, 0xFFF8, 1, 40, 0);
    }
    for (x = 280; x < 300; x++) {
        put(x, 316, 0xFFFFFF00u, 1, 0xFFF8, 1, 230, 0);
    }
    /* option pod */
    fill(250, 308, 266, 324, 0xFFFFFF00u, 1, 0xFFF8, 2, 230, 0);
    put(258, 314, 0x22222200u, 1, 0xFFF8, 2, 50, 0);
    put(258, 315, 0x22222200u, 1, 0xFFF8, 2, 50, 0);
    /* a star */
    put(400, 40, 0xFFFFFF00u, 0, 0, 0, 255, 15);
    apply(2, 10);

    expect("first cruiser dark yellow", (at(140, 40) & 0xFFFFFF00u) == 0xD7D70000u);
    expect("second cruiser cyan", (at(140, 160) & 0xFFFFFF00u) == 0x00FFFF00u);
    ship_hi = at(308, 306);
    ship_mid = at(316, 308);
    ship_lo = at(310, 316);
    tip = at(282, 316);
    expect("ship light is white", (ship_hi & 0xFFFFFF00u) == 0xFFFFFF00u);
    expect("ship grey is black", (ship_mid & 0xFFFFFF00u) == 0x00000000u);
    expect("ship dark line is black", (ship_lo & 0xFFFFFF00u) == 0x00000000u);
    expect("side cannon stays white", (at(338, 316) & 0xFFFFFF00u) == 0xFFFFFF00u);
    expect("wing tip stays white", (tip & 0xFFFFFF00u) == 0xFFFFFF00u);
    opt_hi = at(252, 310);
    opt_lo = at(258, 314);
    expect("option light is weapon red", (opt_hi & 0xFFFFFF00u) == 0xFF000000u);
    expect("option detail is black", (opt_lo & 0xFFFFFF00u) == 0x00000000u);
    expect("star stays white", (at(400, 40) & 0xFFFFFF00u) == 0xFFFFFF00u);

    /* Clouds: a light-blue field must dither, not flatten to one cyan. */
    clear_frame();
    fill(80, 0, 112, 32, 0xB4DCE800u, 0, 0, 0, 0, 3);
    apply(3, 10);
    for (y = 2; y < 30; y++) {
        for (x = 82; x < 110; x++) {
            uint32_t c = at(x, y) & 0xFFFFFF00u;

            if (c == 0xFFFFFF00u || c == 0xD7D7D700u) {
                white++;
            } else if (c == 0x00D7D700u || c == 0x00FFFF00u || c == 0x0000D700u || c == 0x0000FF00u) {
                blue++;
            } else {
                other++;
            }
        }
    }
    expect("clouds use white", white > 0);
    expect("clouds use blue or cyan", blue > 0);
    expect("clouds stay in light blue and white", other == 0);
    printf("  clouds white=%d blue=%d other=%d\n", white, blue, other);

    /* Dark green city field on another stage. */
    clear_frame();
    fill(80, 32, 112, 64, 0x1E461E00u, 0, 0, 0, 0, 2);
    apply(4, 10);
    for (y = 36; y < 60; y++) {
        for (x = 84; x < 108; x++) {
            uint32_t c = at(x, y) & 0xFFFFFF00u;

            if (c == 0x00D70000u || c == 0x00000000u) {
                green++;
            }
        }
    }
    expect("city field is dark green", (at(90, 48) & 0xFFFFFF00u) == 0x00D70000u);
    printf("  city greenish=%d sample=%06X\n", green, (at(90, 48) >> 8) & 0xFFFFFF);

    /* A flat sprite is one colour, not a dither mix. */
    clear_frame();
    fill(200, 200, 216, 216, 0xFF202000u, 1, 501, 0, 200, 0);
    apply(4, 9);
    {
        uint32_t s0 = at(204, 204) & 0xFFFFFF00u;
        int same = 1;

        for (y = 202; y < 214; y++) {
            for (x = 202; x < 214; x++) {
                if ((at(x, y) & 0xFFFFFF00u) != s0 && (at(x, y) & 0xFFFFFF00u) != 0x00000000u) {
                    same = 0;
                }
            }
        }
        expect("sprite stays a single colour", same && s0 != 0xFFFFFF00u && s0 != 0x00D7D700u);
        printf("  sprite=%06X\n", (s0 >> 8) & 0xFFFFFF);
    }

    /* Background cruisers: no sprite id, the way a page stamp looks. */
    clear_frame();
    apply(1, 10);
    clear_frame();
    fill(100, 8, 180, 100, 0xB0B0B000u, 0, 0, 0, 180, 1);
    fill(100, 160, 180, 250, 0xB0B0B000u, 0, 0, 0, 180, 1);
    apply(2, 10);
    expect("tile cruiser 1 is dark yellow", (at(120, 40) & 0xFFFFFF00u) == 0xD7D70000u);
    expect("tile cruiser 2 is cyan", (at(120, 190) & 0xFFFFFF00u) == 0x00FFFF00u);

    /* Gappy hull: bright pixels split by black lines must still be flat. */
    clear_frame();
    apply(1, 10);
    clear_frame();
    for (y = 10; y < 90; y++) {
        for (x = 100; x < 180; x++) {
            if ((x + y) & 1) {
                put(x, y, 0xC0404000u, 0, 0, 0, 140, 1);
            }
        }
    }
    apply(2, 10);
    {
        uint32_t c = at(120, 40) & 0xFFFFFF00u;
        int bad = c != 0xD7D70000u && c != 0x00000000u;

        expect("gappy cruiser is flat yellow or black", !bad);
        expect("gappy cruiser paint is yellow", (at(121, 40) & 0xFFFFFF00u) == 0xD7D70000u);
        printf("  gappy=%06X\n", (c >> 8) & 0xFFFFFF);
    }

    /* White sprite on a white cloud must not stay white. */
    clear_frame();
    fill(80, 0, 200, 80, 0xE8F4FF00u, 0, 0, 0, 230, 3);
    fill(120, 24, 136, 40, 0xFFFFFF00u, 1, 777, 0, 250, 3);
    apply(3, 10);
    {
        uint32_t c = at(124, 28) & 0xFFFFFF00u;

        expect("sprite avoids white clouds", c != 0xFFFFFF00u && c != 0xD7D7D700u);
        printf("  cloud-sprite=%06X\n", (c >> 8) & 0xFFFFFF);
    }

    /* Blue-grey scenery must keep a hue, not collapse to black and white. */
    clear_frame();
    fill(80, 64, 112, 96, 0x5A649600u, 0, 0, 0, 0, 4);
    apply(4, 10);
    {
        int hue = 0, grey = 0;

        for (y = 68; y < 92; y++) {
            for (x = 84; x < 108; x++) {
                uint32_t c = at(x, y) & 0xFFFFFF00u;

                if (c == 0x00000000u || c == 0xD7D7D700u || c == 0xFFFFFF00u) {
                    grey++;
                } else {
                    hue++;
                }
            }
        }
        expect("scenery keeps a hue", hue > 0);
        printf("  hue=%d grey=%d sample=%06X\n", hue, grey, (at(90, 80) >> 8) & 0xFFFFFF);
    }

    if (fails) {
        fprintf(stderr, "%d failed\n", fails);
        return 1;
    }
    printf("zx_rules_test: ok\n");
    return 0;
}
