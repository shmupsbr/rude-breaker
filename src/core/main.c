/**
 * Rude Breaker — program entry (Phase 1 / Phase 2 share this file).
 */
#include "game.h"
#include "hal.h"

#include <stdio.h>
#include <stdlib.h>

static const char *default_assets(void)
{
    const char *env = getenv("RUDEBR_ASSETS");
    if (env && env[0]) {
        return env;
    }
    return "assets/raw/RUDEBR";
}

int main(int argc, char **argv)
{
    RbGame game;
    const char *assets = (argc > 1) ? argv[1] : default_assets();

    if (sys_init("Rude Breaker") != 0) {
        fprintf(stderr, "sys_init failed\n");
        return 1;
    }

    game_init(&game, assets);

    while (!game_wants_quit(&game)) {
        sys_read_input(&game.input);
        game_update(&game);
        game_render(&game);
        sys_wait_vblank();
        sys_present();
    }

    sys_shutdown();
    return 0;
}
