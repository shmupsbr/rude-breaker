#ifndef RUDEBREAKER_ASSETS_H
#define RUDEBREAKER_ASSETS_H

#include "hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RB_SGM1_MAX_CHUNKS 38

typedef struct RB_PACKED RbSgm1Entry {
    uint16_t bank;
    uint16_t offset;
} RbSgm1Entry;

typedef struct RbSgm1Table {
    RbSgm1Entry entries[RB_SGM1_MAX_CHUNKS];
    uint16_t    count;
    uint16_t    _pad;
    uint32_t    abs_off[RB_SGM1_MAX_CHUNKS];
    uint32_t    size[RB_SGM1_MAX_CHUNKS];
} RbSgm1Table;

typedef struct RbAssets {
    uint8_t    *sgm1_data;
    size_t      sgm1_size;
    RbSgm1Table sgm1;
    uint8_t    *song_data;
    size_t      song_size;
    uint8_t    *rank_data;
    size_t      rank_size;
    int         loaded;
} RbAssets;

int  assets_load(RbAssets *a, const char *rudebr_dir);
int  assets_load_mem(RbAssets *a, const uint8_t *sgm1, size_t sgm1_n,
                     const uint8_t *song, size_t song_n,
                     const uint8_t *rank, size_t rank_n);
void assets_free(RbAssets *a);
const uint8_t *assets_sgm1_chunk(const RbAssets *a, uint16_t index,
                                 uint32_t *size_out);

#ifdef __cplusplus
}
#endif

#endif /* RUDEBREAKER_ASSETS_H */
