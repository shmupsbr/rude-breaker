/* Headless panel composite: several window sizes, no black border,
 * widgets inside the frame. No game data and no window. */
#include "panels.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GW 640
#define GH 400

static uint32_t game[GW * GH];
static int fails;

enum {
    PX_LEFT = 0xE10000FFu,
    PX_PLAY_L = 0x00E100FFu,
    PX_PLAY_R = 0x0000E1FFu,
    PX_HUD = 0xE1E100FFu,
    PX_RIGHT = 0xE100E1FFu
};

static int pure_black(uint32_t p)
{
    return (p >> 8) == 0;
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

static void one(int w, int h, int tate)
{
    uint32_t *out;
    RbPanelData d;
    RbPanelGeom g;
    char label[64];
    int i;
    int black = 0;
    int x;
    int y;

    out = (uint32_t *)calloc((size_t)w * (size_t)h, sizeof(uint32_t));
    if (!out) {
        fprintf(stderr, "oom %dx%d\n", w, h);
        fails++;
        return;
    }
    memset(&d, 0, sizeof d);
    d.weapon = 'V';
    d.weapon_lv = 1;
    d.shot_lv = 1;
    d.sub_on = 1;
    d.lives = 3;
    d.score = 123456;
    d.hiscore = 200000;
    d.stage = 1;
    d.medal_spots = 8;
    d.medals = 2;
    d.difficulty = 2;
    d.rank_on = 1;
    d.time_cs = 1250;
    d.speed = 1;
    panels_set_aspect(1);
    panels_compose(out, w, h, tate, game, &d, &g);
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            int in = x >= g.field_x && x < g.field_x + g.field_w && y >= g.field_y &&
                     y < g.field_y + g.field_h;

            if (!in && pure_black(out[(size_t)y * w + x])) {
                black++;
            }
        }
    }
    snprintf(label, sizeof label, "%dx%d%s no black border", w, h, tate ? " tate" : "");
    expect(label, black == 0);
    snprintf(label, sizeof label, "%dx%d%s field inside", w, h, tate ? " tate" : "");
    expect(label, g.field_w > 0 && g.field_h > 0 && g.field_x >= 0 && g.field_y >= 0 &&
                      g.field_x + g.field_w <= w && g.field_y + g.field_h <= h);
    if (!tate && (long)w * 10 >= (long)h * 16) {
        snprintf(label, sizeof label, "%dx%d widgets on", w, h);
        expect(label, g.widgets && g.n > 0);
    }
    if (!tate && (long)w * 10 < (long)h * 16) {
        int stray = 0;

        snprintf(label, sizeof label, "%dx%d no side widgets", w, h);
        expect(label, !g.widgets && g.n == 0);
        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                int in = x >= g.field_x && x < g.field_x + g.field_w &&
                         y >= g.field_y && y < g.field_y + g.field_h;

                if (!in && out[(size_t)y * w + x] != panels_plate_at(x, y)) {
                    stray++;
                }
            }
        }
        snprintf(label, sizeof label, "%dx%d bars are plate only", w, h);
        expect(label, stray == 0);
    }
    for (i = 0; i < g.n; i++) {
        int ok = g.x[i] >= 0 && g.y[i] >= 0 && g.w[i] > 0 && g.h[i] > 0 &&
                 g.x[i] + g.w[i] <= w && g.y[i] + g.h[i] <= h;

        if (!ok) {
            fprintf(stderr, "  box %d %d,%d %dx%d\n", i, g.x[i], g.y[i], g.w[i], g.h[i]);
        }
        snprintf(label, sizeof label, "%dx%d%s widget %d", w, h, tate ? " tate" : "", i);
        expect(label, ok);
    }
    printf("  field %d,%d %dx%d widgets=%d boxes=%d black=%d\n", g.field_x, g.field_y, g.field_w,
           g.field_h, g.widgets, g.n, black);
    if (!tate && (long)w * 10 >= (long)h * 16) {
        int min_h = g.field_h;
        int max_w = 0;
        int bot = 0;

        for (i = 0; i < g.n; i++) {
            if (g.h[i] < min_h) {
                min_h = g.h[i];
            }
            if (g.w[i] > max_w) {
                max_w = g.w[i];
            }
            if (g.y[i] + g.h[i] > bot) {
                bot = g.y[i] + g.h[i];
            }
        }
        snprintf(label, sizeof label, "%dx%d widgets tall enough", w, h);
        expect(label, g.n >= 12 && min_h >= 24 && bot <= h && bot * 10 >= h * 9);
        /* 16:10 leaves a short strip if the 4:3 picture takes the height.
         * Widgets should stay about as wide as on a 16:9 screen this tall. */
        if ((long)w * 9 < (long)h * 16) {
            snprintf(label, sizeof label, "%dx%d widgets readable", w, h);
            expect(label, max_w >= h / 6);
        }
    }
    if (!tate && (long)w * 10 < (long)h * 16) {
        int seen_l = 0, seen_pl = 0, seen_pr = 0, seen_hud = 0, seen_r = 0;

        for (y = g.field_y; y < g.field_y + g.field_h; y++) {
            for (x = g.field_x; x < g.field_x + g.field_w; x++) {
                uint32_t p = out[(size_t)y * w + x];

                seen_l |= p == PX_LEFT;
                seen_pl |= p == PX_PLAY_L;
                seen_pr |= p == PX_PLAY_R;
                seen_hud |= p == PX_HUD;
                seen_r |= p == PX_RIGHT;
            }
        }
        snprintf(label, sizeof label, "%dx%d full frame not clipped", w, h);
        expect(label, seen_l && seen_pl && seen_pr && seen_hud && seen_r);
    }
    free(out);
}

int main(void)
{
    int i;

    for (i = 0; i < GW * GH; i++) {
        game[i] = 0x203040FFu;
    }
    game[0] = PX_LEFT;
    game[80] = PX_PLAY_L;
    game[559] = PX_PLAY_R;
    game[560] = PX_HUD;
    game[639] = PX_RIGHT;
    one(1920, 1080, 0);
    one(1920, 1200, 0);
    one(1680, 1050, 0);
    one(1024, 768, 0);  /* 4:3: no panels, full frame */
    one(1280, 1024, 0); /* 5:4: no panels, bars are plate */
    one(1600, 1200, 0); /* 4:3: no panels, picture fills the frame */
    one(2560, 1080, 0);
    one(3440, 1440, 0);
    one(1080, 1920, 1);
    if (fails) {
        fprintf(stderr, "%d failed\n", fails);
        return 1;
    }
    printf("panels_fit: ok\n");
    return 0;
}
