/**
 * Kraken, the true last boss.
 *
 * After loop 2's stage 5, if every medal from both loops was taken and
 * the ship never went down, the recurring mid-boss leaves, waits, and
 * comes back as its central hull with four arms. The hull cannot be
 * hurt until those arms are gone.
 *
 * KRAKEN_DEBUG (off unless the debug make target defines it) adds an
 * F9 shortcut and an F10 dump of every live object's sprite ids.
 * Delete the debug target and the KRAKEN_DEBUG blocks to drop both
 * before a release build.
 */
#ifndef RUDEBREAKER_KRAKEN_H
#define RUDEBREAKER_KRAKEN_H

#include "vm.h"

void kraken_reset(void);
/* fair is 1 for a run from the title with no continue and no practice. */
void kraken_before(RbVm *vm, int deaths, int fair, int loop, int stage, int paused);
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
