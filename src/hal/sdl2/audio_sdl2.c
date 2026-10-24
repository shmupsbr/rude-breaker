/**
 * SDL2 mixer. The samples come from FPLAY.COM driving a small YM2608.
 */
#include "hal.h"
#include "fplay.h"
#include "opna.h"

#include <SDL.h>
#include <stdio.h>
#include <string.h>

static SDL_AudioDeviceID g_dev;
static int g_opened;
static int g_paused;
static volatile int g_peak;

static void note_level(const int16_t *s, int frames)
{
    int n = frames * 2;
    int i;
    int peak = 0;

    for (i = 0; i < n; i++) {
        int a = s[i];

        if (a < 0) {
            a = -a;
        }
        if (a > peak) {
            peak = a;
        }
    }
    if (peak > g_peak) {
        g_peak = peak;
    }
}

static void audio_cb(void *userdata, Uint8 *stream, int len)
{
    int frames = len / (int)(2 * sizeof(int16_t));

    (void)userdata;
    if (frames <= 0) {
        return;
    }
    fplay_render((int16_t *)stream, frames);
    note_level((const int16_t *)stream, frames);
}

static void audio_resume(void)
{
    if (!g_paused) {
        return;
    }
    g_paused = 0;
    if (g_dev) {
        SDL_PauseAudioDevice(g_dev, 0);
    }
}

static void open_device(void)
{
    SDL_AudioSpec want;
    SDL_AudioSpec got;

    if (g_opened) {
        return;
    }
    memset(&want, 0, sizeof want);
    want.freq = 44100;
    want.format = AUDIO_S16SYS;
    want.channels = 2;
    want.samples = 1024;
    want.callback = audio_cb;
    g_dev = SDL_OpenAudioDevice(NULL, 0, &want, &got, 0);
    if (g_dev == 0) {
        fprintf(stderr, "audio_sdl2: open failed: %s\n", SDL_GetError());
        return;
    }
    SDL_PauseAudioDevice(g_dev, 0);
    g_opened = 1;
}

void sys_set_rhythm_override(const char *path)
{
    opna_set_rhythm_override(path);
}

int sys_audio_bind(const char *dir)
{
    int rc = fplay_bind(dir);

    open_device();
    return rc;
}

int sys_audio_bind_mem(const uint8_t *com, size_t com_n,
                       const uint8_t *song, size_t song_n,
                       const char *rhythm_dir)
{
    int rc = fplay_bind_mem(com, com_n, song, song_n, rhythm_dir);

    open_device();
    return rc;
}

int sys_play_sound(uint16_t id, uint8_t vol)
{
    (void)vol;
    if (!g_opened) {
        open_device();
    }
    audio_resume();
    if (g_dev) {
        SDL_LockAudioDevice(g_dev);
    }
    fplay_command(1, (uint8_t)id);
    if (g_dev) {
        SDL_UnlockAudioDevice(g_dev);
    }
    return 0;
}

int sys_play_music(uint16_t track_id)
{
    if (!g_opened) {
        open_device();
    }
    audio_resume();
    if (g_dev) {
        SDL_LockAudioDevice(g_dev);
    }
    fplay_command(0, (uint8_t)track_id);
    if (g_dev) {
        SDL_UnlockAudioDevice(g_dev);
    }
    fprintf(stderr, "audio_sdl2: play music track %u\n", (unsigned)track_id);
    return 0;
}

void sys_stop_music(void)
{
    audio_resume();
    if (g_dev) {
        SDL_LockAudioDevice(g_dev);
    }
    fplay_command(3, 0);
    if (g_dev) {
        SDL_UnlockAudioDevice(g_dev);
    }
}

int sys_audio_level(void)
{
    int p = g_peak;
    int bars;

    g_peak = (p * 2) / 3;
    if (p < 500) {
        return 0;
    }
    bars = p / 2800;
    if (bars > 10) {
        bars = 10;
    }
    return bars;
}

void sys_audio_pause(int on)
{
    if (!g_opened) {
        open_device();
    }
    g_paused = on ? 1 : 0;
    g_peak = 0;
    if (g_dev) {
        SDL_PauseAudioDevice(g_dev, g_paused);
    }
}

void sys_fade_music(uint8_t speed)
{
    if (g_dev) {
        SDL_LockAudioDevice(g_dev);
    }
    fplay_command(4, speed);
    if (g_dev) {
        SDL_UnlockAudioDevice(g_dev);
    }
}
