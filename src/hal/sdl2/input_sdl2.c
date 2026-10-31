/**
 * SDL2 keyboard and gamepad.
 *
 * The game word is the loader's poll at 0x7846. Arrows and the keypad
 * match the PC-98 clusters (UP/KP8, LEFT/KP4, RIGHT/KP6, DOWN/KP2).
 * Z, Space, and KP0 are fire. X, N, M, and Return are the mode button.
 * Shift is the bit forced into the edge. Escape is bit 0x80. F10 quits.
 * WASD is an extra cluster; it is not the PC-98 matrix.
 *
 * A saved bind replaces one action. The first attached device is used:
 * a mapped game controller when SDL recognizes it, otherwise the
 * joystick API. Unplug and replug are picked up on the next poll.
 */
#include "hal.h"

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Declared in video_sdl2.c */
extern SDL_Window *g_rb_window;

static SDL_GameController *g_pad;
static SDL_Joystick *g_joy;
static int g_dev_is_pad;
static int g_dev_index = -1;
static char g_joy_name[48];

typedef struct Bind {
    int kind;
    int a;
    int b;
    int pad; /* 1 = game controller, 0 = generic joystick */
} Bind;

static Bind g_bind[RB_ACT_COUNT];
static char g_label[RB_ACT_COUNT][24];

static int g_cap;
static int g_cap_result;
static int g_cap_kind;
static int g_cap_a;
static int g_cap_b;
static int g_cap_pad;

static const char *act_name(int action)
{
    static const char *names[RB_ACT_COUNT] = {
        "up", "down", "left", "right", "fire", "mode", "shift", "pause", "speed"
    };

    if (action < 0 || action >= RB_ACT_COUNT) {
        return NULL;
    }
    return names[action];
}

static void close_dev(void)
{
    if (g_pad) {
        SDL_GameControllerClose(g_pad);
        g_pad = NULL;
    }
    if (g_joy) {
        SDL_JoystickClose(g_joy);
        g_joy = NULL;
    }
    g_dev_is_pad = 0;
    g_dev_index = -1;
    g_joy_name[0] = 0;
}

static void remember_name(const char *s)
{
    size_t i;
    size_t n = 0;

    if (!s) {
        s = "JOYSTICK";
    }
    for (i = 0; s[i] && n + 1u < sizeof g_joy_name; i++) {
        unsigned char c = (unsigned char)s[i];

        if (c >= 'a' && c <= 'z') {
            c = (unsigned char)(c - 'a' + 'A');
        }
        if (c < 0x20u || c > 0x7Eu) {
            c = ' ';
        }
        g_joy_name[n++] = (char)c;
    }
    g_joy_name[n] = 0;
}

static int dev_alive(void)
{
    if (g_dev_index < 0 || g_dev_index >= SDL_NumJoysticks()) {
        return 0;
    }
    if (g_dev_is_pad) {
        return g_pad && SDL_GameControllerGetAttached(g_pad);
    }
    return g_joy && SDL_JoystickGetAttached(g_joy);
}

static void open_index(int i)
{
    close_dev();
    if (SDL_IsGameController(i)) {
        g_pad = SDL_GameControllerOpen(i);
        if (!g_pad) {
            return;
        }
        g_dev_is_pad = 1;
        g_dev_index = i;
        remember_name(SDL_GameControllerName(g_pad));
        return;
    }
    g_joy = SDL_JoystickOpen(i);
    if (!g_joy) {
        return;
    }
    g_dev_is_pad = 0;
    g_dev_index = i;
    remember_name(SDL_JoystickName(g_joy));
}

static void bind_device(void)
{
    int i;
    int n;

    SDL_JoystickEventState(SDL_ENABLE);
    SDL_GameControllerEventState(SDL_ENABLE);
    if (dev_alive()) {
        return;
    }
    close_dev();
    n = SDL_NumJoysticks();
    for (i = 0; i < n; i++) {
        open_index(i);
        if (dev_alive()) {
            return;
        }
    }
}

const char *sys_joy_name(void)
{
    return g_joy_name;
}

int sys_joy_present(void)
{
    return dev_alive();
}

static int pad_button(int id)
{
    if (g_dev_is_pad && g_pad) {
        if (id < 0 || id >= SDL_CONTROLLER_BUTTON_MAX) {
            return 0;
        }
        return SDL_GameControllerGetButton(g_pad, (SDL_GameControllerButton)id);
    }
    if (g_joy && id >= 0 && id < SDL_JoystickNumButtons(g_joy)) {
        return SDL_JoystickGetButton(g_joy, id);
    }
    return 0;
}

static int pad_axis_dir(int axis, int sign)
{
    Sint16 v = 0;

    if (g_dev_is_pad && g_pad) {
        if (axis < 0 || axis >= SDL_CONTROLLER_AXIS_MAX) {
            return 0;
        }
        v = SDL_GameControllerGetAxis(g_pad, (SDL_GameControllerAxis)axis);
    } else if (g_joy && axis >= 0 && axis < SDL_JoystickNumAxes(g_joy)) {
        v = SDL_JoystickGetAxis(g_joy, axis);
    }
    if (sign < 0) {
        return v < -16000;
    }
    return v > 16000;
}

static int pad_hat(int hat, int mask)
{
    if (g_dev_is_pad) {
        return 0;
    }
    if (!g_joy || hat < 0 || hat >= SDL_JoystickNumHats(g_joy)) {
        return 0;
    }
    return (SDL_JoystickGetHat(g_joy, hat) & mask) != 0;
}

static int bind_down(const Bind *b)
{
    if (!b || b->kind == RB_BIND_DEFAULT) {
        return 0;
    }
    if (b->pad != g_dev_is_pad || !dev_alive()) {
        if (b->kind != RB_BIND_KEY) {
            return 0;
        }
    }
    if (b->kind == RB_BIND_BUTTON) {
        return pad_button(b->a);
    }
    if (b->kind == RB_BIND_AXIS) {
        return pad_axis_dir(b->a, b->b);
    }
    if (b->kind == RB_BIND_HAT) {
        return pad_hat(b->a, b->b);
    }
    return 0;
}

static void default_pad(RbInput *out)
{
    Sint16 ax = 0;
    Sint16 ay = 0;
    if (!dev_alive()) {
        return;
    }
    out->pad = 1;
    if (g_dev_is_pad && g_pad) {
        ax = SDL_GameControllerGetAxis(g_pad, SDL_CONTROLLER_AXIS_LEFTX);
        ay = SDL_GameControllerGetAxis(g_pad, SDL_CONTROLLER_AXIS_LEFTY);
        if (SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_DPAD_UP) ||
            ay < -16000) {
            out->pad_buttons |= RB_BTN_UP;
        }
        if (SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_DPAD_DOWN) ||
            ay > 16000) {
            out->pad_buttons |= RB_BTN_DOWN;
        }
        if (SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_DPAD_LEFT) ||
            ax < -16000) {
            out->pad_buttons |= RB_BTN_LEFT;
        }
        if (SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) ||
            ax > 16000) {
            out->pad_buttons |= RB_BTN_RIGHT;
        }
        if (SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_A)) {
            out->pad_buttons |= RB_BTN_A;
        }
        if (SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_B)) {
            out->pad_buttons |= RB_BTN_B;
        }
        if (SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_X)) {
            out->pad_buttons |= RB_BTN_C;
        }
        if (SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_Y)) {
            out->pad_buttons |= RB_BTN_SPEED;
        }
        if (SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_START)) {
            out->pad_buttons |= RB_BTN_START;
        }
        if (SDL_GameControllerGetButton(g_pad, SDL_CONTROLLER_BUTTON_BACK)) {
            out->pad_buttons |= RB_BTN_QUIT;
        }
        return;
    }
    if (!g_joy) {
        return;
    }
    if (SDL_JoystickNumAxes(g_joy) >= 2) {
        ax = SDL_JoystickGetAxis(g_joy, 0);
        ay = SDL_JoystickGetAxis(g_joy, 1);
    }
    if (SDL_JoystickNumHats(g_joy) > 0) {
        Uint8 hat = SDL_JoystickGetHat(g_joy, 0);

        if (hat & SDL_HAT_UP) {
            out->pad_buttons |= RB_BTN_UP;
        }
        if (hat & SDL_HAT_DOWN) {
            out->pad_buttons |= RB_BTN_DOWN;
        }
        if (hat & SDL_HAT_LEFT) {
            out->pad_buttons |= RB_BTN_LEFT;
        }
        if (hat & SDL_HAT_RIGHT) {
            out->pad_buttons |= RB_BTN_RIGHT;
        }
    }
    if (ay < -16000) {
        out->pad_buttons |= RB_BTN_UP;
    }
    if (ay > 16000) {
        out->pad_buttons |= RB_BTN_DOWN;
    }
    if (ax < -16000) {
        out->pad_buttons |= RB_BTN_LEFT;
    }
    if (ax > 16000) {
        out->pad_buttons |= RB_BTN_RIGHT;
    }
    if (pad_button(0)) {
        out->pad_buttons |= RB_BTN_A;
    }
    if (pad_button(1)) {
        out->pad_buttons |= RB_BTN_B;
    }
    if (pad_button(2)) {
        out->pad_buttons |= RB_BTN_C;
    }
    if (pad_button(3)) {
        out->pad_buttons |= RB_BTN_SPEED;
    }
    if (pad_button(7) || pad_button(9)) {
        out->pad_buttons |= RB_BTN_START;
    }
}

static int key_default(const Uint8 *keys, int action)
{
    switch (action) {
    case RB_ACT_UP:
        return keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W] ||
               keys[SDL_SCANCODE_KP_8];
    case RB_ACT_DOWN:
        return keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S] ||
               keys[SDL_SCANCODE_KP_2];
    case RB_ACT_LEFT:
        return keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A] ||
               keys[SDL_SCANCODE_KP_4];
    case RB_ACT_RIGHT:
        return keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D] ||
               keys[SDL_SCANCODE_KP_6];
    case RB_ACT_FIRE:
        return keys[SDL_SCANCODE_Z] || keys[SDL_SCANCODE_SPACE] ||
               keys[SDL_SCANCODE_KP_0];
    case RB_ACT_MODE:
        return keys[SDL_SCANCODE_X] || keys[SDL_SCANCODE_N] ||
               keys[SDL_SCANCODE_M] || keys[SDL_SCANCODE_RETURN] ||
               keys[SDL_SCANCODE_KP_ENTER];
    case RB_ACT_SHIFT:
        return keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT];
    case RB_ACT_PAUSE:
        return keys[SDL_SCANCODE_ESCAPE];
    case RB_ACT_SPEED:
        return keys[SDL_SCANCODE_C] || keys[SDL_SCANCODE_LCTRL] ||
               keys[SDL_SCANCODE_RCTRL];
    default:
        return 0;
    }
}

static uint16_t act_bit(int action)
{
    switch (action) {
    case RB_ACT_UP:    return RB_BTN_UP;
    case RB_ACT_DOWN:  return RB_BTN_DOWN;
    case RB_ACT_LEFT:  return RB_BTN_LEFT;
    case RB_ACT_RIGHT: return RB_BTN_RIGHT;
    case RB_ACT_FIRE:  return RB_BTN_A;
    case RB_ACT_MODE:  return RB_BTN_B;
    case RB_ACT_SHIFT: return RB_BTN_C;
    case RB_ACT_PAUSE: return RB_BTN_START;
    case RB_ACT_SPEED: return RB_BTN_SPEED;
    default:           return 0;
    }
}

static void apply_custom(const Uint8 *keys, RbInput *out)
{
    int i;
    uint16_t custom_pad = 0;

    for (i = 0; i < RB_ACT_COUNT; i++) {
        uint16_t bit = act_bit(i);
        const Bind *b = &g_bind[i];

        if (b->kind == RB_BIND_KEY) {
            if (keys[(SDL_Scancode)b->a]) {
                out->buttons |= bit;
            }
            continue;
        }
        if (b->kind == RB_BIND_DEFAULT) {
            if (key_default(keys, i)) {
                out->buttons |= bit;
            }
            continue;
        }
        if (bind_down(b)) {
            out->pad_buttons |= bit;
            custom_pad |= bit;
        }
    }
    (void)custom_pad;
}

static void fill_label(int action)
{
    const Bind *b = &g_bind[action];
    const char *s = "DEFAULT";
    char tmp[24];

    if (b->kind == RB_BIND_KEY) {
        s = SDL_GetScancodeName((SDL_Scancode)b->a);
        if (!s || !s[0]) {
            snprintf(tmp, sizeof tmp, "KEY %d", b->a);
            s = tmp;
        }
    } else if (b->kind == RB_BIND_BUTTON) {
        if (b->pad) {
            s = SDL_GameControllerGetStringForButton((SDL_GameControllerButton)b->a);
            if (!s || !s[0]) {
                snprintf(tmp, sizeof tmp, "BUTTON %d", b->a);
                s = tmp;
            }
        } else {
            snprintf(tmp, sizeof tmp, "JOY %d", b->a);
            s = tmp;
        }
    } else if (b->kind == RB_BIND_AXIS) {
        snprintf(tmp, sizeof tmp, "AXIS %d%c", b->a, b->b < 0 ? '-' : '+');
        s = tmp;
    } else if (b->kind == RB_BIND_HAT) {
        snprintf(tmp, sizeof tmp, "HAT %d", b->b);
        s = tmp;
    }
    snprintf(g_label[action], sizeof g_label[action], "%s", s);
}

const char *sys_bind_label(int action)
{
    if (action < 0 || action >= RB_ACT_COUNT) {
        return "";
    }
    if (g_bind[action].kind == RB_BIND_DEFAULT) {
        switch (action) {
        case RB_ACT_UP:    return "UP";
        case RB_ACT_DOWN:  return "DOWN";
        case RB_ACT_LEFT:  return "LEFT";
        case RB_ACT_RIGHT: return "RIGHT";
        case RB_ACT_FIRE:  return "Z";
        case RB_ACT_MODE:  return "X";
        case RB_ACT_SHIFT: return "SHIFT";
        case RB_ACT_PAUSE: return "ESC";
        case RB_ACT_SPEED: return "C";
        default:           break;
        }
    }
    return g_label[action];
}

int sys_bind_set(int action, int kind, int a, int b)
{
    if (action < 0 || action >= RB_ACT_COUNT) {
        return -1;
    }
    if (kind == RB_BIND_DEFAULT) {
        memset(&g_bind[action], 0, sizeof g_bind[action]);
        fill_label(action);
        return 0;
    }
    g_bind[action].kind = kind;
    g_bind[action].a = a;
    g_bind[action].b = b;
    g_bind[action].pad = g_cap_pad;
    fill_label(action);
    return 0;
}

void sys_bind_restore(void)
{
    memset(g_bind, 0, sizeof g_bind);
}

void sys_bind_begin(void)
{
    g_cap = 1;
    g_cap_result = 0;
}

int sys_bind_result(int *kind, int *a, int *b)
{
    int r = g_cap_result;

    if (r == 0) {
        return 0;
    }
    if (r > 0 && kind && a && b) {
        *kind = g_cap_kind;
        *a = g_cap_a;
        *b = g_cap_b;
    }
    g_cap_result = 0;
    g_cap = 0;
    return r;
}

void sys_bind_write(void *file)
{
    FILE *fp = (FILE *)file;
    int i;

    if (!fp) {
        return;
    }
    for (i = 0; i < RB_ACT_COUNT; i++) {
        const Bind *b = &g_bind[i];
        const char *name = act_name(i);

        if (!name || b->kind == RB_BIND_DEFAULT) {
            continue;
        }
        if (b->kind == RB_BIND_KEY) {
            fprintf(fp, "bind.%s=key:%d\n", name, b->a);
        } else if (b->kind == RB_BIND_BUTTON) {
            fprintf(fp, "bind.%s=%sbtn:%d\n", name, b->pad ? "c" : "j", b->a);
        } else if (b->kind == RB_BIND_AXIS) {
            fprintf(fp, "bind.%s=%saxis:%d:%d\n", name, b->pad ? "c" : "j",
                    b->a, b->b);
        } else if (b->kind == RB_BIND_HAT) {
            fprintf(fp, "bind.%s=hat:%d:%d\n", name, b->a, b->b);
        }
    }
}

int sys_bind_read_line(const char *line)
{
    char name[16];
    char body[32];
    int action;
    int a = 0;
    int b = 0;
    int i;

    if (!line || strncmp(line, "bind.", 5) != 0) {
        return 0;
    }
    if (sscanf(line, "bind.%15[^=]=%31s", name, body) != 2) {
        return 1;
    }
    action = -1;
    for (i = 0; i < RB_ACT_COUNT; i++) {
        if (strcmp(name, act_name(i)) == 0) {
            action = i;
            break;
        }
    }
    if (action < 0) {
        return 1;
    }
    if (strcmp(body, "default") == 0) {
        sys_bind_set(action, RB_BIND_DEFAULT, 0, 0);
        return 1;
    }
    if (sscanf(body, "key:%d", &a) == 1) {
        g_cap_pad = 0;
        sys_bind_set(action, RB_BIND_KEY, a, 0);
        return 1;
    }
    if (sscanf(body, "cbtn:%d", &a) == 1) {
        g_cap_pad = 1;
        sys_bind_set(action, RB_BIND_BUTTON, a, 0);
        return 1;
    }
    if (sscanf(body, "jbtn:%d", &a) == 1) {
        g_cap_pad = 0;
        sys_bind_set(action, RB_BIND_BUTTON, a, 0);
        return 1;
    }
    if (sscanf(body, "caxis:%d:%d", &a, &b) == 2) {
        g_cap_pad = 1;
        sys_bind_set(action, RB_BIND_AXIS, a, b < 0 ? -1 : 1);
        return 1;
    }
    if (sscanf(body, "jaxis:%d:%d", &a, &b) == 2) {
        g_cap_pad = 0;
        sys_bind_set(action, RB_BIND_AXIS, a, b < 0 ? -1 : 1);
        return 1;
    }
    if (sscanf(body, "hat:%d:%d", &a, &b) == 2) {
        g_cap_pad = 0;
        sys_bind_set(action, RB_BIND_HAT, a, b);
        return 1;
    }
    return 1;
}

static void take_key(SDL_Scancode code)
{
    if (code == SDL_SCANCODE_ESCAPE) {
        g_cap_result = -1;
        return;
    }
    g_cap_kind = RB_BIND_KEY;
    g_cap_a = (int)code;
    g_cap_b = 0;
    g_cap_pad = 0;
    g_cap_result = 1;
}

static void take_button(int id, int pad)
{
    g_cap_kind = RB_BIND_BUTTON;
    g_cap_a = id;
    g_cap_b = 0;
    g_cap_pad = pad;
    g_cap_result = 1;
}

static void take_axis(int axis, int value, int pad)
{
    if (value > -20000 && value < 20000) {
        return;
    }
    g_cap_kind = RB_BIND_AXIS;
    g_cap_a = axis;
    g_cap_b = value < 0 ? -1 : 1;
    g_cap_pad = pad;
    g_cap_result = 1;
}

static void take_hat(int hat, int value)
{
    if (value == SDL_HAT_CENTERED) {
        return;
    }
    g_cap_kind = RB_BIND_HAT;
    g_cap_a = hat;
    g_cap_b = value;
    g_cap_pad = 0;
    g_cap_result = 1;
}

static uint16_t hold_token(const char *s, size_t n)
{
    char buf[16];
    size_t i;

    if (n == 0 || n >= sizeof(buf)) {
        return 0;
    }
    for (i = 0; i < n; i++) {
        char c = s[i];

        if (c >= 'a' && c <= 'z') {
            c = (char)(c - 'a' + 'A');
        }
        buf[i] = c;
    }
    buf[n] = 0;
    if (strcmp(buf, "A") == 0 || strcmp(buf, "FIRE") == 0 ||
        strcmp(buf, "Z") == 0 || strcmp(buf, "SPACE") == 0) {
        return RB_BTN_A;
    }
    if (strcmp(buf, "B") == 0 || strcmp(buf, "MODE") == 0 ||
        strcmp(buf, "X") == 0) {
        return RB_BTN_B;
    }
    if (strcmp(buf, "C") == 0 || strcmp(buf, "SHIFT") == 0) {
        return RB_BTN_C;
    }
    if (strcmp(buf, "START") == 0 || strcmp(buf, "ESC") == 0) {
        return RB_BTN_START;
    }
    if (strcmp(buf, "QUIT") == 0 || strcmp(buf, "F10") == 0) {
        return RB_BTN_QUIT;
    }
    if (strcmp(buf, "UP") == 0 || strcmp(buf, "W") == 0) {
        return RB_BTN_UP;
    }
    if (strcmp(buf, "DOWN") == 0 || strcmp(buf, "S") == 0) {
        return RB_BTN_DOWN;
    }
    if (strcmp(buf, "LEFT") == 0 || strcmp(buf, "AKEY") == 0) {
        return RB_BTN_LEFT;
    }
    if (strcmp(buf, "RIGHT") == 0 || strcmp(buf, "D") == 0) {
        return RB_BTN_RIGHT;
    }
    if (strcmp(buf, "1") == 0) {
        return RB_BTN_1;
    }
    if (strcmp(buf, "2") == 0) {
        return RB_BTN_2;
    }
    if (strcmp(buf, "3") == 0) {
        return RB_BTN_3;
    }
    if (strcmp(buf, "4") == 0) {
        return RB_BTN_4;
    }
    return 0;
}

/* RUDEBR_HOLD is a smoke-test injector (for example A,RIGHT). Dummy
 * video has no keyboard, and this is not a player control. */
static uint16_t held_from_env(void)
{
    static int ready;
    static uint16_t bits;
    const char *s;

    if (ready) {
        return bits;
    }
    ready = 1;
    s = getenv("RUDEBR_HOLD");
    while (s && *s) {
        const char *start;
        size_t n;

        while (*s == ' ' || *s == '\t' || *s == ',') {
            s++;
        }
        if (!*s) {
            break;
        }
        start = s;
        while (*s && *s != ',' && *s != ' ' && *s != '\t') {
            s++;
        }
        n = (size_t)(s - start);
        bits = (uint16_t)(bits | hold_token(start, n));
    }
    return bits;
}


/* RUDEBR_INPUT names a smoke-test script: lines of "FROM TO TOKENS"
 * (poll counts, tokens as in RUDEBR_HOLD). Unset means no effect. */
#define SCRIPT_MAX 512
static uint16_t script_from_env(void)
{
    static int ready, n;
    static unsigned long from[SCRIPT_MAX], to[SCRIPT_MAX], tick;
    static uint16_t bits[SCRIPT_MAX];
    uint16_t out = 0;
    int i;

    if (!ready) {
        const char *path = getenv("RUDEBR_INPUT");
        FILE *fp = path && path[0] ? fopen(path, "r") : NULL;
        char line[256];

        ready = 1;
        while (fp && n < SCRIPT_MAX && fgets(line, sizeof(line), fp)) {
            char *p = line, *e;
            unsigned long a = strtoul(p, &e, 10), b;

            if (e == p) {
                continue;
            }
            p = e;
            b = strtoul(p, &e, 10);
            if (e == p) {
                continue;
            }
            p = e;
            bits[n] = 0;
            while (*p) {
                char *st;

                while (*p == ' ' || *p == '\t' || *p == ',') {
                    p++;
                }
                st = p;
                while (*p && *p != ' ' && *p != '\t' && *p != ',' &&
                       *p != '\n' && *p != '\r') {
                    p++;
                }
                if (p == st) {
                    break;
                }
                bits[n] = (uint16_t)(bits[n] | hold_token(st, (size_t)(p - st)));
            }
            from[n] = a;
            to[n] = b;
            n++;
        }
        if (fp) {
            fclose(fp);
        }
    }
    for (i = 0; i < n; i++) {
        if (tick >= from[i] && tick < to[i]) {
            out = (uint16_t)(out | bits[i]);
        }
    }
    tick++;
    return out;
}

static void note_capture(const SDL_Event *ev)
{
    if (!g_cap || g_cap_result != 0) {
        return;
    }
    if (ev->type == SDL_KEYDOWN && !ev->key.repeat) {
        take_key(ev->key.keysym.scancode);
        return;
    }
    if (ev->type == SDL_CONTROLLERBUTTONDOWN && g_dev_is_pad &&
        ev->cbutton.button == SDL_CONTROLLER_BUTTON_BACK) {
        g_cap_result = -1; /* pad BACK cancels, like Esc */
        return;
    }
    if (ev->type == SDL_CONTROLLERBUTTONDOWN && g_dev_is_pad) {
        take_button((int)ev->cbutton.button, 1);
        return;
    }
    if (ev->type == SDL_CONTROLLERAXISMOTION && g_dev_is_pad) {
        take_axis((int)ev->caxis.axis, (int)ev->caxis.value, 1);
        return;
    }
    if (ev->type == SDL_JOYBUTTONDOWN && !g_dev_is_pad) {
        take_button((int)ev->jbutton.button, 0);
        return;
    }
    if (ev->type == SDL_JOYAXISMOTION && !g_dev_is_pad) {
        take_axis((int)ev->jaxis.axis, (int)ev->jaxis.value, 0);
        return;
    }
    if (ev->type == SDL_JOYHATMOTION && !g_dev_is_pad) {
        take_hat((int)ev->jhat.hat, (int)ev->jhat.value);
    }
}

static int g_any_press;
static int g_f11;

int sys_take_fullscreen_key(void)
{
    int r = g_f11;

    g_f11 = 0;
    return r;
}

int sys_any_press(void)
{
    int r = g_any_press;

    g_any_press = 0;
    return r;
}

void sys_read_input(RbInput *out)
{
    SDL_Event ev;
    const Uint8 *keys;
    int i;
    uint16_t pad_default;

    memset(out, 0, sizeof(*out));
    bind_device();

    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            out->buttons |= RB_BTN_QUIT;
        }
        if (ev.type == SDL_JOYDEVICEADDED || ev.type == SDL_JOYDEVICEREMOVED ||
            ev.type == SDL_CONTROLLERDEVICEADDED ||
            ev.type == SDL_CONTROLLERDEVICEREMOVED) {
            close_dev();
            bind_device();
        }
        if ((ev.type == SDL_KEYDOWN && !ev.key.repeat) ||
            ev.type == SDL_CONTROLLERBUTTONDOWN ||
            ev.type == SDL_JOYBUTTONDOWN) {
            g_any_press = 1;
        }
        if (ev.type == SDL_KEYDOWN && !ev.key.repeat && !g_cap &&
            ev.key.keysym.scancode == SDL_SCANCODE_F11) {
            g_f11 = 1;
        }
        note_capture(&ev);
    }

    keys = SDL_GetKeyboardState(NULL);
    if (!g_cap) {
        apply_custom(keys, out);
        pad_default = 0;
        {
            RbInput pad;

            memset(&pad, 0, sizeof pad);
            default_pad(&pad);
            out->pad = pad.pad;
            pad_default = pad.pad_buttons;
        }
        for (i = 0; i < RB_ACT_COUNT; i++) {
            if (g_bind[i].kind != RB_BIND_DEFAULT &&
                g_bind[i].kind != RB_BIND_KEY) {
                pad_default = (uint16_t)(pad_default & (uint16_t)~act_bit(i));
            }
        }
        out->pad_buttons = (uint16_t)(out->pad_buttons | pad_default);
    } else if (dev_alive()) {
        out->pad = 1;
    }

    /* Cheat chord and the 1..4 debug keys stay on the physical keys. */
    if (keys[SDL_SCANCODE_N]) {
        out->chord = (uint8_t)(out->chord | 0x08u);
    }
    if (keys[SDL_SCANCODE_E]) {
        out->chord = (uint8_t)(out->chord | 0x02u);
    }
    if (keys[SDL_SCANCODE_K]) {
        out->chord = (uint8_t)(out->chord | 0x04u);
    }
    if (keys[SDL_SCANCODE_O]) {
        out->chord = (uint8_t)(out->chord | 0x10u);
    }
    if (keys[SDL_SCANCODE_C]) {
        out->chord = (uint8_t)(out->chord | 0x01u);
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
    if (keys[SDL_SCANCODE_F10]) {
        out->buttons |= RB_BTN_QUIT;
    }
    out->buttons = (uint16_t)(out->buttons | held_from_env() | script_from_env());
    if (out->buttons & RB_BTN_UP) {
        out->axis_y = -1;
    }
    if (out->buttons & RB_BTN_DOWN) {
        out->axis_y = 1;
    }
    if (out->buttons & RB_BTN_LEFT) {
        out->axis_x = -1;
    }
    if (out->buttons & RB_BTN_RIGHT) {
        out->axis_x = 1;
    }
    (void)g_rb_window;
}
