#include "codl_internal.h"


int codl_line(codl_window *win, int x1, int y1, int x2, int y2, char *symbol) {
    const int deltaX = abs(x2 - x1);
    const int deltaY = abs(y2 - y1);

    const int signX = x1 < x2 ? 1 : -1;
    const int signY = y1 < y2 ? 1 : -1;

    int error = deltaX - deltaY;
    int error2;

    int tmp_cur_x;
    int tmp_cur_y;

    CODL_NULLPTR_MACRO(!win, "Window pointer for draw line is NULL")
    CODL_NULLPTR_MACRO(!win->window_buffer, "Window buffer for draw line is NULL")

    tmp_cur_x = win->cursor_pos_x;
    tmp_cur_y = win->cursor_pos_y;

    while(x1 != x2 || y1 != y2) {
        codl_set_cursor_position(win, x1, y1);
        codl_write(win, symbol);

        error2 = error * 2;

        if(error2 > -deltaY) {
            error -= deltaY;
            x1 += signX;
        }

        if(error2 < deltaX) {
            error += deltaX;
            y1 += signY;
        }
    }

    win->cursor_pos_x = tmp_cur_x;
    win->cursor_pos_y = tmp_cur_y;

    return(1);
}


int codl_rectangle(codl_window *win, int x0_pos, int y0_pos, int x1_pos, int y1_pos, char *symbol) {
    int width = x1_pos - x0_pos;
    int height = y1_pos - y0_pos;

    int count_1 = 0;
    int count_2 = 0;

    int tmp_cur_x;
    int tmp_cur_y;

    CODL_NULLPTR_MACRO(!win, "Window pointer for draw rectangle is NULL")
    CODL_NULLPTR_MACRO(!win->window_buffer, "Window buffer for draw rectangle is NULL")

    tmp_cur_x = win->cursor_pos_x;
    tmp_cur_y = win->cursor_pos_y;

    for(count_1 = 0; count_1 < height; ++count_1){
        for(count_2 = 0; count_2 < width; ++count_2){
            codl_set_cursor_position(win, count_2 + x0_pos, count_1 + y0_pos);
            codl_write(win, symbol);
        }
    }

    win->cursor_pos_x = tmp_cur_x;
    win->cursor_pos_y = tmp_cur_y;
    
    return(1);
}


int codl_frame(codl_window *win, int x0_pos, int y0_pos, int x1_pos, int y1_pos) {
    int temp_x;
    int temp_y;
    int width = x1_pos - x0_pos;
    int height = y1_pos - y0_pos;

    int tmp_cur_x;
    int tmp_cur_y;
    
    CODL_NULLPTR_MACRO(!win, "Window pointer for draw frame is NULL")
    CODL_NULLPTR_MACRO(!win->window_buffer, "Window buffer for draw frame is NULL")

    tmp_cur_x = win->cursor_pos_x;
    tmp_cur_y = win->cursor_pos_y;

    for(temp_y = 0; temp_y < height; ++temp_y) {
        for(temp_x = 0; temp_x < width; ++temp_x) {
            if(!temp_y) {
                codl_set_cursor_position(win, temp_x + x0_pos, temp_y + y0_pos);
                codl_set_colour(win, (int)win->colour_bg, frame_colours[2]);
                codl_write(win, frame_symbols[2]);

            }

            if(temp_y == height - 1) {
                codl_set_cursor_position(win, temp_x + x0_pos, temp_y + y0_pos);
                codl_set_colour(win, (int)win->colour_bg, frame_colours[3]);
                codl_write(win, frame_symbols[3]);
            }
        }

        codl_set_cursor_position(win, x0_pos, temp_y + y0_pos);
        codl_set_colour(win, (int)win->colour_bg, frame_colours[0]);
        codl_write(win, frame_symbols[0]);

        codl_set_cursor_position(win, x1_pos - 1, temp_y + y0_pos);
        codl_set_colour(win, (int)win->colour_bg, frame_colours[1]);
        codl_write(win, frame_symbols[1]);
    }

    codl_set_cursor_position(win, x0_pos, y0_pos);
    codl_set_colour(win, (int)win->colour_bg, frame_colours[4]);
    codl_write(win, frame_symbols[4]);

    codl_set_cursor_position(win, x1_pos - 1, y0_pos);
    codl_set_colour(win, (int)win->colour_bg, frame_colours[5]);
    codl_write(win, frame_symbols[5]);

    codl_set_cursor_position(win, x0_pos, y1_pos - 1);
    codl_set_colour(win, (int)win->colour_bg, frame_colours[6]);
    codl_write(win, frame_symbols[6]);

    codl_set_cursor_position(win, x1_pos - 1, y1_pos - 1);
    codl_set_colour(win, (int)win->colour_bg, frame_colours[7]);
    codl_write(win, frame_symbols[7]);

    win->cursor_pos_x = tmp_cur_x;
    win->cursor_pos_y = tmp_cur_y;

    return(1);
}

