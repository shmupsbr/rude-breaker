/* Writes two PNGs and checks the screenshot name grows _2 in one second. */
#include "pngwrite.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int fail(const char *m)
{
    fprintf(stderr, "png_shot_test: %s\n", m);
    return 1;
}

int main(void)
{
    uint32_t small[6];
    uint32_t *big;
    FILE *fp;
    int y;
    int x;
    const char *dir = "/tmp/rb_shot_test";
    char cmd[256];

    small[0] = 0x112233FFu;
    small[1] = 0x445566FFu;
    small[2] = 0x778899FFu;
    small[3] = 0xAABBCCFFu;
    small[4] = 0x010203FFu;
    small[5] = 0xFEFDFCFFu;
    fp = fopen("/tmp/rb_png_small.png", "wb");
    if (!fp || rb_write_png(fp, small, 3, 2) != 0) {
        return fail("small write");
    }
    fclose(fp);

    big = (uint32_t *)malloc(400u * 200u * 4u);
    if (!big) {
        return fail("alloc");
    }
    for (y = 0; y < 200; y++) {
        for (x = 0; x < 400; x++) {
            big[y * 400 + x] = ((uint32_t)(x & 255) << 24) | ((uint32_t)(y & 255) << 16) |
                               ((uint32_t)((x + y) & 255) << 8) | 0xFFu;
        }
    }
    fp = fopen("/tmp/rb_png_big.png", "wb");
    if (!fp || rb_write_png(fp, big, 400, 200) != 0) {
        free(big);
        return fail("big write");
    }
    fclose(fp);
    free(big);

    snprintf(cmd, sizeof cmd, "rm -rf %s && mkdir -p %s", dir, dir);
    if (system(cmd) != 0) {
        return fail("mkdir");
    }
    if (rb_save_screenshot(small, 3, 2, dir) != 0) {
        return fail("save 1");
    }
    if (rb_save_screenshot(small, 3, 2, dir) != 0) {
        return fail("save 2");
    }
    snprintf(cmd, sizeof cmd,
             "python3 tools/png_shot_check.py /tmp/rb_png_small.png /tmp/rb_png_big.png %s",
             dir);
    if (system(cmd) != 0) {
        return fail("check");
    }
    printf("png_shot_test: ok\n");
    return 0;
}
