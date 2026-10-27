/**
 * Phase 1 video backend — SDL2 software framebuffer at 640x400.
 */
#include "hal.h"
#include "pace.h"
#include "exepath.h"

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif
#endif

SDL_Window   *g_rb_window  = NULL;
SDL_Renderer *g_rb_renderer = NULL;
SDL_Texture  *g_rb_texture  = NULL;
uint32_t     *g_rb_pixels   = NULL; /* RGBA8888, native resolution */

static uint32_t g_start_ticks;

static int frames_are_fast(void)
{
    const char *v = getenv("RUDEBR_FAST");

    return v && v[0] && strcmp(v, "0") != 0;
}

void sys_message(const char *title, const char *text)
{
    const char *drv;

    fprintf(stderr, "%s\n%s\n", title ? title : "Rude Breaker", text ? text : "");
    if (frames_are_fast() || getenv("RUDEBR_STOP") || getenv("RUDEBR_TRACE")) {
        return;
    }
    drv = SDL_GetCurrentVideoDriver();
    if (drv && strcmp(drv, "dummy") == 0) {
        return;
    }
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                             title ? title : "Rude Breaker",
                             text ? text : "",
                             g_rb_window);
}

int sys_init(const char *title)
{
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return -1;
    }

    g_rb_window = SDL_CreateWindow(
        title ? title : "Rude Breaker",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        RB_NATIVE_W, RB_NATIVE_H,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!g_rb_window) {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        return -1;
    }

    /* Present waits for vertical blank so frames are never torn and
     * each one is held for a whole number of refreshes. The logic clock
     * does not follow the monitor: sys_pace_frame_start decides how many
     * VM frames run, and a refresh that is not a multiple of 30 Hz only
     * repeats the latest frame. RUDEBR_VSYNC=0 turns it off. */
    {
        const char *vs = getenv("RUDEBR_VSYNC");
        int want_vsync = !(vs && strcmp(vs, "0") == 0);

        if (want_vsync) {
            g_rb_renderer = SDL_CreateRenderer(g_rb_window, -1,
                                               SDL_RENDERER_ACCELERATED |
                                               SDL_RENDERER_PRESENTVSYNC);
        }
    }
    if (!g_rb_renderer) {
        g_rb_renderer = SDL_CreateRenderer(g_rb_window, -1, SDL_RENDERER_ACCELERATED);
    }
    if (!g_rb_renderer) {
        g_rb_renderer = SDL_CreateRenderer(g_rb_window, -1, 0);
    }
    if (!g_rb_renderer) {
        fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
        return -1;
    }
    {
        SDL_RendererInfo info;

        if (SDL_GetRendererInfo(g_rb_renderer, &info) == 0 && info.name) {
            fprintf(stderr, "renderer: %s\n", info.name);
        }
    }
    SDL_RenderSetLogicalSize(g_rb_renderer, RB_NATIVE_W, RB_NATIVE_H);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    g_rb_texture = SDL_CreateTexture(g_rb_renderer,
                                     SDL_PIXELFORMAT_RGBA8888,
                                     SDL_TEXTUREACCESS_STREAMING,
                                     RB_NATIVE_W, RB_NATIVE_H);
    if (!g_rb_texture) {
        fprintf(stderr, "SDL_CreateTexture: %s\n", SDL_GetError());
        return -1;
    }

    g_rb_pixels = (uint32_t *)calloc((size_t)RB_NATIVE_W * (size_t)RB_NATIVE_H,
                                     sizeof(uint32_t));
    if (!g_rb_pixels) {
        return -1;
    }

    g_start_ticks = SDL_GetTicks();
    return 0;
}

void sys_shutdown(void)
{
    free(g_rb_pixels);
    g_rb_pixels = NULL;
    if (g_rb_texture) {
        SDL_DestroyTexture(g_rb_texture);
        g_rb_texture = NULL;
    }
    if (g_rb_renderer) {
        SDL_DestroyRenderer(g_rb_renderer);
        g_rb_renderer = NULL;
    }
    if (g_rb_window) {
        SDL_DestroyWindow(g_rb_window);
        g_rb_window = NULL;
    }
    SDL_Quit();
}

uint32_t *sys_frame_pixels(void)
{
    return g_rb_pixels;
}

void sys_clear(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t color = ((uint32_t)r << 24) | ((uint32_t)g << 16) |
                     ((uint32_t)b << 8) | 0xFFu;
    size_t n = (size_t)RB_NATIVE_W * (size_t)RB_NATIVE_H;
    size_t i;
    for (i = 0; i < n; i++) {
        g_rb_pixels[i] = color;
    }
}

static void put_pixel(int x, int y, uint32_t rgba)
{
    if (x < 0 || y < 0 || x >= RB_NATIVE_W || y >= RB_NATIVE_H) {
        return;
    }
    g_rb_pixels[(size_t)y * (size_t)RB_NATIVE_W + (size_t)x] = rgba;
}

void sys_draw_rect(const RbRect *rect, uint32_t rgba)
{
    int16_t y;
    if (!rect) {
        return;
    }
    for (y = 0; y < rect->h; y++) {
        int16_t x;
        for (x = 0; x < rect->w; x++) {
            put_pixel(rect->x + x, rect->y + y, rgba);
        }
    }
}

void sys_draw_sprite(const RbSprite *spr)
{
    uint16_t row;
    if (!spr) {
        return;
    }
    if (!spr->pixels) {
        RbRect r;
        r.x = spr->x;
        r.y = spr->y;
        r.w = (int16_t)spr->w;
        r.h = (int16_t)spr->h;
        sys_draw_rect(&r, spr->solid_rgba ? spr->solid_rgba : 0xFFFFFFFFu);
        return;
    }
    for (row = 0; row < spr->h; row++) {
        uint16_t col;
        for (col = 0; col < spr->w; col++) {
            uint8_t idx = spr->pixels[row * spr->w + col];
            if (idx == 0) {
                continue; /* 0 = transparent */
            }
            /* Placeholder greyscale until real palettes are ported. */
            uint32_t c = ((uint32_t)idx << 24) | ((uint32_t)idx << 16) |
                         ((uint32_t)idx << 8) | 0xFFu;
            put_pixel(spr->x + (int16_t)col, spr->y + (int16_t)row, c);
        }
    }
}

/* Tiny 5x7 hex font for HUD (ASCII 32..95). */
static const uint8_t FONT5X7[64][5] = {
    {0,0,0,0,0}, /* space */
    {0x00,0x00,0x5F,0x00,0x00}, {0x00,0x07,0x00,0x07,0x00},
    {0x14,0x7F,0x14,0x7F,0x14}, {0x24,0x2A,0x7F,0x2A,0x12},
    {0x23,0x13,0x08,0x64,0x62}, {0x36,0x49,0x55,0x22,0x50},
    {0x00,0x05,0x03,0x00,0x00}, {0x00,0x1C,0x22,0x41,0x00},
    {0x00,0x41,0x22,0x1C,0x00}, {0x08,0x2A,0x1C,0x2A,0x08},
    {0x08,0x08,0x3E,0x08,0x08}, {0x00,0x50,0x30,0x00,0x00},
    {0x08,0x08,0x08,0x08,0x08}, {0x00,0x60,0x60,0x00,0x00},
    {0x20,0x10,0x08,0x04,0x02}, {0x3E,0x51,0x49,0x45,0x3E},
    {0x00,0x42,0x7F,0x40,0x00}, {0x42,0x61,0x51,0x49,0x46},
    {0x21,0x41,0x45,0x4B,0x31}, {0x18,0x14,0x12,0x7F,0x10},
    {0x27,0x45,0x45,0x45,0x39}, {0x3C,0x4A,0x49,0x49,0x30},
    {0x01,0x71,0x09,0x05,0x03}, {0x36,0x49,0x49,0x49,0x36},
    {0x06,0x49,0x49,0x29,0x1E}, {0x00,0x36,0x36,0x00,0x00},
    {0x00,0x56,0x36,0x00,0x00}, {0x00,0x08,0x14,0x22,0x41},
    {0x14,0x14,0x14,0x14,0x14}, {0x41,0x22,0x14,0x08,0x00},
    {0x02,0x01,0x51,0x09,0x06}, {0x32,0x49,0x79,0x41,0x3E},
    {0x7E,0x11,0x11,0x11,0x7E}, {0x7F,0x49,0x49,0x49,0x36},
    {0x3E,0x41,0x41,0x41,0x22}, {0x7F,0x41,0x41,0x22,0x1C},
    {0x7F,0x49,0x49,0x49,0x41}, {0x7F,0x09,0x09,0x09,0x01},
    {0x3E,0x41,0x49,0x49,0x7A}, {0x7F,0x08,0x08,0x08,0x7F},
    {0x00,0x41,0x7F,0x41,0x00}, {0x20,0x40,0x41,0x3F,0x01},
    {0x7F,0x08,0x14,0x22,0x41}, {0x7F,0x40,0x40,0x40,0x40},
    {0x7F,0x02,0x04,0x02,0x7F}, {0x7F,0x04,0x08,0x10,0x7F},
    {0x3E,0x41,0x41,0x41,0x3E}, {0x7F,0x09,0x09,0x09,0x06},
    {0x3E,0x41,0x51,0x21,0x5E}, {0x7F,0x09,0x19,0x29,0x46},
    {0x46,0x49,0x49,0x49,0x31}, {0x01,0x01,0x7F,0x01,0x01},
    {0x3F,0x40,0x40,0x40,0x3F}, {0x1F,0x20,0x40,0x20,0x1F},
    {0x7F,0x20,0x18,0x20,0x7F}, {0x63,0x14,0x08,0x14,0x63},
    {0x03,0x04,0x78,0x04,0x03}, {0x61,0x51,0x49,0x45,0x43},
    {0x00,0x7F,0x41,0x41,0x00}, {0x02,0x04,0x08,0x10,0x20},
    {0x00,0x41,0x41,0x7F,0x00}, {0x04,0x02,0x01,0x02,0x04},
    {0x40,0x40,0x40,0x40,0x40},
};

void sys_draw_text(int16_t x, int16_t y, const char *utf8, uint32_t rgba)
{
    int16_t cx = x;
    const unsigned char *p = (const unsigned char *)utf8;
    if (!p) {
        return;
    }
    while (*p) {
        unsigned char ch = *p++;
        int idx;
        int col;
        if (ch == '\n') {
            cx = x;
            y = (int16_t)(y + 8);
            continue;
        }
        if (ch < 32 || ch > 95) {
            if (ch >= 'a' && ch <= 'z') {
                ch = (unsigned char)(ch - 32);
            } else {
                ch = '?';
            }
        }
        idx = (int)ch - 32;
        for (col = 0; col < 5; col++) {
            uint8_t bits = FONT5X7[idx][col];
            int row;
            for (row = 0; row < 7; row++) {
                if (bits & (1u << row)) {
                    put_pixel(cx + col, y + row, rgba);
                }
            }
        }
        cx = (int16_t)(cx + 6);
    }
}

static void note_timing(unsigned counts)
{
    static uint64_t origin;
    static uint64_t window;
    static unsigned frames;
    const char *v = getenv("RUDEBR_TIMING");
    uint64_t freq;
    uint64_t now;

    if (!v || !v[0] || strcmp(v, "0") == 0) {
        return;
    }
    freq = SDL_GetPerformanceFrequency();
    now = SDL_GetPerformanceCounter();
    if (freq == 0) {
        return;
    }
    if (frames == 0) {
        origin = now;
        window = now;
    }
    frames++;
    if ((frames % 60u) == 0u && now > window) {
        double wall_ms = (double)(now - origin) * 1000.0 / (double)freq;
        double fps = 60.0 * (double)freq / (double)(now - window);

        fprintf(stderr,
                "timing: frames=%u wall_ms=%.0f logic_fps=%.2f vsyncs=%u\n",
                frames, wall_ms, fps, counts);
        window = now;
    }
}

void sys_wait_vblank(unsigned counts)
{
    /* Absolute deadline so a slow frame does not stack extra delay, and
     * a late frame resyncs instead of trying to catch up. RUDEBR_FAST
     * skips the wait. It is not a player control. */
    static uint64_t next;
    uint64_t freq;
    uint64_t now;
    uint64_t step;

    if (counts == 0u) {
        counts = 1u;
    }
    if (counts > 30u) {
        counts = 30u;
    }
    note_timing(counts);
    if (frames_are_fast()) {
        return;
    }
    freq = SDL_GetPerformanceFrequency();
    now = SDL_GetPerformanceCounter();
    if (freq == 0) {
        SDL_Delay((1000u / (unsigned)RB_WAIT_HZ) * counts);
        return;
    }
    step = (freq * (uint64_t)counts) / (uint64_t)RB_WAIT_HZ;
    if (step == 0u) {
        step = 1u;
    }
    if (next == 0u || now > next + step) {
        next = now;
    }
    next += step;
    while ((now = SDL_GetPerformanceCounter()) + freq / 500ull < next) {
        uint64_t left = next - now;
        uint32_t ms = (uint32_t)((left * 1000ull) / freq);

        if (ms > 1u) {
            SDL_Delay(ms - 1u);
        } else {
            break;
        }
    }
    while (SDL_GetPerformanceCounter() < next) {
    }
}

/* ---- locked logic clock -------------------------------------------------- */

static uint64_t pace_freq;
static uint64_t pace_period;
static unsigned pace_count = 2;
static int pace_skip_wait;
static uint64_t pace_next;
static uint64_t pace_origin;
static uint64_t pace_steps;
static uint64_t pace_slow_events;
static uint64_t pace_caught;
static int pace_refresh;
static int pace_window_refresh;
static int pace_vsync_driver;
static int pace_timer_ms;
static int pace_overlay_on;
static int pace_f8_was;
/* Presentation. pace_pref_exact is the player's choice. pace_vsync_on
 * means the renderer was created with PRESENTVSYNC. pace_vsync_ok is
 * set once present intervals prove the driver really blocks, and
 * pace_hold is the refreshes per logic frame in smooth mode (0 when
 * the measured refresh is not a multiple of 30 Hz). */
static int pace_pref_exact;
static int pace_pref_loaded;
static int pace_vsync_on;
static int pace_vsync_ok;
static int pace_hold;
static double pace_meas_hz;
static uint64_t pace_last_present;
static double pace_iv_sum;
static int pace_iv_n;
static uint64_t pace_presents;
static char pace_strategy[64];
#if defined(_WIN32)
static HANDLE pace_timer;
#endif

static const char *pace_mode_name(void);

static void pace_write_log(void)
{
    char dir[1024];
    char path[1100];
    char body[1600];
    FILE *fp;
    double measured = 0.0;
    uint64_t now;
    int n;

    if (pace_freq == 0u) {
        return;
    }
    now = SDL_GetPerformanceCounter();
    if (pace_origin != 0u && now > pace_origin && pace_steps > 0u) {
        measured = (double)pace_steps * (double)pace_freq / (double)(now - pace_origin);
    }
    n = snprintf(body, sizeof(body),
                 "strategy=%s\n"
                 "logic_hz=%.4f\n"
                 "logic_period_us=%.1f\n"
                 "measured_logic_hz=%.3f\n"
                 "logic_steps=%llu\n"
                 "catchup_steps=%llu\n"
                 "slowdowns=%llu\n"
                 "catchup_max=%d\n"
                 "refresh_hz=%d\n"
                 "window_refresh_hz=%d\n"
                 "vsync_driver=%s\n"
                 "vsync_used=%s\n"
                 "pacing=%s\n"
                 "measured_refresh_hz=%.2f\n"
                 "refreshes_per_logic_frame=%d\n"
                 "timer_hz=%llu\n"
                 "timer_period_ms=%d\n"
                 "overlay=F8\n"
                 "menu_wait=ignored\n"
                 "smooth: on a 60/120/240 Hz display each logic frame is held for\n"
                 "exactly refresh/30 refreshes (30.00 Hz, 2.2%% under 30.67).\n"
                 "exact: the logic tick is one performance-counter period (30.67 Hz)\n"
                 "and every refresh shows the latest frame. Without a verified vsync\n"
                 "the timer paces both. A hitch repays at most %d logic frames.\n",
                 pace_strategy,
                 (double)RB_WAIT_HZ / 3.0,
                 (pace_freq ? (double)pace_period * 1000000.0 / (double)pace_freq : 0.0),
                 measured,
                 (unsigned long long)pace_steps,
                 (unsigned long long)pace_caught,
                 (unsigned long long)pace_slow_events,
                 RB_PACE_CATCH_MAX,
                 pace_refresh,
                 pace_window_refresh,
                 pace_vsync_driver ? "available" : "unavailable",
                 pace_vsync_ok ? "on" : (pace_vsync_on ? "requested-unverified" : "off"),
                 pace_mode_name(),
                 pace_meas_hz,
                 pace_hold,
                 (unsigned long long)pace_freq,
                 pace_timer_ms,
                 RB_PACE_CATCH_MAX);
    if (n < 0) {
        return;
    }
    if (rb_exe_dir(dir, sizeof(dir)) == 0) {
        snprintf(path, sizeof(path), "%s/pace.txt", dir);
    } else {
        snprintf(path, sizeof(path), "pace.txt");
    }
    fp = fopen(path, "wb");
    if (!fp) {
        return;
    }
    fwrite(body, 1, (size_t)n, fp);
    fclose(fp);
}

static void pace_sleep_until(uint64_t deadline)
{
    for (;;) {
        uint64_t now = SDL_GetPerformanceCounter();
        uint64_t left;
        uint64_t ms;

        if (pace_freq == 0u || now >= deadline) {
            return;
        }
        left = deadline - now;
        ms = (left * 1000ull) / pace_freq;
        if (ms <= 2u) {
            while (SDL_GetPerformanceCounter() < deadline) {
            }
            return;
        }
#if defined(_WIN32)
        if (pace_timer) {
            LARGE_INTEGER due;

            due.QuadPart = -(LONGLONG)((left * 10000000ull) / pace_freq);
            if (SetWaitableTimer(pace_timer, &due, 0, NULL, NULL, FALSE)) {
                WaitForSingleObject(pace_timer, (DWORD)ms);
                continue;
            }
        }
#endif
        SDL_Delay((Uint32)(ms - 1u));
    }
}

static void pace_poll_overlay(void)
{
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    int down = keys && keys[SDL_SCANCODE_F8];

    if (down && !pace_f8_was) {
        pace_overlay_on = !pace_overlay_on;
    }
    pace_f8_was = down;
}

/* ---- presentation: smooth / exact --------------------------------------- */

static void pace_pref_path(char *path, size_t n)
{
    char dir[1024];

    if (rb_exe_dir(dir, sizeof(dir)) == 0) {
        snprintf(path, n, "%s/rudebreaker.ini", dir);
    } else {
        snprintf(path, n, "rudebreaker.ini");
    }
}

static void pace_load_pref(void)
{
    char path[1100];
    char line[128];
    const char *env = getenv("RUDEBR_PACING");
    FILE *fp;

    if (pace_pref_loaded) {
        return;
    }
    pace_pref_loaded = 1;
    pace_pref_exact = 0;
    pace_pref_path(path, sizeof path);
    fp = fopen(path, "rb");
    if (fp) {
        while (fgets(line, sizeof line, fp)) {
            if (strncmp(line, "pacing=", 7) == 0) {
                pace_pref_exact = strncmp(line + 7, "exact", 5) == 0;
            }
        }
        fclose(fp);
    }
    if (env && env[0]) {
        pace_pref_exact = strcmp(env, "exact") == 0;
    }
}

int sys_pace_pref_exact(void)
{
    pace_load_pref();
    return pace_pref_exact;
}

void sys_pace_set_pref_exact(int exact)
{
    char path[1100];
    FILE *fp;

    pace_load_pref();
    exact = exact ? 1 : 0;
    if (exact == pace_pref_exact) {
        return;
    }
    pace_pref_exact = exact;
    pace_next = 0;
    pace_pref_path(path, sizeof path);
    fp = fopen(path, "wb");
    if (fp) {
        fprintf(fp, "pacing=%s\n", exact ? "exact" : "smooth");
        fclose(fp);
    }
}

static int pace_smooth_active(void)
{
    return !pace_pref_exact && pace_vsync_ok && pace_hold > 0 &&
           !pace_skip_wait;
}

static const char *pace_mode_name(void)
{
    if (pace_smooth_active()) {
        return "smooth";
    }
    if (pace_vsync_ok) {
        return pace_pref_exact ? "exact" : "exact (refresh not a multiple of 30)";
    }
    return pace_vsync_on ? "timer (vsync not verified)" : "timer (vsync off)";
}

/* Until 24 measured presents say otherwise, exact mode runs without
 * sleeping so the probe sees how long a present blocks. A present that
 * returns in under 3 ms is not waiting for vertical blank. */
static int pace_vsync_probe(void)
{
    /* A minimised window or a driver switch can make a present stop
     * blocking. Probe again now and then while vsync is unverified. */
    if (pace_vsync_on && !pace_vsync_ok && pace_presents > 3000u) {
        pace_presents = 0;
        pace_iv_sum = 0.0;
        pace_iv_n = 0;
    }
    return pace_vsync_on && pace_iv_n < 24 && pace_presents < 400u;
}

/* Refreshes per logic frame for the measured refresh, or 0 when the
 * refresh is not within 2.5% of a multiple of 30 Hz (VRR, 75, 144...). */
static int pace_hold_for(double hz)
{
    int k = (int)(hz / 30.0 + 0.5);

    if (k < 2 || k > 8) {
        return 0;
    }
    if (hz < (double)k * 30.0 * 0.975 || hz > (double)k * 30.0 * 1.025) {
        return 0;
    }
    return k;
}

static void pace_note_present(void)
{
    uint64_t now = SDL_GetPerformanceCounter();

    pace_presents++;
    if (pace_last_present != 0u && pace_freq != 0u) {
        double ms = (double)(now - pace_last_present) * 1000.0 / (double)pace_freq;

        /* Skip hitches and the stage hold. */
        if (ms > 0.0 && ms < 60.0) {
            pace_iv_sum += ms;
            pace_iv_n++;
        }
    }
    pace_last_present = now;
    if (pace_iv_n >= 24) {
        double mean = pace_iv_sum / (double)pace_iv_n;
        int ok = pace_vsync_on && mean >= 3.0;
        double hz = mean > 0.0 ? 1000.0 / mean : 0.0;

        /* In exact mode a slow loop can span several refreshes, so the
         * measured rate is only trusted when it looks like one refresh. */
        if (ok && (hz < 47.0 || hz > 362.0)) {
            ok = pace_vsync_ok;
            hz = pace_meas_hz;
        }
        if (ok != pace_vsync_ok || pace_hold_for(hz) != pace_hold) {
            pace_next = 0;
        }
        pace_vsync_ok = ok;
        if (ok) {
            pace_meas_hz = hz;
            pace_hold = pace_hold_for(hz);
        } else {
            pace_hold = 0;
        }
        pace_iv_sum = 0.0;
        pace_iv_n = 0;
    }
}

int sys_pace_present_count(void)
{
    if (frames_are_fast()) {
        return 1;
    }
    return pace_smooth_active() ? pace_hold : 1;
}

const char *sys_pace_mode_name(void)
{
    pace_load_pref();
    return pace_mode_name();
}

void sys_pace_startup(void)
{
    SDL_DisplayMode mode;
    int index;
    int drivers;
    int i;

    memset(pace_strategy, 0, sizeof pace_strategy);
    snprintf(pace_strategy, sizeof pace_strategy, "performance-counter");
    pace_freq = SDL_GetPerformanceFrequency();
    if (pace_freq == 0u) {
        pace_freq = 1000u;
    }
    pace_period = (pace_freq * 3ull) / (uint64_t)RB_WAIT_HZ;
    if (pace_period == 0u) {
        pace_period = 1u;
    }
    pace_next = 0;
    pace_origin = 0;
    pace_steps = 0;
    pace_slow_events = 0;
    pace_caught = 0;
    pace_refresh = 0;
    pace_window_refresh = 0;
    pace_vsync_driver = 0;
    pace_timer_ms = 0;
    pace_overlay_on = 0;
    pace_f8_was = 0;

    pace_load_pref();
    pace_vsync_on = 0;
    pace_vsync_ok = 0;
    pace_hold = 0;
    pace_iv_sum = 0.0;
    pace_iv_n = 0;
    pace_presents = 0;
    pace_last_present = 0;
    if (g_rb_renderer) {
        SDL_RendererInfo ri;

        if (SDL_GetRendererInfo(g_rb_renderer, &ri) == 0 &&
            (ri.flags & SDL_RENDERER_PRESENTVSYNC) != 0) {
            pace_vsync_on = 1;
        }
    }
    if (g_rb_window) {
        index = SDL_GetWindowDisplayIndex(g_rb_window);
        if (index >= 0 && SDL_GetCurrentDisplayMode(index, &mode) == 0) {
            pace_refresh = mode.refresh_rate;
        }
        if (SDL_GetWindowDisplayMode(g_rb_window, &mode) == 0) {
            pace_window_refresh = mode.refresh_rate;
        }
    }
    drivers = SDL_GetNumRenderDrivers();
    for (i = 0; i < drivers; i++) {
        SDL_RendererInfo info;

        if (SDL_GetRenderDriverInfo(i, &info) == 0 &&
            (info.flags & SDL_RENDERER_PRESENTVSYNC) != 0) {
            pace_vsync_driver = 1;
            break;
        }
    }
#if defined(_WIN32)
    if (timeBeginPeriod(1) == TIMERR_NOERROR) {
        pace_timer_ms = 1;
    }
    pace_timer = CreateWaitableTimerExW(NULL, NULL,
                                        CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                                        TIMER_ALL_ACCESS);
    if (!pace_timer) {
        pace_timer = CreateWaitableTimerW(NULL, FALSE, NULL);
    }
#else
    /* clock_nanosleep / SDL_Delay on this host is already sub-millisecond
     * once the last 2 ms are spun on the performance counter. */
    pace_timer_ms = 1;
#endif
    fprintf(stderr,
            "pace: strategy=%s logic_hz=%.3f refresh=%d window_refresh=%d "
            "vsync_driver=%d vsync_requested=%d pacing=%s timer_hz=%llu timer_ms=%d\n",
            pace_strategy, (double)RB_WAIT_HZ / 3.0, pace_refresh,
            pace_window_refresh, pace_vsync_driver, pace_vsync_on,
            pace_pref_exact ? "exact" : "smooth",
            (unsigned long long)pace_freq, pace_timer_ms);
    pace_write_log();
}

void sys_pace_shutdown(void)
{
    pace_write_log();
#if defined(_WIN32)
    if (pace_timer) {
        CloseHandle(pace_timer);
        pace_timer = NULL;
    }
    if (pace_timer_ms == 1) {
        timeEndPeriod(1);
    }
#endif
}

int sys_pace_fast(void)
{
    return frames_are_fast();
}

void sys_pace_set_mode(unsigned count, int skip_wait)
{
    if (count == 0u) {
        count = 1u;
    }
    if (count > 16u) {
        count = 16u;
    }
    pace_count = count;
    pace_skip_wait = skip_wait ? 1 : 0;
    if (pace_freq == 0u) {
        return;
    }
    /* The logic period stays on the capture lock, 3/92 s. The wait word
     * [bp+0x60] shares its value with the stock field, which RBRANK.DAT
     * saves, so a stale 3 would have made the period 20.4 Hz and a 1
     * 61.3 Hz. Neither matches the capture, so the count is only kept
     * for the log. */
    pace_period = (pace_freq * 3ull) / (uint64_t)RB_WAIT_HZ;
    if (pace_period == 0u) {
        pace_period = 1u;
    }
}

int sys_pace_frame_start(void)
{
    uint64_t now;
    int slowed = 0;
    int steps;

    pace_poll_overlay();
    if (frames_are_fast()) {
        pace_steps++;
        return 1;
    }
    if (pace_skip_wait) {
        pace_next = 0;
        pace_steps++;
        return 1;
    }
    if (pace_period == 0u) {
        sys_pace_startup();
    }
    pace_load_pref();
    if (pace_smooth_active()) {
        /* The presents that follow block for pace_hold refreshes, so
         * one logic frame per loop is the whole clock. */
        now = SDL_GetPerformanceCounter();
        pace_next = 0;
        if (pace_origin == 0u) {
            pace_origin = now;
        }
        pace_steps++;
        if ((pace_steps % 60u) == 0u) {
            pace_write_log();
        }
        return 1;
    }
    now = SDL_GetPerformanceCounter();
    if (pace_vsync_ok || pace_vsync_probe()) {
        /* Exact with vsync: the present blocks for one refresh, so do
         * not sleep. Run whatever logic frames are due (often 0). */
        if (pace_next != 0u && now < pace_next) {
            return 0;
        }
        steps = rb_pace_due(now, &pace_next, pace_period, RB_PACE_CATCH_MAX,
                            &slowed);
        if (steps < 1) {
            return 0;
        }
    } else {
        if (pace_next != 0u && now < pace_next) {
            pace_sleep_until(pace_next);
            now = SDL_GetPerformanceCounter();
        }
        steps = rb_pace_due(now, &pace_next, pace_period, RB_PACE_CATCH_MAX,
                            &slowed);
        if (steps < 1) {
            steps = 1;
            pace_next = now + pace_period;
        }
    }
    if (pace_origin == 0u) {
        pace_origin = now;
    }
    if (steps > 1) {
        pace_caught += (uint64_t)(steps - 1);
    }
    if (slowed) {
        pace_slow_events++;
    }
    pace_steps += (uint64_t)steps;
    if ((pace_steps % 60u) == 0u) {
        pace_write_log();
    }
    return steps;
}

void sys_pace_resync(void)
{
    pace_next = 0;
}

int sys_pace_overlay(void)
{
    return pace_overlay_on;
}

void sys_pace_overlay_draw(void)
{
    char line[96];
    double measured = 0.0;
    uint64_t now;

    if (!pace_overlay_on || !g_rb_pixels || pace_freq == 0u) {
        return;
    }
    now = SDL_GetPerformanceCounter();
    if (pace_origin != 0u && now > pace_origin && pace_steps > 0u) {
        measured = (double)pace_steps * (double)pace_freq / (double)(now - pace_origin);
    }
    snprintf(line, sizeof(line), "PACE %s  lock %.2f Hz  now %.2f",
             pace_strategy, (double)RB_WAIT_HZ / 3.0, measured);
    sys_draw_text(8, 360, line, 0xFFFFFF00u);
    snprintf(line, sizeof(line), "%s  refresh %.1f  hold %d",
             pace_mode_name(), pace_meas_hz, pace_hold);
    sys_draw_text(8, 372, line, 0xFFFFFF00u);
    snprintf(line, sizeof(line), "catch +%llu  slow %llu  F8 hides",
             (unsigned long long)pace_caught,
             (unsigned long long)pace_slow_events);
    sys_draw_text(8, 384, line, 0xFFFFFF00u);
}

void sys_idle(unsigned ms)
{
    while (ms > 0u) {
        unsigned step = ms > 50u ? 50u : ms;

        SDL_PumpEvents();
        SDL_Delay(step);
        ms -= step;
    }
}

void sys_present(void)
{
    if (!g_rb_texture || !g_rb_renderer || !g_rb_pixels) {
        return;
    }
    SDL_UpdateTexture(g_rb_texture, NULL, g_rb_pixels,
                      RB_NATIVE_W * (int)sizeof(uint32_t));
    SDL_SetRenderDrawColor(g_rb_renderer, 0, 0, 0, 255);
    SDL_RenderClear(g_rb_renderer);
    SDL_RenderCopy(g_rb_renderer, g_rb_texture, NULL, NULL);
    SDL_RenderPresent(g_rb_renderer);
    pace_note_present();
}

uint32_t sys_ticks_ms(void)
{
    return SDL_GetTicks() - g_start_ticks;
}
