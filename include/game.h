#ifndef RUDEBREAKER_GAME_H
#define RUDEBREAKER_GAME_H

#include "hal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum RbGameState {
    RB_STATE_BOOT = 0,
    RB_STATE_TITLE,
    RB_STATE_PLAY,
    RB_STATE_RANKING,
    RB_STATE_QUIT
} RbGameState;

typedef struct RbGame {
    RbGameState state;
    RbPlayer    player;
    RbInput     input;
    RbInput     input_prev;
    uint32_t    frame;
    /* Native score is the dword at [bp+0x8E]/[bp+0x90]. */
    uint32_t    score;
    uint32_t    hiscore;
    uint16_t    stage;
    uint16_t    flags;
    /* Background scroll. Opcode 0x4B adds the stage speed into scroll_y
     * and wraps at 400. Row blit stays out of the core. */
    int16_t     scroll_y;
    int16_t     scroll_speed;
    char        asset_dir[512];
} RbGame;

void game_init(RbGame *g, const char *asset_dir);
void game_update(RbGame *g);
void game_render(const RbGame *g);
int  game_wants_quit(const RbGame *g);
/* 1 after RUDEBR_STOP's event was reached. Players never set that variable. */
int  game_smoke_done(void);

#ifdef __cplusplus
}
#endif

#endif /* RUDEBREAKER_GAME_H */
