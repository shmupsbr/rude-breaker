#ifndef RB_DEMO_H
#define RB_DEMO_H

#include <stdint.h>

/* Attract-mode demo play: one input recording per stage, made for this
 * port (no game data). Frame 0 is the first VM frame after the stage
 * is entered from a clean reset (vm_reset_play), the state the
 * recordings were made in. */
#define DEMO_FRAMES 920u

/* RB_BTN_* bits held on that frame of the stage's recording. */
uint16_t demo_buttons(int stage, unsigned frame);

#endif
