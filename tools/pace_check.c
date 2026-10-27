/* The logic period is 3/92 s. Refresh only changes how long a blocking
 * present would take. With vsync off that wait is zero, so every rate
 * below produces the same step count. A 50 ms machine must run slow
 * rather than skip steps or exceed the capture rate. */
#include "pace.h"

#include <stdio.h>

#define FREQ 1000000ull
#define SPAN 10000000ull

static uint64_t period_of(void)
{
    return (FREQ * 3ull) / 92ull;
}

static void simulate(const char *name, unsigned work_us, unsigned present_block_us,
                     unsigned hitch_at, unsigned hitch_us)
{
    uint64_t period = period_of();
    uint64_t next = 0;
    uint64_t t = 0;
    uint64_t steps = 0;
    uint64_t slowed = 0;
    uint64_t frames = 0;
    int hitched = 0;

    while (t < SPAN && frames < 1000000ull) {
        int slow = 0;
        int n;

        t += work_us;
        if (!hitched && hitch_us && t >= hitch_at) {
            t += hitch_us;
            hitched = 1;
        }
        if (t < next) {
            t = next;
        }
        n = rb_pace_due(t, &next, period, 4, &slow);
        if (n < 1) {
            n = 1;
            next = t + period;
        }
        steps += (unsigned)n;
        if (slow) {
            slowed++;
        }
        t += present_block_us;
        frames++;
    }
    {
        double hz = (double)steps * (double)FREQ / (double)t;
        double want = 92.0 / 3.0;

        printf("%-12s work=%5u block=%5u steps=%4llu hz=%7.3f ratio=%5.3f slow=%llu\n",
               name, work_us, present_block_us, (unsigned long long)steps,
               hz, hz / want, (unsigned long long)slowed);
    }
}

int main(void)
{
    printf("logic period %.3f ms (%.3f Hz)\n", 3000.0 / 92.0, 92.0 / 3.0);
    /* present_block is one vblank, the cost if vsync were left on.
     * The pacer still sleeps to the logic period, so the ratio stays 1
     * while a vblank is shorter than that period. */
    simulate("50Hz", 1000, 20000, 0, 0);
    simulate("60Hz", 1000, 16667, 0, 0);
    simulate("75Hz", 1000, 13333, 0, 0);
    simulate("120Hz", 1000, 8333, 0, 0);
    simulate("144Hz", 1000, 6944, 0, 0);
    simulate("165Hz", 1000, 6061, 0, 0);
    simulate("240Hz", 1000, 4167, 0, 0);
    simulate("vsync-off", 1000, 0, 0, 0);
    simulate("slow-50ms", 50000, 0, 0, 0);
    simulate("hitch-100ms", 1000, 0, 5000000u, 100000u);
    simulate("hitch-400ms", 1000, 0, 5000000u, 400000u);
    return 0;
}
