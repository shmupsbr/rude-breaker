#include "pause_box.h"

#include <string.h>

/* Glyphs are 16x16, one text column is 8 px, rows are 16 px apart. */
void rb_pause_box(RbPauseBox *o)
{
    static const char *const line[3] = {"PAUSE", "RESUME", "MAIN MENU"};
    int len[3];
    int i;
    int widest = 0;
    int text_w;
    int text_h;
    int pad_x = 24;
    int pad_y = 16;
    int cx = 320;
    int top;

    memset(o, 0, sizeof *o);
    o->n_items = 2;
    for (i = 0; i < 3; i++) {
        len[i] = (int)strlen(line[i]);
        if (len[i] > widest) {
            widest = len[i];
        }
    }
    text_w = widest * 16;
    /* Title plus two items, one blank text row between each. */
    text_h = 16 + 16 + 16 + 16 + 16;
    o->box_w = text_w + pad_x * 2;
    o->box_h = text_h + pad_y * 2;
    o->box_x = cx - o->box_w / 2;
    /* Block of text centred on the playfield's vertical middle. */
    top = 200 - text_h / 2;
    o->box_y = top - pad_y;
    o->title_row = top / 16;
    o->title_col = (cx - len[0] * 8) / 8;
    o->item_row[0] = o->title_row + 2;
    o->item_row[1] = o->title_row + 4;
    o->item_col[0] = (cx - len[1] * 8) / 8;
    o->item_col[1] = (cx - len[2] * 8) / 8;
    o->bar_x = o->box_x + 8;
    o->bar_w = o->box_w - 16;
    o->bar_h = 16;
}
