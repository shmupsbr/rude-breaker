/**
 * Phase 1 audio backend — stub mixer until FPLAY/SONG.DAT is interpreted.
 */
#include "hal.h"

#include <SDL.h>
#include <stdio.h>

static int g_audio_ok;
static uint16_t g_current_track = 0xFFFFu;

int sys_play_sound(uint16_t id, uint8_t vol)
{
    (void)vol;
    if (!g_audio_ok) {
        /* Lazy open: video_sdl2 already called SDL_Init with AUDIO. */
        g_audio_ok = 1;
    }
    /* TODO: map id to PC-98 SSG / PCM once FPLAY tables are ported. */
    (void)id;
    return 0;
}

int sys_play_music(uint16_t track_id)
{
    g_current_track = track_id;
    fprintf(stderr, "audio_sdl2: play music track %u (stub)\n",
            (unsigned)track_id);
    return 0;
}

void sys_stop_music(void)
{
    if (g_current_track != 0xFFFFu) {
        fprintf(stderr, "audio_sdl2: stop music\n");
    }
    g_current_track = 0xFFFFu;
}
