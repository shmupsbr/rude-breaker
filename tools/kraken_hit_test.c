/* Logic checks for Kraken damage and what counts as a player bullet.
 * No game data and no environment variables. */
#include "kraken_hit.h"

#include <stdio.h>
#include <string.h>

static int g_fail;

static void expect(int cond, const char *msg)
{
    if (!cond) {
        fprintf(stderr, "kraken_hit_test: FAIL %s\n", msg);
        g_fail = 1;
    }
}

int main(void)
{
    int dmg = kr_shot_damage(480);
    int arm;
    int i;
    int hp;

    expect(dmg == 15, "default shot damage is 15");
    arm = 480 / 2;
    expect(kr_hits_to_zero(arm, dmg) == 16, "one arm dies in 16 hits");
    expect(kr_hits_to_zero(480, dmg) == 32, "the core dies in 32 hits");

    /* After the arms are gone, later shots still count. */
    hp = 480;
    for (i = 0; i < 16 * 4; i++) {
        /* arm hits do not touch the core */
    }
    hp -= dmg;
    expect(hp == 465, "first core hit leaves 465");
    while (hp > 0) {
        hp -= dmg;
    }
    expect(hp <= 0, "core reaches 0");
    expect(kr_hits_to_zero(465, dmg) == 31, "31 shots remain after the first");

    expect(!kr_core_hit(320, 96, 320, 330), "a shot at the ship is not a core hit");
    expect(kr_core_hit(320, 96, 320, 120), "a shot under the core sprite hits");
    expect(kr_core_hit(320, 96, 340, 96), "the side of the 48px core hits");
    expect(!kr_core_hit(320, 96, 360, 96), "well outside the core misses");

    expect(!kr_is_projectile(0x8000u, 0, 0x62E3u, 26, 0, 0),
           "the shot controller is not a bullet");
    expect(!kr_is_projectile(0x8000u, 0, 0x6400u, 26, 0, 0),
           "a script with no sprite is not a bullet");
    expect(kr_is_projectile(0x1000u, 0, 0x6331u, 26, 1, 1),
           "a small 0x1000 sprite is a bullet");
    expect(kr_is_projectile(0x0004u, 0, 0x1000u, 26, 1, 2),
           "a ship-sheet bullet is a bullet");
    expect(!kr_is_projectile(0x1000u, 0x0012u, 0x4F6Eu, 26, 2, 2),
           "the ship is not a bullet");
    expect(!kr_is_projectile(0x0080u, 0, 0xCE11u, 0, 1, 1),
           "a Kraken shot is not a player bullet");

    /* Two clean loops are required. A failed loop does not erase one
     * that already counted, and it only spoils itself. */
    {
        KrLoops k;
        const int need = 50;

        memset(&k, 0, sizeof k);
        k.need = need;

        k.medals = need;
        expect(!kr_tlb_ready(&k), "one clean loop does not start Kraken");
        kr_finish_loop(&k);
        expect(k.banked == 1 && k.medals == 0 && k.deaths == 0,
               "the first clean loop is banked");
        expect(!kr_tlb_ready(&k), "still no Kraken with only one banked");

        /* perfect, then bad, then perfect. */
        k.medals = need;
        k.deaths = 1;
        expect(!kr_tlb_ready(&k), "a death blocks this loop");
        kr_finish_loop(&k);
        expect(k.banked == 1, "a death does not erase the banked loop");

        k.medals = need;
        k.deaths = 0;
        expect(kr_tlb_ready(&k), "perfect, bad, perfect starts Kraken");

        /* bad, then perfect, then perfect. */
        memset(&k, 0, sizeof k);
        k.need = need;
        k.deaths = 1;
        k.medals = need;
        kr_finish_loop(&k);
        expect(k.banked == 0, "a first-loop death banks nothing");
        k.medals = need;
        kr_finish_loop(&k);
        expect(k.banked == 1 && !kr_tlb_ready(&k), "the first clean loop waits");
        k.medals = need;
        expect(kr_tlb_ready(&k), "bad, perfect, perfect starts Kraken");

        /* A missed medal spoils only that loop. */
        memset(&k, 0, sizeof k);
        k.need = need;
        k.medals = need;
        kr_finish_loop(&k);
        k.medals = need - 1;
        expect(!kr_loop_clean(&k), "a missed medal spoils this loop");
        kr_finish_loop(&k);
        expect(k.banked == 1, "a missed medal does not erase the banked loop");
        k.medals = need;
        expect(kr_tlb_ready(&k), "a later clean loop still completes the pair");
    }

    {
        static const uint8_t core[12] = {
            26, 27, 28, 74, 75, 76, 90, 91, 92, 106, 107, 108
        };
        uint8_t bad[12];

        memcpy(bad, core, sizeof bad);
        bad[3] = 0;
        expect(kr_pat_is_core(core, 12), "core pattern bytes match");
        expect(!kr_pat_is_core(bad, 12), "a different pattern is rejected");
        expect(!kr_f9_ready(2, 1, 1, 200, 1, 1, 380, 86, 3, 5, 640, 400),
               "stage 1 is not accepted");
        expect(!kr_f9_ready(6, 0, 1, 200, 1, 1, 380, 86, 3, 5, 640, 400),
               "no live core is not accepted");
        expect(!kr_f9_ready(6, 1, 0, 200, 1, 1, 380, 86, 3, 5, 640, 400),
               "wrong pattern is not accepted");
        expect(!kr_f9_ready(6, 1, 1, 10, 1, 1, 380, 86, 3, 5, 640, 400),
               "an unpainted background is not accepted");
        expect(!kr_f9_ready(6, 1, 1, 200, 1, 0, 380, 86, 3, 5, 640, 400),
               "no weapon is not accepted");
        expect(kr_f9_ready(6, 1, 1, 200, 1, 1, 380, 86, 3, 5, 640, 400),
               "stage 5 core context is accepted");
        expect(!kr_f9_ready(6, 1, 1, 200, 0, 1, 380, 86, 3, 5, 640, 400),
               "no ship is not accepted");
        expect(!kr_sprite_inside(320, -123, 3, 5, 640, 400),
               "y=-123 is above the screen");
        expect(!kr_f9_ready(6, 1, 1, 10304, 1, 1, 320, -123, 3, 5, 640, 400),
               "the reported offscreen frame is not ready");
        expect(!kr_sprite_inside(320, 20, 3, 5, 640, 400),
               "a clipped top is not fully visible");
        expect(kr_sprite_inside(380, 86, 3, 5, 640, 400),
               "the fight position is fully visible");
        expect(kr_core_fate(1, 1, 1) == 1, "a counted loop stays in place");
        expect(kr_core_fate(1, 0, 1) == 2, "a failed loop still falls");
        expect(kr_core_fate(0, 1, 1) == 0, "other modes do not transform");
        expect(kr_core_fate(1, 1, 0) == 0, "a living core is not transformed");
        {
            int mode;
            int opened[3];

            for (mode = 0; mode < 3; mode++) {
                KrF9Watch w;
                int frame;
                int got = -1;

                memset(&w, 0, sizeof w);
                for (frame = 0; frame < 40; frame++) {
                    int x = 320;
                    int y = -123;
                    int ready;

                    if (frame >= 4 && frame < 8) {
                        y = 20;
                    } else if (frame >= 8 && frame < 12) {
                        y = 70 + (frame - 8) * 4;
                    } else if (frame >= 12) {
                        x = 380;
                        y = 86;
                    }
                    ready = kr_f9_ready(6, 1, 1, 10304, 1, 1, x, y, 3, 5, 640, 400);
                    if (kr_f9_poll(mode, &w, ready, x, y, KR_F9_SLACK, KR_F9_SETTLE) &&
                        got < 0) {
                        got = frame;
                    }
                }
                opened[mode] = got;
            }
            expect(opened[0] == 12 + KR_F9_SETTLE - 1, "opens on the settled frame");
            expect(opened[0] > 0, "does not open on the offscreen frame");
            expect(opened[0] == opened[1] && opened[1] == opened[2],
                   "original, infinite and black label open on the same frame");
        }
    }

    if (g_fail) {
        return 1;
    }
    printf("kraken_hit_test: ok\n");
    return 0;
}
