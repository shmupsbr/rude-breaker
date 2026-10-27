/**
 * SDL2 mixer. The samples come from FPLAY.COM driving a small YM2608.
 */
#include "hal.h"
#include "fplay.h"
#include "opna.h"

#include <SDL.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

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

/* Spectrum for the Sound Test VU. One band-pass biquad per band on the
 * mono mix, log-spaced from 50 Hz to 14 kHz. The callback keeps the
 * loudest RMS of each band since the last read. */
#define VU_BANDS 16
typedef struct VuBiquad {
    float b0, b2, a1, a2;
    float x1, x2, y1, y2;
} VuBiquad;
static VuBiquad g_vu[VU_BANDS];
static float g_vu_acc[VU_BANDS];
static int g_vu_rate;

static void vu_setup(int rate)
{
    int i;

    if (rate <= 0) {
        rate = 44100;
    }
    g_vu_rate = rate;
    for (i = 0; i < VU_BANDS; i++) {
        double f = 50.0 * pow(14000.0 / 50.0, (double)i / (VU_BANDS - 1));
        double q = 2.2;
        double w;
        double alpha;
        double a0;

        if (f > rate * 0.45) {
            f = rate * 0.45;
        }
        w = 2.0 * 3.14159265358979 * f / rate;
        alpha = sin(w) / (2.0 * q);
        a0 = 1.0 + alpha;
        memset(&g_vu[i], 0, sizeof g_vu[i]);
        g_vu[i].b0 = (float)(alpha / a0);
        g_vu[i].b2 = (float)(-alpha / a0);
        g_vu[i].a1 = (float)(-2.0 * cos(w) / a0);
        g_vu[i].a2 = (float)((1.0 - alpha) / a0);
    }
}

static void vu_feed(const int16_t *s, int frames)
{
    float sum[VU_BANDS];
    int i;
    int k;

    if (!g_vu_rate || frames <= 0) {
        return;
    }
    memset(sum, 0, sizeof sum);
    for (i = 0; i < frames; i++) {
        float x = ((float)s[i * 2] + (float)s[i * 2 + 1]) * (0.5f / 32768.0f);

        for (k = 0; k < VU_BANDS; k++) {
            VuBiquad *q = &g_vu[k];
            float y = q->b0 * x + q->b2 * q->x2 - q->a1 * q->y1 - q->a2 * q->y2;

            q->x2 = q->x1;
            q->x1 = x;
            q->y2 = q->y1;
            q->y1 = y;
            sum[k] += y * y;
        }
    }
    for (k = 0; k < VU_BANDS; k++) {
        float rms = sqrtf(sum[k] / (float)frames);

        if (rms > g_vu_acc[k]) {
            g_vu_acc[k] = rms;
        }
    }
}

int sys_audio_bands(float *out, int n)
{
    int i;

    if (!out || n <= 0) {
        return 0;
    }
    if (n > VU_BANDS) {
        n = VU_BANDS;
    }
    if (g_dev) {
        SDL_LockAudioDevice(g_dev);
    }
    for (i = 0; i < n; i++) {
        /* -48 dB .. -9 dB onto 0..1. */
        float v = g_vu_acc[i];
        float db = v > 1e-6f ? 20.0f * log10f(v) : -120.0f;
        float lv = (db + 48.0f) / 39.0f;

        if (lv < 0.0f) {
            lv = 0.0f;
        }
        if (lv > 1.0f) {
            lv = 1.0f;
        }
        out[i] = (g_paused || !g_opened) ? 0.0f : lv;
        g_vu_acc[i] = 0.0f;
    }
    if (g_dev) {
        SDL_UnlockAudioDevice(g_dev);
    }
    return n;
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
    vu_feed((const int16_t *)stream, frames);
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
    vu_setup(got.freq);
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
