/* Time the host composite at 1080p and 4K. No window, no game data. */
#define _POSIX_C_SOURCE 200809L
#include "panels.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static double ms_now(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1.0e6;
}

static double bench(int w, int h, int reps, RbPanelGeom *g)
{
    uint32_t *out;
    uint32_t *game;
    RbPanelData d;
    int i;
    double t0;
    double t1;

    out = (uint32_t *)malloc((size_t)w * (size_t)h * 4u);
    game = (uint32_t *)malloc(640u * 400u * 4u);
    if (!out || !game) {
        fprintf(stderr, "oom %dx%d\n", w, h);
        free(out);
        free(game);
        return -1.0;
    }
    memset(game, 0x20, 640u * 400u * 4u);
    memset(&d, 0, sizeof d);
    d.weapon = 'V';
    d.weapon_lv = 1;
    d.shot_lv = 1;
    d.lives = 3;
    d.medal_spots = 8;
    d.speed = 1;
    d.rank_on = 1;
    d.difficulty = 2;
    panels_set_aspect(1);
    /* Warm the reference render and the output buffer. */
    panels_compose(out, w, h, 0, game, &d, g);
    t0 = ms_now();
    for (i = 0; i < reps; i++) {
        panels_compose(out, w, h, 0, game, &d, g);
    }
    t1 = ms_now();
    free(out);
    free(game);
    return (t1 - t0) / (double)reps;
}

int main(void)
{
    RbPanelGeom a, b;
    double m1080;
    double m2160;
    int i;
    int min_a = 0;
    int min_b = 0;

    m1080 = bench(1920, 1080, 8, &a);
    m2160 = bench(3840, 2160, 4, &b);
    printf("compose 1920x1080  %.2f ms  boxes=%d field=%dx%d\n", m1080, a.n, a.field_w,
           a.field_h);
    printf("compose 3840x2160  %.2f ms  boxes=%d field=%dx%d\n", m2160, b.n, b.field_w,
           b.field_h);
    if (a.n > 0 && b.n > 0) {
        printf("widget0 1080 %d,%d %dx%d\n", a.x[0], a.y[0], a.w[0], a.h[0]);
        printf("widget0 2160 %d,%d %dx%d\n", b.x[0], b.y[0], b.w[0], b.h[0]);
    }
    for (i = 0; i < a.n; i++) {
        if (a.h[i] < a.h[min_a]) {
            min_a = i;
        }
    }
    for (i = 0; i < b.n; i++) {
        if (b.h[i] < b.h[min_b]) {
            min_b = i;
        }
    }
    if (a.n > 0 && b.n > 0) {
        printf("smallest widget 1080 h=%d  2160 h=%d  ratio=%.3f\n", a.h[min_a], b.h[min_b],
               a.h[min_a] ? (double)b.h[min_b] / (double)a.h[min_a] : 0.0);
        printf("field ratio w=%.3f h=%.3f\n",
               a.field_w ? (double)b.field_w / (double)a.field_w : 0.0,
               a.field_h ? (double)b.field_h / (double)a.field_h : 0.0);
    }
    printf("logic budget 30.67 Hz = %.2f ms\n", 1000.0 / (92.0 / 3.0));
    return m1080 < 0.0 || m2160 < 0.0 ? 1 : 0;
}
