/**
 * Rude Breaker — program entry.
 *
 * Data comes from loose files (SGM1.DAT, FPLAY.COM, SONG.DAT) or from a
 * PC-98 disk image. Search order: the command argument, RUDEBR_ASSETS,
 * the executable's directory, <exe>/RUDEBR, then assets/raw/RUDEBR.
 * An image may be a raw .hdm, .fdi, .d88, .hdi, .nhd, or a zip of one.
 */
#include "diskimg.h"
#include "exepath.h"
#include "game.h"
#include "hal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int loose_sgm1(const char *dir)
{
    char path[1200];
    FILE *fp;
    int n;

    if (!dir || !dir[0]) {
        return 0;
    }
    n = snprintf(path, sizeof(path), "%s/SGM1.DAT", dir);
    if (n <= 0 || (size_t)n >= sizeof(path)) {
        return 0;
    }
    fp = fopen(path, "rb");
    if (fp) {
        fclose(fp);
        return 1;
    }
    n = snprintf(path, sizeof(path), "%s/RUDEBR/SGM1.DAT", dir);
    if (n <= 0 || (size_t)n >= sizeof(path)) {
        return 0;
    }
    fp = fopen(path, "rb");
    if (!fp) {
        return 0;
    }
    fclose(fp);
    return 1;
}

static int dir_usable(const char *dir)
{
    char image[1200];

    if (loose_sgm1(dir)) {
        return 1;
    }
    return disk_find_image(dir, image, sizeof(image)) == 0;
}

static void choose_source(int argc, char **argv, char *out, size_t n)
{
    const char *cli = NULL;
    const char *env;
    char exe[1024];
    char nested[1100];
    int have_exe = 0;
    int i;

    out[0] = 0;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--ym2608-rom") == 0) {
            if (i + 1 < argc) {
                sys_set_rhythm_override(argv[++i]);
            }
            continue;
        }
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            fprintf(stderr,
                    "usage: rudebreaker [folder-or-disk-image] [--ym2608-rom file]\n"
                    "\n"
                    "Put SGM1.DAT, FPLAY.COM, and SONG.DAT next to this program,\n"
                    "or drop a PC-98 disk image there. Images: .hdm .fdi .d88\n"
                    ".hdi .nhd, or a zip of one of those. RBRANK.DAT and\n"
                    "ym2608_adpcm_rom.bin are optional. SGC.COM, READJS.COM,\n"
                    "and RUDEBR.BAT are not read.\n");
            exit(0);
        }
        if (!cli) {
            cli = argv[i];
        }
    }

    if (cli && cli[0]) {
        snprintf(out, n, "%s", cli);
        return;
    }
    env = getenv("RUDEBR_ASSETS");
    if (env && env[0]) {
        snprintf(out, n, "%s", env);
        return;
    }
    if (rb_exe_dir(exe, sizeof(exe)) == 0) {
        have_exe = 1;
        if (dir_usable(exe)) {
            snprintf(out, n, "%s", exe);
            return;
        }
        snprintf(nested, sizeof(nested), "%s/RUDEBR", exe);
        if (dir_usable(nested)) {
            snprintf(out, n, "%s", nested);
            return;
        }
    }
    if (loose_sgm1("assets/raw/RUDEBR")) {
        snprintf(out, n, "%s", "assets/raw/RUDEBR");
        return;
    }
    if (have_exe) {
        snprintf(out, n, "%s", exe);
        return;
    }
    snprintf(out, n, "%s", "assets/raw/RUDEBR");
}

int main(int argc, char **argv)
{
    RbGame game;
    RbDisk disk;
    char source[1024];
    const char *stop;
    int rc = 0;

    choose_source(argc, argv, source, sizeof(source));
    fprintf(stderr, "rudebreaker: data directory: %s\n", source);
    disk_init(&disk);
    if (disk_open(source, &disk) != 0 && disk.label[0] == 0) {
        snprintf(disk.label, sizeof(disk.label), "%s", source);
    }

    if (sys_init("Rude Breaker") != 0) {
        fprintf(stderr, "sys_init failed\n");
        disk_free(&disk);
        return 1;
    }

    game_init(&game, &disk);
    disk_free(&disk);

    while (!game_wants_quit(&game)) {
        sys_read_input(&game.input);
        game_update(&game);
        game_render(&game);
        sys_wait_vblank(game_vsyncs(&game));
        sys_present();
    }

    if (game_data_missing()) {
        rc = 1;
    }
    stop = getenv("RUDEBR_STOP");
    if (stop && stop[0] && !game_data_missing() && !game_smoke_done()) {
        fprintf(stderr, "smoke: stop condition not reached (%s)\n", stop);
        rc = 3;
    }

    sys_shutdown();
    return rc;
}
