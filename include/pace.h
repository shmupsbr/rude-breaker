/**
 * Fixed-step logic clock.
 *
 * The step count is how many VM frames are due at `now`. A short hitch
 * is repaid up to max_catch steps so the average rate stays on the
 * period. Debt past that cap is dropped: the game slows down instead of
 * queuing a burst that would skip ahead through play.
 */
#ifndef RUDEBREAKER_PACE_H
#define RUDEBREAKER_PACE_H

#include <stdint.h>

/* Returns 0 when `now` is still before *next. Otherwise 1..max_catch.
 * *next of 0 starts the clock at `now`. *slowed is set when the debt
 * was larger than max_catch and the extra time was abandoned. */
int rb_pace_due(uint64_t now, uint64_t *next, uint64_t period,
                int max_catch, int *slowed);

#endif /* RUDEBREAKER_PACE_H */
