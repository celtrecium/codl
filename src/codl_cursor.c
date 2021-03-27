#include "codl_internal.h"


int codl_set_cursor_position(codl_window *win, int x_pos, int y_pos) {
    CODL_NULLPTR_MACRO(!win, "Window pointer for set cursor position is NULL")
    CODL_NULLPTR_MACRO(!win->window_buffer, "Window buffer for set cursor position is NULL")

    win->cursor_pos_y = y_pos;

    if(x_pos < win->width) {
        win->cursor_pos_x = x_pos;
    } else {
        win->cursor_pos_x = x_pos % win->width;
        ++win->cursor_pos_y;
    }

    if(y_pos > win->height) {
        codl_buffer_scroll_down(win, y_pos - win->height);
        win->cursor_pos_y = win->height;
    }

    return(1);
}


int codl_save_cursor_position(codl_window *win) {
    CODL_NULLPTR_MACRO(!win, "Window pointer for save cursor position is NULL")
    CODL_NULLPTR_MACRO(!win->window_buffer, "Window buffer for save cursor position is NULL")

    win->s_cur_pos_x = win->cursor_pos_x;
    win->s_cur_pos_y = win->cursor_pos_y;

    return(1);
}


int codl_restore_cursor_position(codl_window *win) {
    CODL_NULLPTR_MACRO(!win, "Window pointer for restore cursor position is NULL")
    CODL_NULLPTR_MACRO(!win->window_buffer, "Window buffer for restore cursor position is NULL")

    win->cursor_pos_x = win->s_cur_pos_x;
    win->cursor_pos_y = win->s_cur_pos_y;

    return(1);
}


int codl_replace_attributes(codl_window *win, int x0_pos, int y0_pos, int x1_pos, int y1_pos) {
    int width = x1_pos - x0_pos + 1;
    int height = y1_pos - y0_pos + 1;

    int count_1 = 0;
    int count_2 = 0;
    char *ptr;

    int tmp_cur_x;
    int tmp_cur_y;

    CODL_NULLPTR_MACRO(!win, "Window pointer for draw rectangle is NULL")
    CODL_NULLPTR_MACRO(!win->window_buffer, "Window buffer for draw rectangle is NULL")

    tmp_cur_x = win->cursor_pos_x;
    tmp_cur_y = win->cursor_pos_y;
        
    for(count_1 = 0; count_1 < height; ++count_1){
        for(count_2 = 0; count_2 < width; ++count_2){
            if(((count_1 + y0_pos) < win->height) && ((count_1 + y0_pos) >= 0) &&
               ((count_2 + x0_pos) < win->width)  && ((count_2 + x0_pos) >= 0)) {
                ptr = win->window_buffer[count_2 + x0_pos][count_1 + y0_pos];
                ptr[4] = (char)win->colour_bg;
                ptr[5] = (char)win->colour_fg;
                ptr[6] = win->text_attribute;
            }
        }
    }

    __codl_set_region_diff(win->x_position + x0_pos, win->y_position + y0_pos,
                           win->x_position + x1_pos, win->y_position + y1_pos);
    
    win->cursor_pos_x = tmp_cur_x;
    win->cursor_pos_y = tmp_cur_y;

    return(1);
}


void codl_monochrome_mode(CODL_SWITCH mode) {
    switch(mode) {
        case CODL_ENABLE:
            mono_mode = 1;
            break;
        case CODL_DISABLE:
            mono_mode = 0;
            break;
    }
}
