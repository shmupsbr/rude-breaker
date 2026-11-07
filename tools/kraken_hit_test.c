/* Logic checks for Kraken damage and what counts as a player bullet.
 * No game data and no environment variables. */
#include "kraken_hit.h"

#include <stdio.h>

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

    if (g_fail) {
        return 1;
    }
    printf("kraken_hit_test: ok\n");
    return 0;
}
