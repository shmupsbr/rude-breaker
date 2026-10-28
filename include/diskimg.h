/**
 * Loose disk files, or the same files read out of a PC-98 FAT12/FAT16 image.
 * Images: raw .hdm, Anex86 .fdi/.hdi, D88 .d88, T98-Next .nhd, or a zip of one.
 */
#ifndef RUDEBREAKER_DISKIMG_H
#define RUDEBREAKER_DISKIMG_H

#include <stddef.h>
#include <stdint.h>

typedef struct RbDisk {
    int      ok;          /* SGM1.DAT, FPLAY.COM, and SONG.DAT were all read */
    int      saw_image;   /* a disk image or a zip of one was opened */
    char     label[1024]; /* path named in the missing-files dialog */
    char     data_dir[1024];
    uint8_t *sgm1;
    size_t   sgm1_n;
    uint8_t *fplay;
    size_t   fplay_n;
    uint8_t *song;
    size_t   song_n;
    uint8_t *rank;
    size_t   rank_n;
} RbDisk;

void disk_init(RbDisk *d);
void disk_free(RbDisk *d);

/* path is a directory of loose files, a disk image, or a zip of an image.
 * Returns 0 when the three required files were read. */
int disk_open(const char *path, RbDisk *d);

/* First image or zip in dir whose FAT contains SGM1.DAT. Returns 0 and
 * writes the path when one is found. */
int disk_find_image(const char *dir, char *out, size_t n);

#endif
