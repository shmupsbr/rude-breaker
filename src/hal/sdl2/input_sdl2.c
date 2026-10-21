/**
 * SDL2 keyboard and gamepad.
 *
 * The game word is the loader's poll at 0x7846. Arrows and the keypad
 * match the PC-98 clusters (UP/KP8, LEFT/KP4, RIGHT/KP6, DOWN/KP2).
 * Z, Space, and KP0 are fire. X, N, M, and Return are the mode button.
 * Shift is the bit forced into the edge. Escape is bit 0x80. F10 quits.
 * WASD is an extra cluster; it is not the PC-98 matrix.
 */
#include "hal.h"

#include <SDL.h>
#include <string.h>

/* Declared in video_sdl2.c */
extern SDL_Window *g_rb_window;

static SDL_GameController *g_pad;

static void bind_pad(void)
{
    int i;
    int n;

    if (g_pad && SDL_GameControllerGetAttached(g_pad)) {
        return;
    }
    if (g_pad) {
        SDL_GameControllerClose(g_pad);
        g_pad = NULL;
    }
    n = SDL_NumJoysticks();
    for (i = 0; i < n; i++) {
        if (SDL_IsGameController(i)) {
            g_pad = SDL_GameControllerOpen(i);
            if (g_pad) {
                return;
            }
        }
    }
}

static void poll_pad(RbInput *out)
{
    Sint16 ax;
    Sint16 ay;

    bind_pad();
    if (!g_pad) {
        return;
    }
    ax = SDL_GameControllerGetAxis(g_pad, SDL_CONTROLLER_AXIS_LEFTX);
    ay = SDL_GameControllerGetAxis(g_pad, SDL_CONTROLLER_AXIS_LEFTY);
    if (SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_DPAD_UP) ||
        ay < -16000) {
        out->buttons |= RB_BTN_UP;
        out->axis_y = -1;
    }
    if (SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_DPAD_DOWN) ||
        ay > 16000) {
        out->buttons |= RB_BTN_DOWN;
        out->axis_y = 1;
    }
    if (SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_DPAD_LEFT) ||
        ax < -16000) {
        out->buttons |= RB_BTN_LEFT;
        out->axis_x = -1;
    }
    if (SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) ||
        ax > 16000) {
        out->buttons |= RB_BTN_RIGHT;
        out->axis_x = 1;
    }
    /* South is fire, east is mode, west is the forced edge. */
    if (SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_A)) {
        out->buttons |= RB_BTN_A;
    }
    if (SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_B)) {
        out->buttons |= RB_BTN_B;
    }
    if (SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_X)) {
        out->buttons |= RB_BTN_C;
    }
    if (SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_START)) {
        out->buttons |= RB_BTN_START;
    }
    if (SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_BACK)) {
        out->buttons |= RB_BTN_QUIT;
    }
}

void sys_read_input(RbInput *out)
{
    SDL_Event ev;
    const Uint8 *keys;

    memset(out, 0, sizeof(*out));

    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            out->buttons |= RB_BTN_QUIT;
        }
    }

    keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W] ||
        keys[SDL_SCANCODE_KP_8]) {
        out->buttons |= RB_BTN_UP;
        out->axis_y = -1;
    }
    if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S] ||
        keys[SDL_SCANCODE_KP_2]) {
        out->buttons |= RB_BTN_DOWN;
        out->axis_y = 1;
    }
    if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A] ||
        keys[SDL_SCANCODE_KP_4]) {
        out->buttons |= RB_BTN_LEFT;
        out->axis_x = -1;
    }
    if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D] ||
        keys[SDL_SCANCODE_KP_6]) {
        out->buttons |= RB_BTN_RIGHT;
        out->axis_x = 1;
    }
    if (keys[SDL_SCANCODE_Z] || keys[SDL_SCANCODE_SPACE] ||
        keys[SDL_SCANCODE_KP_0]) {
        out->buttons |= RB_BTN_A;
    }
    if (keys[SDL_SCANCODE_X] || keys[SDL_SCANCODE_N] ||
        keys[SDL_SCANCODE_M] || keys[SDL_SCANCODE_RETURN] ||
        keys[SDL_SCANCODE_KP_ENTER]) {
        out->buttons |= RB_BTN_B;
    }
    if (keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT]) {
        out->buttons |= RB_BTN_C;
    }
    if (keys[SDL_SCANCODE_ESCAPE]) {
        out->buttons |= RB_BTN_START;
    }
    if (keys[SDL_SCANCODE_F10]) {
        out->buttons |= RB_BTN_QUIT;
    }
    if (keys[SDL_SCANCODE_1]) {
        out->buttons |= RB_BTN_1;
    }
    if (keys[SDL_SCANCODE_2]) {
        out->buttons |= RB_BTN_2;
    }
    if (keys[SDL_SCANCODE_3]) {
        out->buttons |= RB_BTN_3;
    }
    if (keys[SDL_SCANCODE_4]) {
        out->buttons |= RB_BTN_4;
    }
    poll_pad(out);
    (void)g_rb_window;
}
