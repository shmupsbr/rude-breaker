/**
 * Rude Breaker — program entry.
 *
 * Disk files are resolved in this order: an explicit directory argument,
 * RUDEBR_ASSETS, the executable's directory, <exe>/RUDEBR, then the
 * in-tree assets/raw/RUDEBR used while developing. The packaged binary
 * ships with none of those files.
 */
#include "exepath.h"
#include "game.h"
#include "hal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char g_asset_dir[1024];

static int file_readable(const char *path)
{
    FILE *fp = fopen(path, "rb");

    if (!fp) {
        return 0;
    }
    fclose(fp);
    return 1;
}

static int dir_has_sgm1(const char *dir)
{
    char path[1200];
    int n;

    if (!dir || !dir[0]) {
        return 0;
    }
    n = snprintf(path, sizeof(path), "%s/SGM1.DAT", dir);
    if (n <= 0 || (size_t)n >= sizeof(path)) {
        return 0;
    }
    return file_readable(path);
}

static void set_dir(const char *dir)
{
    snprintf(g_asset_dir, sizeof(g_asset_dir), "%s", dir ? dir : "");
}

static void choose_assets(int argc, char **argv)
{
    const char *cli = NULL;
    const char *env;
    char exe[1024];
    char nested[1100];
    int have_exe = 0;
    int i;

    g_asset_dir[0] = 0;
    exe[0] = 0;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--ym2608-rom") == 0) {
            if (i + 1 < argc) {
                sys_set_rhythm_override(argv[++i]);
            }
            continue;
        }
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            fprintf(stderr,
                    "usage: rudebreaker [data-directory] [--ym2608-rom file]\n"
                    "\n"
                    "Put SGM1.DAT, FPLAY.COM, and SONG.DAT from your own\n"
                    "Rude Breaker disk next to this program. RBRANK.DAT and\n"
                    "ym2608_adpcm_rom.bin are optional. SGC.COM, READJS.COM,\n"
                    "and RUDEBR.BAT are not read.\n");
            exit(0);
        }
        if (!cli) {
            cli = argv[i];
        }
    }

    if (cli && cli[0]) {
        set_dir(cli);
        return;
    }
    env = getenv("RUDEBR_ASSETS");
    if (env && env[0]) {
        set_dir(env);
        return;
    }
    if (rb_exe_dir(exe, sizeof(exe)) == 0) {
        have_exe = 1;
        if (dir_has_sgm1(exe)) {
            set_dir(exe);
            return;
        }
        snprintf(nested, sizeof(nested), "%s/RUDEBR", exe);
        if (dir_has_sgm1(nested)) {
            set_dir(nested);
            return;
        }
    }
    if (dir_has_sgm1("assets/raw/RUDEBR")) {
        set_dir("assets/raw/RUDEBR");
        return;
    }
    if (have_exe) {
        set_dir(exe);
        return;
    }
    set_dir("assets/raw/RUDEBR");
}

int main(int argc, char **argv)
{
    RbGame game;
    const char *stop;
    int rc = 0;

    choose_assets(argc, argv);
    fprintf(stderr, "rudebreaker: data directory: %s\n", g_asset_dir);

    if (sys_init("Rude Breaker") != 0) {
        fprintf(stderr, "sys_init failed\n");
        return 1;
    }

    game_init(&game, g_asset_dir);

    /* Same order as frame_loop at game image 0x933: poll input, run the
     * entity phase, draw, wait for vertical blank, then present. */
    while (!game_wants_quit(&game)) {
        sys_read_input(&game.input);
        game_update(&game);
        game_render(&game);
        sys_wait_vblank();
        sys_present();
    }

    stop = getenv("RUDEBR_STOP");
    if (stop && stop[0] && !game_smoke_done()) {
        fprintf(stderr, "smoke: stop condition not reached (%s)\n", stop);
        rc = 3;
    }

    sys_shutdown();
    return rc;
}
