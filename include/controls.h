/**
 * Keyboard bits always reach the ship. Pad bits do only while the
 * joystick flag is on. A stage reset used to clear that flag and leave
 * a connected pad ignored.
 */
#ifndef RUDEBREAKER_CONTROLS_H
#define RUDEBREAKER_CONTROLS_H

#include <stdint.h>

static inline uint16_t rb_control_bits(uint16_t keys, uint16_t pad, int joy_on)
{
    if (!joy_on) {
        return keys;
    }
    return (uint16_t)(keys | pad);
}

#endif
