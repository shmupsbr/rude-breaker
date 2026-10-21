/**
 * Rude Breaker — Hardware Abstraction Layer
 *
 * Core logic includes only this header. Phase 1 implements the API with SDL2;
 * Phase 2 implements it with X68000 IOCS / VDP registers.
 *
 * All comments are in English by project rule.
 */
#ifndef RUDEBREAKER_HAL_H
#define RUDEBREAKER_HAL_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- packing / alignment (M68000-safe) ---------------------------------- */

#if defined(__GNUC__) || defined(__clang__)
#define RB_PACKED __attribute__((packed, aligned(2)))
#else
#define RB_PACKED
#endif

/* ---- endian helpers (PC-98 assets are little-endian) -------------------- */

#define SWAP16(x) ((uint16_t)((((uint16_t)(x) & 0x00FFu) << 8) | \
                              (((uint16_t)(x) & 0xFF00u) >> 8)))

#define SWAP32(x) ((uint32_t)((((uint32_t)(x) & 0x000000FFu) << 24) | \
                              (((uint32_t)(x) & 0x0000FF00u) <<  8) | \
                              (((uint32_t)(x) & 0x00FF0000u) >>  8) | \
                              (((uint32_t)(x) & 0xFF000000u) >> 24)))

static inline uint16_t read_u16_le(const uint8_t *p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static inline uint32_t read_u32_le(const uint8_t *p)
{
    return (uint32_t)p[0]
         | ((uint32_t)p[1] << 8)
         | ((uint32_t)p[2] << 16)
         | ((uint32_t)p[3] << 24);
}

static inline void write_u16_le(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)(v & 0xFFu);
    p[1] = (uint8_t)((v >> 8) & 0xFFu);
}

static inline void write_u32_le(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v & 0xFFu);
    p[1] = (uint8_t)((v >> 8) & 0xFFu);
    p[2] = (uint8_t)((v >> 16) & 0xFFu);
    p[3] = (uint8_t)((v >> 24) & 0xFFu);
}

#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define RB_HOST_BE 1
#define RB_FROM_LE16(x) SWAP16(x)
#define RB_FROM_LE32(x) SWAP32(x)
#else
#define RB_HOST_BE 0
#define RB_FROM_LE16(x) ((uint16_t)(x))
#define RB_FROM_LE32(x) ((uint32_t)(x))
#endif

/* ---- display / timing --------------------------------------------------- */

#define RB_NATIVE_W     640
#define RB_NATIVE_H     400
/* One count of the loader's vsync wait ([bp+0x60], usually 3). Three
 * counts match the PC-9821 capture at about 40 logic frames per second
 * (ship 8 px/frame ~320 px/s, cruise scroll 4 px/frame ~160 px/s,
 * player shots 24 px/frame ~960 px/s). Not the host refresh rate. */
#define RB_WAIT_HZ      120
#define RB_TARGET_FPS   RB_WAIT_HZ

/* X68000 Phase-2 outer raster (640x400 centered in 640x480) */
#define RB_X68K_OUTER_H 480
#define RB_X68K_VMARGIN 40

/* ---- input -------------------------------------------------------------- */

typedef struct RB_PACKED RbInput {
    uint16_t buttons;   /* bitfield: see RB_BTN_* */
    int16_t  axis_x;    /* -1, 0, +1 */
    int16_t  axis_y;    /* -1, 0, +1 */
} RbInput;

#define RB_BTN_UP       0x0001u
#define RB_BTN_DOWN     0x0002u
#define RB_BTN_LEFT     0x0004u
#define RB_BTN_RIGHT    0x0008u
#define RB_BTN_A        0x0010u
#define RB_BTN_B        0x0020u
#define RB_BTN_C        0x0040u
#define RB_BTN_START    0x0080u
#define RB_BTN_QUIT     0x8000u
/* PC-98 keys 1..4. The loader ORs these into the high byte of the
 * game word (0x2000, 0x1000, 0x0800, 0x0400), not into the d-pad. */
#define RB_BTN_1        0x0100u
#define RB_BTN_2        0x0200u
#define RB_BTN_3        0x0400u
#define RB_BTN_4        0x0800u

/* ---- entities (shared layout for Phase 1 and Phase 2) ------------------- */

typedef struct RB_PACKED RbVec2 {
    int16_t x;
    int16_t y;
} RbVec2;

typedef struct RB_PACKED RbRect {
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
} RbRect;

typedef struct RB_PACKED RbPlayer {
    RbVec2   pos;
    RbVec2   vel;
    RbRect   hitbox;
    int16_t  hp;
    int16_t  lives;
    uint16_t flags;
    uint16_t anim_frame;
    uint16_t invuln_frames;
} RbPlayer;

typedef struct RB_PACKED RbEnemy {
    RbVec2   pos;
    RbVec2   vel;        /* 8.8 fixed-point words (entity +0x26 / +0x28) */
    RbRect   hitbox;     /* absolute rectangle, rebuilt from the offsets */
    int16_t  hp;
    uint16_t type;
    uint16_t flags;
    uint16_t anim_frame;
    uint16_t timer;
    uint16_t angle;      /* +0x2E, polar heading */
    uint16_t spin;       /* +0x30 */
    uint16_t speed;      /* +0x32 */
    int16_t  ax;         /* +0x2A, added to the X velocity word each frame */
    int16_t  ay;         /* +0x2C */
    int16_t  box_l;
    int16_t  box_r;
    int16_t  box_t;
    int16_t  box_b;
    int16_t  hp_seen;    /* previous +0x3C, so a change can score once */
    uint8_t  frac_x;     /* +0x4A */
    uint8_t  frac_y;     /* +0x4B */
    uint8_t  mover;      /* RB_MOVER_* */
    uint8_t  counted;    /* 1 = this spawn incremented the stage gate */
} RbEnemy;

typedef struct RB_PACKED RbBullet {
    RbVec2   pos;
    RbVec2   vel;
    RbRect   hitbox;
    int16_t  damage;
    uint16_t owner;     /* 0 = player, else enemy type id */
    uint16_t flags;
    uint16_t life;
} RbBullet;

typedef struct RB_PACKED RbParticle {
    RbVec2   pos;
    RbVec2   vel;
    uint16_t life;
    uint16_t color;
    uint16_t flags;
    uint16_t _pad;
} RbParticle;

/* ---- sprite blit descriptor --------------------------------------------- */

typedef struct RB_PACKED RbSprite {
    int16_t  x;
    int16_t  y;
    uint16_t w;
    uint16_t h;
    uint16_t palette;       /* logical palette bank */
    uint16_t flags;         /* flip / priority bits */
    const uint8_t *pixels;  /* 8-bit indexed, row-major; NULL = solid */
    uint32_t solid_rgba;    /* used when pixels == NULL (host RGBA8888) */
} RbSprite;

/* ---- HAL lifecycle ------------------------------------------------------ */

int  sys_init(const char *title);
void sys_shutdown(void);
/* stderr, plus a dialog when a real video driver is up. */
void sys_message(const char *title, const char *text);

/* Sleep `counts` PC-98 wait ticks (1/RB_WAIT_HZ each). A zero count still
 * waits one tick, which is the GDC edge that follows the counter. */
void sys_wait_vblank(unsigned counts);
void sys_present(void);

void sys_clear(uint8_t r, uint8_t g, uint8_t b);
/* Host RGBA8888 framebuffer, RB_NATIVE_W by RB_NATIVE_H, or NULL. */
uint32_t *sys_frame_pixels(void);
void sys_draw_sprite(const RbSprite *spr);
void sys_draw_rect(const RbRect *rect, uint32_t rgba);
void sys_draw_text(int16_t x, int16_t y, const char *utf8, uint32_t rgba);

void sys_read_input(RbInput *out);

int  sys_play_sound(uint16_t id, uint8_t vol);
int  sys_play_music(uint16_t track_id);
void sys_stop_music(void);
/* Open FPLAY.COM and SONG.DAT from a RUDEBR directory. 0 on success. */
int  sys_audio_bind(const char *dir);
/* Same player, from buffers already read out of a directory or a disk image. */
int  sys_audio_bind_mem(const uint8_t *com, size_t com_n,
                        const uint8_t *song, size_t song_n,
                        const char *rhythm_dir);
/* Optional path to ym2608_adpcm_rom.bin. NULL clears it. The file is
 * not shipped; a missing ROM leaves ADPCM-A silent. */
void sys_set_rhythm_override(const char *path);
/* Opcode 0x46, INT 7Fh AH=4. speed is the fade step the driver stores. */
void sys_fade_music(uint8_t speed);

/* Milliseconds since sys_init (debug / Phase 1 only; core must not rely). */
uint32_t sys_ticks_ms(void);

#ifdef __cplusplus
}
#endif

#endif /* RUDEBREAKER_HAL_H */
