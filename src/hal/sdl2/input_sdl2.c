/**
 * Phase 1 input backend — keyboard + optional gamecontroller.
 */
#include "hal.h"

#include <SDL.h>
#include <string.h>

/* Declared in video_sdl2.c */
extern SDL_Window *g_rb_window;

void sys_read_input(RbInput *out)
{
    SDL_Event ev;
    const Uint8 *keys;
    memset(out, 0, sizeof(*out));

    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            out->buttons |= RB_BTN_QUIT;
        }
        if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) {
            out->buttons |= RB_BTN_QUIT;
        }
    }

    keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W]) {
        out->buttons |= RB_BTN_UP;
        out->axis_y = -1;
    }
    if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S]) {
        out->buttons |= RB_BTN_DOWN;
        out->axis_y = 1;
    }
    if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A]) {
        out->buttons |= RB_BTN_LEFT;
        out->axis_x = -1;
    }
    if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) {
        out->buttons |= RB_BTN_RIGHT;
        out->axis_x = 1;
    }
    if (keys[SDL_SCANCODE_Z] || keys[SDL_SCANCODE_J]) {
        out->buttons |= RB_BTN_A;
    }
    if (keys[SDL_SCANCODE_X] || keys[SDL_SCANCODE_K]) {
        out->buttons |= RB_BTN_B;
    }
    if (keys[SDL_SCANCODE_C] || keys[SDL_SCANCODE_L]) {
        out->buttons |= RB_BTN_C;
    }
    if (keys[SDL_SCANCODE_RETURN] || keys[SDL_SCANCODE_SPACE]) {
        out->buttons |= RB_BTN_START;
    }

    (void)g_rb_window;
}
