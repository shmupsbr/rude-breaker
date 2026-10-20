/**
 * Load PC-98 extracted assets. Binary fields are little-endian.
 */
#include "assets.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int read_file(const char *path, uint8_t **out, size_t *out_size)
{
    FILE *fp;
    long sz;
    uint8_t *buf;

    fp = fopen(path, "rb");
    if (!fp) {
        return -1;
    }
    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return -1;
    }
    sz = ftell(fp);
    if (sz < 0) {
        fclose(fp);
        return -1;
    }
    rewind(fp);
    buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) {
        fclose(fp);
        return -1;
    }
    if (fread(buf, 1, (size_t)sz, fp) != (size_t)sz) {
        free(buf);
        fclose(fp);
        return -1;
    }
    fclose(fp);
    *out = buf;
    *out_size = (size_t)sz;
    return 0;
}

static void parse_sgm1_toc(RbAssets *a)
{
    uint16_t i;
    uint16_t n = 0;

    memset(&a->sgm1, 0, sizeof(a->sgm1));
    if (a->sgm1_size < 4) {
        return;
    }

    for (i = 0; i < RB_SGM1_MAX_CHUNKS; i++) {
        size_t pos = (size_t)i * 4u;
        uint16_t bank;
        uint16_t off;
        uint32_t abs_off;

        if (pos + 4u > a->sgm1_size) {
            break;
        }
        bank = read_u16_le(a->sgm1_data + pos);
        off = read_u16_le(a->sgm1_data + pos + 2);
        abs_off = (uint32_t)bank * 0x10000u + (uint32_t)off;

        /* Sentinel: absolute offset equals EOF (entry 37 on stock disk). */
        if (i > 0 && abs_off >= a->sgm1_size) {
            break;
        }
        if (bank == 0 && off == 0 && i > 0) {
            break;
        }

        a->sgm1.entries[n].bank = bank;
        a->sgm1.entries[n].offset = off;
        a->sgm1.abs_off[n] = abs_off;
        n++;
    }

    a->sgm1.count = n;
    for (i = 0; i < n; i++) {
        uint32_t end = (i + 1u < n) ? a->sgm1.abs_off[i + 1u]
                                    : (uint32_t)a->sgm1_size;
        a->sgm1.size[i] = (end > a->sgm1.abs_off[i])
                              ? (end - a->sgm1.abs_off[i])
                              : 0u;
    }
}

int assets_load(RbAssets *a, const char *rudebr_dir)
{
    char path[300];

    memset(a, 0, sizeof(*a));
    if (!rudebr_dir || !rudebr_dir[0]) {
        return -1;
    }

    snprintf(path, sizeof(path), "%s/SGM1.DAT", rudebr_dir);
    if (read_file(path, &a->sgm1_data, &a->sgm1_size) != 0) {
        return -1;
    }
    parse_sgm1_toc(a);

    snprintf(path, sizeof(path), "%s/SONG.DAT", rudebr_dir);
    (void)read_file(path, &a->song_data, &a->song_size);

    snprintf(path, sizeof(path), "%s/RBRANK.DAT", rudebr_dir);
    (void)read_file(path, &a->rank_data, &a->rank_size);

    a->loaded = 1;
    return 0;
}

void assets_free(RbAssets *a)
{
    free(a->sgm1_data);
    free(a->song_data);
    free(a->rank_data);
    memset(a, 0, sizeof(*a));
}

const uint8_t *assets_sgm1_chunk(const RbAssets *a, uint16_t index,
                                 uint32_t *size_out)
{
    if (!a || !a->loaded || index >= a->sgm1.count) {
        if (size_out) {
            *size_out = 0;
        }
        return NULL;
    }
    if (size_out) {
        *size_out = a->sgm1.size[index];
    }
    return a->sgm1_data + a->sgm1.abs_off[index];
}
