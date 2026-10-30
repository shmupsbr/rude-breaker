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
    int onecc;              /* 1CC on record */
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
void panels_render(uint32_t *out, const uint32_t *game640, const RbPanelData *d);

#endif
