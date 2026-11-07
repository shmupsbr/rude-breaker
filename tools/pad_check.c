/* Keyboard and a virtual SDL joystick through the production poll.
 * No game data and no physical pad. */
#include "hal.h"

#include <SDL.h>
#include <stdio.h>
#include <string.h>

SDL_Window *g_rb_window;

static int g_fail;

static void expect(int cond, const char *msg)
{
    if (!cond) {
        fprintf(stderr, "pad_check: FAIL %s\n", msg);
        g_fail = 1;
    }
}

static void prime(SDL_Joystick *joy)
{
    SDL_JoystickSetVirtualAxis(joy, 0, 0);
    SDL_JoystickSetVirtualAxis(joy, 1, -30000);
    SDL_JoystickSetVirtualButton(joy, 0, 1);
    SDL_JoystickUpdate();
}

int main(void)
{
    RbInput in;
    int dev;
    SDL_Joystick *joy;

    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_JOYSTICK) != 0) {
        fprintf(stderr, "pad_check: SDL_Init %s\n", SDL_GetError());
        return 1;
    }
    dev = SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_GAMECONTROLLER, 2, 4, 0);
    if (dev < 0) {
        fprintf(stderr, "pad_check: no virtual joystick (%s)\n", SDL_GetError());
        return 1;
    }
    joy = SDL_JoystickOpen(dev);
    if (!joy) {
        fprintf(stderr, "pad_check: open %s\n", SDL_GetError());
        return 1;
    }
    prime(joy);
    expect(SDL_JoystickGetAxis(joy, 1) == -30000, "virtual axis is armed");
    expect(SDL_JoystickGetButton(joy, 0) == 1, "virtual button is armed");
    sys_read_input(&in);
    expect(sys_joy_present(), "virtual joystick is attached");
    expect(in.pad == 1, "poll reports a pad");
    expect((in.pad_buttons & RB_BTN_UP) != 0, "stick up is up");
    expect((in.pad_buttons & RB_BTN_A) != 0, "button 0 is fire");
    expect((in.buttons & RB_BTN_A) == 0, "the pad fire is not a keyboard bit");

    SDL_JoystickClose(joy);
    SDL_JoystickDetachVirtual(dev);
    sys_read_input(&in);
    expect(!sys_joy_present(), "unplug drops the pad");
    dev = SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_UNKNOWN, 2, 4, 0);
    expect(dev >= 0, "replug creates a pad");
    if (dev >= 0) {
        joy = SDL_JoystickOpen(dev);
        if (joy) {
            prime(joy);
        }
        sys_read_input(&in);
        expect(sys_joy_present(), "replug is picked up on the next poll");
        expect((in.pad_buttons & RB_BTN_A) != 0, "replugged fire still reads");
        if (joy) {
            SDL_JoystickClose(joy);
        }
        SDL_JoystickDetachVirtual(dev);
    }
    if (g_rb_window) {
        SDL_DestroyWindow(g_rb_window);
    }
    SDL_Quit();
    if (g_fail) {
        return 1;
    }
    printf("pad_check: ok\n");
    return 0;
}
