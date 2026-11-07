#ifndef RB_PANELS_H
#define RB_PANELS_H

#include <stdint.h>

/* Side panels around the playfield for wide (16:9) windows. Drawn in
 * software into a 1920x1080 RGBA frame (0xRRGGBBAA). All art comes from
 * the running game: the title screen and the sprite sheets read from the
 * player's own SGM1.DAT. */

#define RB_PANEL_W 1920
#define RB_PANEL_H 1080

typedef struct RbPanelData {
    int training;
    int rank_on;            /* rank option (OFF shows the value dimmed) */
    int rank_now;           /* 0..159 */
    int rank_adv;           /* RANK MODE ADVANCED: bar, no numbers */
    int rank_bar;           /* ADVANCED: progress to the next LEVEL band, 0..1000 */
    const uint32_t *ship;   /* 32x48 player ship, or NULL */
    int rank_mul;           /* rise multiplier, percent */
    const uint8_t *rank_hist;  /* one sample per second */
    int rank_n;
    const uint16_t *deaths; /* seconds */
    int death_n;
    const uint16_t *ups;    /* difficulty step-ups, seconds */
    int up_n;
    char weapon;            /* 'V', 'L', 'M' or 0 */
    int weapon_lv;          /* sub-weapon level 1..3 (global D6 + 1) */
    int sub_on;             /* 1 while a sub-weapon is carried (global D5) */
    int shot_items;         /* power-up items toward the shot (global DE) */
    int shot_lv;            /* main shot level 1..4 (global D9 + 1) */
    uint32_t frame;         /* logic frame counter, for the speed dash */
    const uint32_t *capsule; /* 32x32 RGBA or NULL */
    int lives;
    int iframe;             /* invulnerability frames left (global DF) */
    int iframe_max;         /* its value when granted */
    int iframe_respawn;     /* 1 if granted by a respawn */
    int run_deaths;         /* lives lost this run (NO DEATH) */
    int loop_on;            /* LOOP option on: LOOP n by RANK from loop 2 */
    int loop;               /* LOOP number, 1 = first pass (hidden) */
    int speed;              /* 0..2 */
    uint32_t score;
    uint32_t hiscore;
    int stage;
    int foes;               /* enemies appeared this stage */
    int foes_hit;           /* of those, destroyed */
    int foes_total;         /* fixed enemy count of the stage, 0 if unknown */
    const uint32_t *medal;  /* 32x32 RGBA icon or NULL */
    int medals;             /* found */
    int medal_spots;        /* passed; -1 hides the box */
    int difficulty;         /* 0..4 */
    int level_flash;        /* frames left of the step-up flash, of 61 */
    int level_pct;          /* VERY HARD extra bullet speed, 0 hides */
    uint32_t time_cs;       /* match time, centiseconds */
    uint16_t buttons;
} RbPanelData;

/* Keep a copy of the settled title screen (640x400) for the logo and
 * the metal side art. */
void panels_capture_title(const uint32_t *rgba640);
int  panels_have_title(void);
const uint32_t *panels_title_pixels(void);
/* Composite the playfield (640x400 game frame) and the panels. */
/* ASPECT for the playfield: 1 = 4:3 (default), 0 = 1:1. */
void panels_set_aspect(int a43);
void panels_render(uint32_t *out, const uint32_t *game640, const RbPanelData *d);

/* TATE: recompose the 1920x1080 frame panels_render just drew (and the
 * game frame) into a 1080x1920 portrait frame. */
#define RB_TATE_W 1080
#define RB_TATE_H 1920
void panels_render_tate(uint32_t *out, const uint32_t *wide, const uint32_t *game640);

/* Where the playfield and the scaled widgets landed in a compose. */
typedef struct RbPanelGeom {
    int field_x, field_y, field_w, field_h;
    int widgets; /* 1 when the side (or tate top/bottom) widgets are drawn */
    int n;
    int x[40], y[40], w[40], h[40];
} RbPanelGeom;

/* Host composite at an arbitrary window size. tate=0 is yoko. The metal
 * plate is sampled at 1:1 and cropped; widgets keep the 1920x1080
 * proportions inside the side area. On a window narrower than 16:10 the
 * bars around the 4:3 picture are that plate only: no widgets, no text.
 * out is w*h pixels, 0xRRGGBBAA. */
void panels_compose(uint32_t *out, int w, int h, int tate,
                    const uint32_t *game640, const RbPanelData *d,
                    RbPanelGeom *geom);

/* The grey metal plate at window pixel (x, y), 1:1, 0xRRGGBBAA. */
uint32_t panels_plate_at(int x, int y);

/* Size to rasterise. A 16:9 output uses the 1920x1080 reference and
 * the GPU scales that picture, so 1080p and 4K are the same layout.
 * 21:9, 16:10 and 4:3 stay at the output size. */
void panels_output_size(int dw, int dh, int *cw, int *ch);

#endif
