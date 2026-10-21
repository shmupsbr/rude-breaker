/**
 * Phase 2 audio backend — YM2151 (OPM) + MSM6258 ADPCM via IOCS.
 */
#include "hal.h"

#if defined(RB_PLATFORM_X68K)

#include <sys/iocs.h>

int sys_play_sound(uint16_t id, uint8_t vol)
{
    /*
     * Map logical sound ids to OPM registers / ADPCM slots once FPLAY
     * instrument tables are converted. Do not invent non-IOCS helpers.
     */
    (void)id;
    (void)vol;
    return 0;
}

int sys_play_music(uint16_t track_id)
{
    (void)track_id;
    /* TODO: feed converted SONG.DAT events to OPM via _iocs_opmdrv / registers. */
    return 0;
}

void sys_stop_music(void)
{
    /* Key-off all OPM channels when the driver is wired. */
}

int sys_audio_bind(const char *dir)
{
    (void)dir;
    return 0;
}

int sys_audio_bind_mem(const uint8_t *com, size_t com_n,
                       const uint8_t *song, size_t song_n,
                       const char *rhythm_dir)
{
    (void)com;
    (void)com_n;
    (void)song;
    (void)song_n;
    (void)rhythm_dir;
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

#else

typedef int rb_audio_x68k_placeholder;

#endif /* RB_PLATFORM_X68K */
