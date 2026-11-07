/**
 * Pause dialog geometry, in native 640x400 pixels.
 * The playfield is columns 80..560. Every line is centred in the box,
 * the box is centred in the playfield, and the selection bar is the
 * same width for every row.
 */
#ifndef RUDEBREAKER_PAUSE_BOX_H
#define RUDEBREAKER_PAUSE_BOX_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RbPauseBox {
    int box_x, box_y, box_w, box_h;
    int title_col, title_row;
    int item_col[2], item_row[2];
    int bar_x, bar_w, bar_h;
    int n_items;
} RbPauseBox;

void rb_pause_box(RbPauseBox *o);

#ifdef __cplusplus
}
#endif

#endif
