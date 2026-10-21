/**
 * Phase 1 video backend — SDL2 software framebuffer at 640x400.
 */
#include "hal.h"

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;

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

    if (!frames_are_fast()) {
        render_flags |= SDL_RENDERER_PRESENTVSYNC;
    }
    g_rb_renderer = SDL_CreateRenderer(g_rb_window, -1, render_flags);
    if (!g_rb_renderer) {
        g_rb_renderer = SDL_CreateRenderer(g_rb_window, -1, 0);
    }
    if (!g_rb_renderer) {
        fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
        return -1;
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

void sys_wait_vblank(void)
{
    /* Prefer vsync via Present; also clamp to ~60 Hz if vsync is off.
     * RUDEBR_FAST skips the clamp so a smoke test can walk a stage
     * without waiting out 60 Hz. It is not a player control. */
    static uint32_t last;
    uint32_t now;
    const uint32_t frame_ms = 1000u / (uint32_t)RB_TARGET_FPS;

    if (frames_are_fast()) {
        return;
    }
    now = SDL_GetTicks();
    if (last != 0) {
        uint32_t elapsed = now - last;
        if (elapsed < frame_ms) {
            SDL_Delay(frame_ms - elapsed);
        }
    }
    last = SDL_GetTicks();
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
