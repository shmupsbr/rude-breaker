/**
 * Pure checks for Kraken shots and the core hit box.
 * No VM, no input, no environment variables.
 */
#ifndef RUDEBREAKER_KRAKEN_HIT_H
#define RUDEBREAKER_KRAKEN_HIT_H

#include <stdint.h>

/* Damage taken from one connecting shot. core_max is Kraken's own HP. */
int kr_shot_damage(int core_max);

/* How many shots of `dmg` reduce hp to 0. */
int kr_hits_to_zero(int hp, int dmg);

/* 1 if (x,y) is inside the stage 5 core sprite (3 by 5 cells). */
int kr_core_hit(int hx, int hy, int x, int y);

/* 1 if this entity is a player bullet, not the ship, an option, or
 * the shot controller. w and h are the sprite size in cells. */
int kr_is_projectile(unsigned type, unsigned kind, unsigned origin,
                     unsigned seg, int w, int h);

/* One loop of Black Label. banked is how many earlier loops already
 * qualified. medals/deaths are this loop only. need is every secret
 * medal in one five-stage loop. */
typedef struct KrLoops {
    int banked;
    int medals;
    int deaths;
    int need;
} KrLoops;

/* This loop had no death and every secret medal. */
int kr_loop_clean(const KrLoops *k);
/* The loop in progress is the second qualifying one, so Kraken starts. */
int kr_tlb_ready(const KrLoops *k);
/* The loop ended without Kraken. A clean loop is banked. A failed one
 * is dropped and does not change banked. Counters for the next loop
 * are cleared. */
void kr_finish_loop(KrLoops *k);

/* 1 when the first 12 pattern bytes are the stage 5 core frame. */
int kr_pat_is_core(const uint8_t *b, int n);

/* F9 may start only with stage 5, the live core bank, that pattern,
 * a painted background, the ship, and the weapon controller. */
int kr_f9_ready(int chunk, int live_core, int pat_ok, int bg_lit,
                int ship, int weapon);

/* 0: not our death. 1: Kraken where the core is. 2: Black Label fall. */
int kr_core_fate(int black, int tlb_ready, int hp_gone);

#endif
