/* The embedded mapping list parses, and the families we care about
 * are actually in it. SDL accepts every line. */
#include <SDL.h>

#include <stdio.h>
#include <string.h>

#include "../src/hal/sdl2/gc_db.inc"

static int has(const char *line, const char *key)
{
    const char *name = strchr(line, ',');
    const char *end;
    char buf[128];
    size_t n;
    size_t i;

    if (!name) {
        return 0;
    }
    name++;
    end = strchr(name, ',');
    /* Name can hold commas; the mapping starts at the first key:value. */
    end = name;
    while (*end && *end != ':') {
        end++;
    }
    while (end > name && *end != ',') {
        end--;
    }
    n = (size_t)(end - name);
    if (n >= sizeof buf) {
        n = sizeof buf - 1;
    }
    for (i = 0; i < n; i++) {
        char c = name[i];

        if (c >= 'A' && c <= 'Z') {
            c = (char)(c - 'A' + 'a');
        }
        buf[i] = c;
    }
    buf[n] = 0;
    return strstr(buf, key) != NULL;
}

static int count_key(const char *key)
{
    int i;
    int n = 0;

    for (i = 0; i < rb_gc_map_count; i++) {
        if (has(rb_gc_map[i], key)) {
            n++;
        }
    }
    return n;
}

static int expect(const char *key, int min)
{
    int n = count_key(key);

    if (n < min) {
        fprintf(stderr, "gc_db: %s has %d, want >= %d\n", key, n, min);
        return 1;
    }
    printf("gc_db: %s %d\n", key, n);
    return 0;
}

int main(void)
{
    int i;
    int bad = 0;
    int added = 0;

    if (rb_gc_map_count < 100) {
        fprintf(stderr, "gc_db: too small (%d)\n", rb_gc_map_count);
        return 1;
    }
    bad |= expect("ps5", 1);
    bad |= expect("ps4", 1);
    bad |= expect("dualshock 4", 1);
    bad |= expect("xbox", 1);
    bad |= expect("8bitdo", 1);
    bad |= expect("wii", 1);
    bad |= expect("mayflash", 1);
    bad |= expect("arcade", 1);
    {
        int win = 0, mac = 0, lin = 0;

        for (i = 0; i < rb_gc_map_count; i++) {
            if (strstr(rb_gc_map[i], "platform:Windows,")) {
                win++;
            }
            if (strstr(rb_gc_map[i], "platform:Mac OS X,")) {
                mac++;
            }
            if (strstr(rb_gc_map[i], "platform:Linux,")) {
                lin++;
            }
        }
        printf("gc_db: windows %d mac %d linux %d\n", win, mac, lin);
        if (win < 1 || mac < 1 || lin < 1) {
            fprintf(stderr, "gc_db: missing a desktop platform\n");
            bad = 1;
        }
    }
    for (i = 0; i < rb_gc_map_count; i++) {
        const char *s = rb_gc_map[i];
        int k;

        if ((int)strlen(s) < 40 || strstr(s, ",platform:") == NULL) {
            fprintf(stderr, "gc_db: bad line %d\n", i);
            return 1;
        }
        for (k = 0; k < 32; k++) {
            char c = s[k];

            if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))) {
                fprintf(stderr, "gc_db: guid %d\n", i);
                return 1;
            }
        }
    }
    if (SDL_Init(SDL_INIT_GAMECONTROLLER) != 0) {
        fprintf(stderr, "gc_db: SDL_Init %s\n", SDL_GetError());
        return 1;
    }
    for (i = 0; i < rb_gc_map_count; i++) {
        if (SDL_GameControllerAddMapping(rb_gc_map[i]) < 0) {
            fprintf(stderr, "gc_db: SDL rejected: %s (%s)\n", rb_gc_map[i], SDL_GetError());
            bad = 1;
        } else {
            added++;
        }
    }
    SDL_Quit();
    if (bad) {
        return 1;
    }
    printf("gc_db: ok %d mappings, SDL accepted %d\n", rb_gc_map_count, added);
    return 0;
}
