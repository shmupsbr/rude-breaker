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
/* Logic period for menu wait count N ([bp+0x60]) is N/2 * 3/92 s.
 * N = 2 is the capture lock: cruise scroll is 4 px per logic frame at
 * about 122 px/s, so one logic frame is 3/92 s (about 30.7 Hz).
 * Count 1 is about 61 Hz. Count 3 is about 20 Hz. Bit 0x4000 of the
 * debug word skips that wait. 120 Hz per count was about 1.3x ahead
 * of the capture. Not the host refresh, and not one PC-98 vertical
 * blank (56.42 Hz): three of those would be about 19 Hz. */
#define RB_WAIT_HZ      92
#define RB_TARGET_FPS   (RB_WAIT_HZ / 3)
/* Repay at most this many logic frames after a hitch. Past the cap
 * the clock drops the debt and the game slows instead of skipping. */
#define RB_PACE_CATCH_MAX 4

/* X68000 Phase-2 outer raster (640x400 centered in 640x480) */
#define RB_X68K_OUTER_H 480
#define RB_X68K_VMARGIN 40

/* ---- input -------------------------------------------------------------- */

typedef struct RB_PACKED RbInput {
    uint16_t buttons;   /* bitfield: see RB_BTN_* */
    int16_t  axis_x;    /* -1, 0, +1 */
    int16_t  axis_y;    /* -1, 0, +1 */
    /* 1 when a gamepad is attached. The menu ORs this into B9 bit 3
     * before it will set the joystick flag. */
    uint8_t  pad;
    /* PC-98 key matrix copy at [bp+0xC2]. N E K O C, in that bit order. */
    uint8_t  chord;
    /* Gamepad buttons, same RB_BTN_* bits. Merged only while the
     * joystick flag [bp+0x72] bit 0x400 is set. */
    uint16_t pad_buttons;
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
/* Host-only speed button (> >> >>>). The game script never sees it. */
#define RB_BTN_SPEED    0x2000u

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

/* Sleep `counts` of 1/RB_WAIT_HZ. Kept for the X68000 backend. The SDL
 * loop paces with sys_pace_frame_start and does not call this. */
void sys_wait_vblank(unsigned counts);
/* Hold the last presented frame. The audio callback keeps running. */
void sys_idle(unsigned ms);
void sys_present(void);

/* Detect the display and the timer, then lock the logic clock. */
void sys_pace_startup(void);
void sys_pace_shutdown(void);
/* Sleep until at least one logic frame is due. Returns how many frames
 * to run now, from 1 to RB_PACE_CATCH_MAX. RUDEBR_FAST returns 1 and
 * does not sleep. */
int sys_pace_frame_start(void);
/* Next periods use wait count N as N/2 * 3/92 s. skip_wait is the
 * debug bit 0x4000: one step, no sleep, and no catch-up afterwards. */
void sys_pace_set_mode(unsigned count, int skip_wait);
/* Forget lateness. Used after the two-second stage hold. */
void sys_pace_resync(void);
/* F8 toggles a timing overlay drawn on top of the game frame. */
int sys_pace_overlay(void);
void sys_pace_overlay_draw(void);
int sys_pace_fast(void);
/* Presents to make for the frame just rendered: refresh/30 in smooth
 * mode on a verified-vsync 60/120/240 Hz display, otherwise 1. */
int sys_pace_present_count(void);
/* Player choice, saved to rudebreaker.ini beside the program.
 * RUDEBR_PACING=smooth|exact overrides it. */
int sys_pace_pref_exact(void);
void sys_pace_set_pref_exact(int exact);
const char *sys_pace_mode_name(void);

void sys_clear(uint8_t r, uint8_t g, uint8_t b);
/* Host RGBA8888 framebuffer, RB_NATIVE_W by RB_NATIVE_H, or NULL. */
uint32_t *sys_frame_pixels(void);

/* Wide layout (side panels): a 1920x1080 frame presented instead of the
 * 640x400 one while sys_set_wide(1) is in effect. */
#define RB_WIDE_W 1920
#define RB_WIDE_H 1080
int       sys_window_wide(void);
uint32_t *sys_wide_pixels(void);
void      sys_set_wide(int on);
void sys_draw_sprite(const RbSprite *spr);
void sys_draw_rect(const RbRect *rect, uint32_t rgba);
void sys_draw_text(int16_t x, int16_t y, const char *utf8, uint32_t rgba);

void sys_read_input(RbInput *out);
/* 1 if any key or pad/joystick button went down since the last call
 * (bound or not). Clears the latch. Used to skip the intro card. */
int sys_any_press(void);

/* One slot per action. DEFAULT keeps the built-in key and pad map.
 * KEY is an SDL scancode. BUTTON is a controller or joystick button.
 * AXIS uses a = axis index and b = -1 or +1. HAT uses a = hat index
 * and b = the SDL hat mask. */
#define RB_ACT_UP     0
#define RB_ACT_DOWN   1
#define RB_ACT_LEFT   2
#define RB_ACT_RIGHT  3
#define RB_ACT_FIRE   4
#define RB_ACT_MODE   5
#define RB_ACT_SHIFT  6
#define RB_ACT_PAUSE  7
#define RB_ACT_SPEED  8
#define RB_ACT_COUNT  9

#define RB_BIND_DEFAULT 0
#define RB_BIND_KEY     1
#define RB_BIND_BUTTON  2
#define RB_BIND_AXIS    3
#define RB_BIND_HAT     4

/* Arm capture. The next key, button, or axis edge is stored. Escape
 * cancels. sys_bind_result returns 1, -1, or 0 and clears the result. */
void sys_bind_begin(void);
int  sys_bind_result(int *kind, int *a, int *b);
void sys_bind_restore(void);
int  sys_bind_set(int action, int kind, int a, int b);
const char *sys_bind_label(int action);
void sys_bind_write(void *file);
int  sys_bind_read_line(const char *line);
/* Empty string when no device is open. The first live device wins. */
const char *sys_joy_name(void);
int  sys_joy_present(void);

int  sys_play_sound(uint16_t id, uint8_t vol);
int  sys_play_music(uint16_t track_id);
void sys_stop_music(void);
/* Open FPLAY.COM and SONG.DAT from a RUDEBR directory. 0 on success. */
int  sys_audio_bind(const char *dir);
/* Same player, from buffers already read out of a directory or a disk image. */
int  sys_audio_bind_mem(const uint8_t *com, size_t com_n,
                        const uint8_t *song, size_t song_n,
                        const char *data_dir);
/* Opcode 0x46, INT 7Fh AH=4. speed is the fade step the driver stores. */
void sys_fade_music(uint8_t speed);
/* 0..10 from the samples last handed to the audio device. */
int  sys_audio_level(void);
/* Sound Test spectrum: up to 16 bands, 0..1, loudest since the last
 * call. Returns how many were written (0 when there is no audio). */
int  sys_audio_bands(float *out, int n);

/* Freeze the output clock. Play and stop clear it. */
void sys_audio_pause(int on);
/* While on, play/stop/fade calls are dropped (silent hidden VM runs). */
void sys_audio_suppress(int on);

/* Milliseconds since sys_init (debug / Phase 1 only; core must not rely). */
uint32_t sys_ticks_ms(void);

#ifdef __cplusplus
}
#endif

#endif /* RUDEBREAKER_HAL_H */
