/**
 * Phase 2 video backend — Sharp X68000 (Human68k).
 *
 * Target: 640x480 @ 31 kHz CRTC mode with a 640x400 playfield centered
 * (40 px top/bottom margins). Uses IOCS and documented VDP regions only.
 *
 * Build with PLATFORM=x68k. This translation unit is not compiled for SDL2.
 */
#include "hal.h"

#if defined(RB_PLATFORM_X68K)

#include <stdio.h>
#include <string.h>
#include <sys/iocs.h>

/* Graphic VRAM (512x512 plane, 16-color modes) — official map. */
#define X68K_GVRAM   ((volatile uint16_t *)0xC00000)
/* Text VRAM / sprite control region (see Inside X68000 / IOCS docs). */
#define X68K_TVRAM   ((volatile uint16_t *)0xE00000)
#define X68K_SPRITE  ((volatile uint16_t *)0xEB0000)

static uint8_t g_fb[RB_NATIVE_W * RB_NATIVE_H]; /* indexed staging buffer */

int sys_init(const char *title)
{
    (void)title;
    /*
     * Select a 31 kHz progressive mode that yields 640x480 outer raster.
     * Exact CRTMOD code is board/monitor dependent; 0x1b is a common
     * 31 kHz 512x512-class mode — replace with the project-approved CRTMOD
     * once hardware bring-up is measured on real silicon / XM6.
     */
    _iocs_crtmod(0x1b);
    _iocs_g_clr_on();
    memset(g_fb, 0, sizeof(g_fb));
    return 0;
}

void sys_message(const char *title, const char *text)
{
    fprintf(stderr, "%s\n%s\n", title ? title : "Rude Breaker", text ? text : "");
}

void sys_shutdown(void)
{
    _iocs_g_clr_on();
}

uint32_t *sys_frame_pixels(void)
{
    return NULL;
}

void sys_clear(uint8_t r, uint8_t g, uint8_t b)
{
    /* Map RGB to a coarse 16-color index until real palettes are loaded. */
    uint8_t idx = (uint8_t)(((r >> 6) << 4) | ((g >> 6) << 2) | (b >> 6));
    memset(g_fb, idx, sizeof(g_fb));
}

void sys_draw_rect(const RbRect *rect, uint32_t rgba)
{
    int16_t y;
    uint8_t idx;
    if (!rect) {
        return;
    }
    idx = (uint8_t)(((rgba >> 28) & 0xFu));
    for (y = 0; y < rect->h; y++) {
        int16_t py = (int16_t)(rect->y + y);
        int16_t x;
        if (py < 0 || py >= RB_NATIVE_H) {
            continue;
        }
        for (x = 0; x < rect->w; x++) {
            int16_t px = (int16_t)(rect->x + x);
            if (px < 0 || px >= RB_NATIVE_W) {
                continue;
            }
            g_fb[(size_t)py * RB_NATIVE_W + (size_t)px] = idx;
        }
    }
}

void sys_draw_sprite(const RbSprite *spr)
{
    /*
     * Preferred path on X68000: upload 16x16 patterns to the 128 hardware
     * sprites at 0xEB0000. Until pattern banks are extracted from SGM1,
     * fall back to software blit into the staging buffer.
     */
    RbRect r;
    if (!spr) {
        return;
    }
    r.x = spr->x;
    r.y = spr->y;
    r.w = (int16_t)spr->w;
    r.h = (int16_t)spr->h;
    sys_draw_rect(&r, spr->solid_rgba ? spr->solid_rgba : 0xFFFFFFFFu);
}

void sys_draw_text(int16_t x, int16_t y, const char *utf8, uint32_t rgba)
{
    /* IOCS character output at text plane; y is playfield-relative. */
    int row = (int)y + RB_X68K_VMARGIN;
    int col = (int)x;
    (void)rgba;
    if (!utf8) {
        return;
    }
    _iocs_b_locate(col / 8, row / 16);
    _iocs_b_print((const unsigned char *)utf8);
}

void sys_wait_vblank(unsigned counts)
{
    unsigned i;

    /* X68000 vertical wait stands in for one PC-98 wait count. */
    if (counts == 0u) {
        counts = 1u;
    }
    for (i = 0; i < counts; i++) {
        _iocs_vdisp_wait();
    }
}

void sys_idle(unsigned ms)
{
    unsigned ticks = (ms * 60u + 999u) / 1000u;
    unsigned i;

    if (ticks == 0u) {
        ticks = 1u;
    }
    for (i = 0; i < ticks; i++) {
        _iocs_vdisp_wait();
    }
}

void sys_present(void)
{
    /*
     * Copy 640x400 staging buffer into graphic VRAM starting at line
     * RB_X68K_VMARGIN. Exact pitch/format depends on CRTMOD; this is the
     * structural hook — refine during hardware bring-up.
     */
    int y;
    for (y = 0; y < RB_NATIVE_H; y++) {
        int dst_y = y + RB_X68K_VMARGIN;
        const uint8_t *src = g_fb + (size_t)y * RB_NATIVE_W;
        volatile uint16_t *dst =
            X68K_GVRAM + (size_t)dst_y * (size_t)(RB_NATIVE_W / 2);
        int x;
        for (x = 0; x < RB_NATIVE_W; x += 2) {
            uint16_t packed = (uint16_t)((src[x] << 8) | src[x + 1]);
            dst[x / 2] = packed;
        }
    }
    (void)X68K_TVRAM;
    (void)X68K_SPRITE;
}

uint32_t sys_ticks_ms(void)
{
    /* Timekeeper: 100 Hz IOCS clock → approximate ms. */
    return (uint32_t)_iocs_timeadrs() * 10u;
}

#else /* !RB_PLATFORM_X68K */

/* Silence empty translation unit when accidentally listed in SDL builds. */
typedef int rb_video_x68k_placeholder;

#endif /* RB_PLATFORM_X68K */
