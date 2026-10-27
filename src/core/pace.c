#include "pace.h"

int rb_pace_due(uint64_t now, uint64_t *next, uint64_t period,
                int max_catch, int *slowed)
{
    int steps = 0;

    if (!next) {
        return 0;
    }
    if (period == 0u) {
        period = 1u;
    }
    if (max_catch < 1) {
        max_catch = 1;
    }
    if (slowed) {
        *slowed = 0;
    }
    if (*next == 0u) {
        *next = now;
    }
    while (now >= *next && steps < max_catch) {
        steps++;
        *next += period;
    }
    if (steps == max_catch && now >= *next) {
        if (slowed) {
            *slowed = 1;
        }
        /* Still late after the bounded repay. Drop the rest of the
         * debt so the next frame is one period from now. Running the
         * cap forever would play faster than the capture; skipping the
         * steps already counted would jump the simulation. */
        *next = now + period;
    }
    return steps;
}
