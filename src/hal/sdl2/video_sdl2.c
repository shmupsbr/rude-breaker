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

    /* The logic rate is a fixed timer in sys_wait_vblank. Present vsync
     * would lock that to a 120 Hz or 144 Hz monitor and run the game
     * several times too fast. */
    g_rb_renderer = SDL_CreateRenderer(g_rb_window, -1, SDL_RENDERER_ACCELERATED);
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
static char pace_strategy[64];
#if defined(_WIN32)
static HANDLE pace_timer;
#endif

static void pace_write_log(void)
{
    char dir[1024];
    char path[1100];
    char body[1200];
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
                 "vsync_used=off\n"
                 "timer_hz=%llu\n"
                 "timer_period_ms=%d\n"
                 "overlay=F8\n"
                 "menu_wait=ignored\n"
                 "The logic tick is one performance-counter period. Present does not\n"
                 "wait for vertical blank, so 50/60/75/120/144/165/240 Hz, variable\n"
                 "refresh, and a vsync toggle cannot change the game speed. A hitch\n"
                 "repays at most %d logic frames. Debt past that is dropped and the\n"
                 "game slows down instead of skipping ahead.\n",
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
            "vsync_driver=%d vsync_used=off timer_hz=%llu timer_ms=%d\n",
            pace_strategy, (double)RB_WAIT_HZ / 3.0, pace_refresh,
            pace_window_refresh, pace_vsync_driver,
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
    if (pace_period == 0u) {
        sys_pace_startup();
    }
    now = SDL_GetPerformanceCounter();
    if (pace_next != 0u && now < pace_next) {
        pace_sleep_until(pace_next);
        now = SDL_GetPerformanceCounter();
    }
    steps = rb_pace_due(now, &pace_next, pace_period, RB_PACE_CATCH_MAX, &slowed);
    if (steps < 1) {
        steps = 1;
        pace_next = now + pace_period;
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
    snprintf(line, sizeof(line), "refresh %d  vsync off  timer %llu Hz",
             pace_refresh, (unsigned long long)pace_freq);
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
}

uint32_t sys_ticks_ms(void)
{
    return SDL_GetTicks() - g_start_ticks;
}
