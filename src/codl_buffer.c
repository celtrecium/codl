#include "codl_internal.h"

int codl_buffer_scroll_down(codl_window *win, int down) {
    int count;
    int temp_x;
    int temp_y;

    CODL_NULLPTR_MACRO(!win, "Window pointer for scroll down is NULL")
    CODL_NULLPTR_MACRO(!win->window_buffer, "Window buffer pointer for scroll down is NULL")

    if(down > win->height) {
        for(temp_x = 0; temp_x < win->width; ++temp_x) {
            for(temp_y = 0; temp_y < win->height; ++temp_y) {
                for(count = 0; count < CELL_SIZE; ++count) {
                    win->window_buffer[temp_x][temp_y][count] = 0;
                }
            }
        }

        return(0);
    }

    for(temp_x = 0; temp_x < win->width; ++temp_x) {
        for(temp_y = 0; temp_y < (win->height - down); ++temp_y) {
            for(count = 0; count < CELL_SIZE; ++count) {
                win->window_buffer[temp_x][temp_y][count] = win->window_buffer[temp_x][temp_y + down][count];
            }
        }
    }

    for(temp_x = 0; temp_x < win->width; ++temp_x) {
        for(temp_y = win->height - down; temp_y < win->height; ++temp_y) {
            for(count = 0; count < CELL_SIZE; ++count) {
                win->window_buffer[temp_x][temp_y][count] = 0;
            }
        }
    }

    return(1);
}


int codl_buffer_scroll_up(codl_window *win, int up) {
    int count;
    int temp_x;
    int temp_y;

    CODL_NULLPTR_MACRO(!win, "Window pointer for scroll down is NULL")
    CODL_NULLPTR_MACRO(!win->window_buffer, "Window buffer pointer for scroll down is NULL")

    if(up > win->height) {
        for(temp_x = 0; temp_x < win->width; ++temp_x) {
            for(temp_y = 0; temp_y < win->height; ++temp_y) {
                for(count = 0; count < CELL_SIZE; ++count) {
                    win->window_buffer[temp_x][temp_y][count] = 0;
                }
            }
        }

        return(0);
    }

    for(temp_x = 0; temp_x < win->width; ++temp_x) {
        for(temp_y = (win->height - up - 1); temp_y >= 0; --temp_y) {
            for(count = 0; count < CELL_SIZE; ++count) {
                win->window_buffer[temp_x][temp_y + up][count] = win->window_buffer[temp_x][temp_y][count];
            }
        }
    }

    for(temp_x = 0; temp_x < win->width; ++temp_x) {
        for(temp_y = 0; temp_y < up; ++temp_y) {
            for(count = 0; count < CELL_SIZE; ++count) {
                win->window_buffer[temp_x][temp_y][count] = 0;
            }
        }
    }

    return(1);
}


int codl_write(codl_window *win, char *string) {
    int count;
    int count_1;
    char *ptr;
    int length = (int)codl_strlen(string);

    CODL_NULLPTR_MACRO(!win,                "Window pointer for write is NULL")
    CODL_NULLPTR_MACRO(!win->window_buffer, "Window buffer for write is NULL")

    for(count = 0; count < length; ++count) {
        if(string[count] == '\n') {
            ++win->cursor_pos_y;
            win->cursor_pos_x = 0;

            if(win->cursor_pos_y > win->height) {
                codl_buffer_scroll_down(win, win->cursor_pos_y - win->height);
            }
        } else if(string[count] == '\t') {
            for(count_1 = 0; count_1 < tab_width; ++count_1) {
                ptr = win->window_buffer[win->cursor_pos_x][win->cursor_pos_y];

                if(!codl_memset(ptr, CELL_SIZE, 0, CELL_SIZE)) {
                    codl_set_fault(fault_enum, "Error memset(1) in write function");

                    return(0);
                }

                ptr[0] = ' ';
                ptr[4] = (char)win->colour_bg;
                ptr[5] = (char)win->colour_fg;
                ptr[6] = win->text_attribute;

                ++win->cursor_pos_x;
                if(win->cursor_pos_x > win->width - 1) break;
            }
        } else if(string[count] == '\033') {
            if(count < length - 1) {
                if(string[count + 1] == '[') {
                    count += __codl_parse_ansi_seq(win, string, (size_t)count);
                } else {
                    ++count;
                } 
            } else {
             ++count;
            }
        } else if(string[count] == '\b') {
            codl_set_cursor_position(win, win->cursor_pos_x - 1, win->cursor_pos_y);
            if(!codl_memset(win->window_buffer[win->cursor_pos_x][win->cursor_pos_y], CELL_SIZE, 0, CELL_SIZE)) {
                codl_set_fault(fault_enum, "Error memset(2) in write function");

                return(0);
            }
        } else {
            if(win->cursor_pos_x > win->width - 1) {
                ++win->cursor_pos_y;
                win->cursor_pos_x = 0;
            }

            if(win->cursor_pos_y > win->height - 1) {
                codl_buffer_scroll_down(win, win->cursor_pos_y - (win->height - 1));
                win->cursor_pos_y -= win->cursor_pos_y - (win->height - 1);
            }

            ptr = win->window_buffer[win->cursor_pos_x][win->cursor_pos_y];

            if(!codl_memset(ptr, CELL_SIZE, 0, 4)) {
                codl_set_fault(fault_enum, "Error memset(3) in write function");

                return(0);
            }

            if((UTF8_CODEPOINT_4B & string[count]) == UTF8_CODEPOINT_4B) {
                for(count_1 = 0; count_1 < 4; ++count_1) {
                    ptr[count_1] = string[count + count_1];
                }

                count += 3;

            } else if((UTF8_CODEPOINT_3B & string[count]) == UTF8_CODEPOINT_3B) {
                for(count_1 = 0; count_1 < 3; ++count_1) {
                    ptr[count_1] = string[count + count_1];
                }

                count += 2;

            } else if((UTF8_CODEPOINT_2B & string[count]) == UTF8_CODEPOINT_2B) {
                for(count_1 = 0; count_1 < 2; ++count_1) {
                    ptr[count_1] = string[count + count_1];
                }

                ++count;

            } else {
                ptr[0] = string[count];
            }

            ptr[4] = (char)win->colour_bg;
            ptr[5] = (char)win->colour_fg;
            ptr[6] = win->text_attribute;

            ++win->cursor_pos_x;
        }
    }

    return(1);
}


int codl_save_buffer_to_file(codl_window *win, const char *filename) {
    int count;
    int temp_x;
    int temp_y;
    FILE *output = fopen(filename, "w");

    CODL_NULLPTR_MACRO(!win, "Window pointer for save buffer to file is NULL")
    CODL_NULLPTR_MACRO(!win->window_buffer, "Window buffer for save to file is NULL")

    CODL_NULLPTR_MACRO(!output, "Error open file for save buffer")

    fwrite(&win->width, sizeof(int), 1, output);
    fwrite(&win->height, sizeof(int), 1, output);

    for(temp_y = 0; temp_y < win->height; ++temp_y) {
        for(temp_x = 0; temp_x < win->width; ++temp_x) {
            for(count = 0; count < CELL_SIZE; ++count) {
                fwrite(&win->window_buffer[temp_x][temp_y][count], sizeof(char), 1, output);
            }
        }
    }

    fclose(output);

    return(1);
}


int codl_load_buffer_from_file(codl_window *win, const char *filename, int x_pos, int y_pos) {
    int temp_x;
    int temp_y;
    int count;
    int width;
    int height;
    FILE *input = fopen(filename, "r");

    CODL_NULLPTR_MACRO(!win, "Window pointer for load buffer from file is NULL")
    CODL_NULLPTR_MACRO(!win->window_buffer, "Window buffer for load another buffer from file is NULL")

    CODL_NULLPTR_MACRO(!input, "Error open file for load buffer")

    fread(&width, sizeof(int), 1, input);
    fread(&height, sizeof(int), 1, input);

    for(temp_y = 0; (temp_y < height) && ((temp_y + y_pos) < win->height); ++temp_y) {
        fseek(input, (long int)((2 * sizeof(int)) + ((size_t)(temp_y * width)) * sizeof(char) * CELL_SIZE), SEEK_SET);

        for(temp_x = 0; (temp_x < width) && ((temp_x + x_pos) < win->width); ++temp_x) {
            for(count = 0; count < CELL_SIZE; ++count) {
                fread(&win->window_buffer[temp_x + x_pos][temp_y + y_pos][count], sizeof(char), 1, input);
            }
        }
    }

    fclose(input);

    return(1);
}
