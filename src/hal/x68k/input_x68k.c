/**
 * Phase 2 input backend — keyboard / joystick through IOCS.
 */
#include "hal.h"

#if defined(RB_PLATFORM_X68K)

#include <string.h>
#include <sys/iocs.h>

void sys_read_input(RbInput *out)
{
    int key;
    int joy;

    memset(out, 0, sizeof(*out));

    /* Non-blocking key sense. */
    key = _iocs_bitsns(0);
    if (key & 0x01) { /* UP (group dependent — refine with keymap table) */
        out->buttons |= RB_BTN_UP;
        out->axis_y = -1;
    }
    if (key & 0x02) {
        out->buttons |= RB_BTN_DOWN;
        out->axis_y = 1;
    }
    if (key & 0x04) {
        out->buttons |= RB_BTN_LEFT;
        out->axis_x = -1;
    }
    if (key & 0x08) {
        out->buttons |= RB_BTN_RIGHT;
        out->axis_x = 1;
    }

    /* Joystick 0 via IOCS. Merged only while the script's joystick
     * flag is set; the pad bit lets that row turn the flag on. */
    joy = _iocs_joyget(0);
    out->pad = 1;
    if (!(joy & 0x01)) {
        out->pad_buttons |= RB_BTN_UP;
    }
    if (!(joy & 0x02)) {
        out->pad_buttons |= RB_BTN_DOWN;
    }
    if (!(joy & 0x04)) {
        out->pad_buttons |= RB_BTN_LEFT;
    }
    if (!(joy & 0x08)) {
        out->pad_buttons |= RB_BTN_RIGHT;
    }
    if (!(joy & 0x20)) {
        out->pad_buttons |= RB_BTN_A;
    }
    if (!(joy & 0x40)) {
        out->pad_buttons |= RB_BTN_B;
    }

    /* ESC group — treat as quit. */
    if (_iocs_bitsns(0) & 0x80) {
        out->buttons |= RB_BTN_QUIT;
    }
}

#else

typedef int rb_input_x68k_placeholder;

#endif /* RB_PLATFORM_X68K */
