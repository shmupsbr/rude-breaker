#include "pause_box.h"

#include <stdio.h>
#include <string.h>

static int fails;

static void expect(const char *n, int c)
{
    if (!c) {
        fprintf(stderr, "FAIL %s\n", n);
        fails++;
    } else {
        printf("ok %s\n", n);
    }
}

static int center(int col, int chars)
{
    return col * 8 + chars * 8;
}

int main(void)
{
    RbPauseBox b;
    int i;
    const char *item[2] = {"RESUME", "MAIN MENU"};

    rb_pause_box(&b);
    printf("box %d,%d %dx%d title col %d row %d\n", b.box_x, b.box_y, b.box_w, b.box_h,
           b.title_col, b.title_row);
    expect("inside playfield", b.box_x >= 80 && b.box_x + b.box_w <= 560 && b.box_y >= 0 &&
                                   b.box_y + b.box_h <= 400);
    expect("centred horizontally", b.box_x + b.box_w / 2 == 320 ||
                                       b.box_x + (b.box_w - 1) / 2 == 319);
    expect("title centred", center(b.title_col, 5) == 320);
    for (i = 0; i < 2; i++) {
        int c = center(b.item_col[i], (int)strlen(item[i]));

        expect(item[i], c == 320);
        expect("row inside box", b.item_row[i] * 16 >= b.box_y &&
                                     b.item_row[i] * 16 + 16 <= b.box_y + b.box_h);
    }
    expect("title inside", b.title_row * 16 >= b.box_y &&
                               b.title_row * 16 + 16 <= b.box_y + b.box_h);
    expect("even vertical pad", (b.title_row * 16 - b.box_y) ==
                                    (b.box_y + b.box_h) - (b.item_row[1] * 16 + 16));
    expect("bar same width", b.bar_w > 9 * 16 && b.bar_x >= b.box_x &&
                                 b.bar_x + b.bar_w <= b.box_x + b.box_w);
    expect("rows spaced evenly", b.item_row[0] - b.title_row == 2 &&
                                     b.item_row[1] - b.item_row[0] == 2);
    if (fails) {
        fprintf(stderr, "%d failed\n", fails);
        return 1;
    }
    printf("pause_box: ok\n");
    return 0;
}
