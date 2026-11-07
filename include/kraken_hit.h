/**
 * Pure checks for Kraken shots and the core hit box.
 * No VM, no input, no environment variables.
 */
#ifndef RUDEBREAKER_KRAKEN_HIT_H
#define RUDEBREAKER_KRAKEN_HIT_H

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

#endif
