#include "hal.h"

#include <stdio.h>

int sys_play_sound(uint16_t id, uint8_t vol)
{
    (void)id;
    (void)vol;
    return 0;
}

int sys_play_music(uint16_t track_id)
{
    (void)track_id;
    return 0;
}

void sys_stop_music(void)
{
}

int sys_audio_bind(const char *dir)
{
    (void)dir;
    return 0;
}

void sys_set_rhythm_override(const char *path)
{
    (void)path;
}

void sys_fade_music(uint8_t speed)
{
    (void)speed;
}

void sys_message(const char *title, const char *text)
{
    fprintf(stderr, "%s\n%s\n", title ? title : "", text ? text : "");
}

void sys_draw_sprite(const RbSprite *spr)
{
    (void)spr;
}

void sys_draw_rect(const RbRect *rect, uint32_t rgba)
{
    (void)rect;
    (void)rgba;
}

void sys_draw_text(int16_t x, int16_t y, const char *utf8, uint32_t rgba)
{
    (void)x;
    (void)y;
    (void)utf8;
    (void)rgba;
}

uint32_t *sys_frame_pixels(void)
{
    return NULL;
}

void sys_clear(uint8_t r, uint8_t g, uint8_t b)
{
    (void)r;
    (void)g;
    (void)b;
}
