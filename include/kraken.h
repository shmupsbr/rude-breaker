/**
 * Kraken, the true last boss.
 *
 * Black Label only. Kraken starts when two whole loops have each been
 * cleared with every secret medal and no death. The loops need not be
 * the first two, and they need not be consecutive: a failed loop drops
 * only itself. The stage 5 core then falls off the bottom, the white
 * flash plays, and three seconds later that core returns with four
 * arms. The hull cannot be hurt until those arms are gone.
 *
 * KRAKEN_DEBUG (off unless the debug make target defines it) adds F9,
 * which plays that appearance on stage 5 without the medal check, and
 * F10, which dumps live sprite ids. Delete the debug target and the
 * KRAKEN_DEBUG blocks to drop both before a release build.
 */
#ifndef RUDEBREAKER_KRAKEN_H
#define RUDEBREAKER_KRAKEN_H

#include "vm.h"

void kraken_reset(void);
/* fair is 1 for a run from the title with no continue and no practice.
 * black is 1 only in Black Label. Kraken does not exist in other modes. */
void kraken_before(RbVm *vm, int deaths, int fair, int loop, int stage, int paused,
                   int black);
void kraken_after(RbVm *vm, int stage, int paused);
void kraken_draw(RbVm *vm, const RbGfx *gfx, uint32_t *px, int w, int h);
int  kraken_active(void);

#if KRAKEN_DEBUG
void kraken_debug_arm(void);
int  kraken_debug_armed(void);
/* F10 in the debug build: one stderr line per live object. */
void kraken_dump_screen(const RbVm *vm);
#endif

#endif
