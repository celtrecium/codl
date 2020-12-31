#include "codl_internal.h"


int __codl_reverse(char *string) {
    int count;
    int count_1;
    char ch;

    if(!string) {
        codl_set_fault(CODL_NULL_POINTER, "String pointer is NULL");

        return(0);
    }

    for(count = 0, count_1 = (int)codl_strlen(string) - 1; count < count_1; ++count, --count_1) {
        ch = string[count];
        string[count] = string[count_1];
        string[count_1] = ch;
    }

    return(1);
}


void __codl_int_swap(int *num1, int *num2) {
    *num1 ^= *num2;
    *num2 ^= *num1;
    *num1 ^= *num2;
}


int __codl_clear_window_buffer(codl_window *win) {
    int temp_width;
    int temp_height;

    CODL_NULLPTR_MACRO(!win, "Window pointer for clear is NULL")
    CODL_NULLPTR_MACRO(!win->window_buffer, "Window buffer pointer for clear is NULL")
    
    for(temp_width = 0; temp_width < win->width; ++temp_width) {    
        for(temp_height = 0; temp_height < win->height; ++temp_height) {
            free(win->window_buffer[temp_width][temp_height]);
        }

        free(win->window_buffer[temp_width]);
    }

    free(win->window_buffer);

    return(1);
}


void __codl_parse_attributes_ansi_seq(codl_window *win, char *string) {
    int num;
    int tmp_num;
    char *eptr = string + 2;

    for(; *eptr != 'm';) {
        num = (int)strtol(eptr, NULL, 10);

        if((num >= 30) && (num <= 37)) {
            codl_set_colour(win, win->colour_bg, num - 30);
        }

        if((num >= 40) && (num <= 47)) {
            codl_set_colour(win, num - 40, win->colour_fg);
        }

        switch(num) {
            case 0:
                codl_set_attribute(win, CODL_NO_ATTRIBUTES);
                break;
            case 1:
                codl_add_attribute(win, CODL_BOLD);
                break;
            case 2:
                codl_add_attribute(win, CODL_DIM);
                break;
            case 3:
                codl_add_attribute(win, CODL_ITALIC);
                break;
            case 4:
                codl_add_attribute(win, CODL_UNDERLINE);
                break;
            case 9:
                codl_add_attribute(win, CODL_CROSSED_OUT);
                break;
            case 22:
                codl_remove_attribute(win, CODL_BOLD);
                break;
            case 23:
                codl_remove_attribute(win, CODL_ITALIC);
                break;
            case 24:
                codl_remove_attribute(win, CODL_UNDERLINE);
                break;
            case 29:
                codl_remove_attribute(win, CODL_CROSSED_OUT);
                break;
            case 38:
                for(; (*eptr != ';') && (*eptr != 'm'); ++eptr);
                if(*eptr == ';') ++eptr;
                tmp_num = (int)strtol(eptr, NULL, 10);

                if(tmp_num == 5) {
                    for(; (*eptr != ';') && (*eptr != 'm'); ++eptr);
                    if(*eptr == ';') ++eptr;
                    tmp_num = (int)strtol(eptr, NULL, 10);

                    codl_set_colour(win, win->colour_bg, tmp_num);
                    for(; (*eptr != ';') && (*eptr != 'm'); ++eptr);
                    if(*eptr == ';') ++eptr;
                } else {
                    for(; (*eptr != ';') && (*eptr != 'm'); ++eptr);
                    if(*eptr == ';') ++eptr;
                }

                break;
            case 48:
                for(; (*eptr != ';') && (*eptr != 'm'); ++eptr);
                if(*eptr == ';') ++eptr;
                tmp_num = (int)strtol(eptr, NULL, 10);

                if(tmp_num == 5) {
                    for(; (*eptr != ';') && (*eptr != 'm'); ++eptr);
                    if(*eptr == ';') ++eptr;
                    tmp_num = (int)strtol(eptr, NULL, 10);

                    codl_set_colour(win, tmp_num, win->colour_bg);
                    for(; (*eptr != ';') && (*eptr != 'm'); ++eptr);
                    if(*eptr == ';') ++eptr;
                } else {
                    for(; (*eptr != ';') && (*eptr != 'm'); ++eptr);
                    if(*eptr == ';') ++eptr;
                }

                break;
        }

        for(; (*eptr != ';') && (*eptr != 'm'); ++eptr);
        if(*eptr == ';') ++eptr;
    }
}


int __codl_parse_ansi_seq(codl_window *win, char *string, size_t begin) {
    int count     = 0;
    int count_1   = 0;
    int tmp_cur_y = 0;
    int tmp_cur_x = 0;
    int num       = 0;
    int offs      = 0;
    char *eptr;
    size_t length = codl_strlen(string);

    for(; count < (int)length; ++count) {
        switch(string[count + (int)begin]) {
            case 'A':
                offs = (int)strtol(string + (int)begin + 2, NULL, 10);
                codl_set_cursor_position(win, win->cursor_pos_x, win->cursor_pos_y - offs);
                return(count);
            case 'B':
                offs = (int)strtol(string + (int)begin + 2, NULL, 10);
                codl_set_cursor_position(win, win->cursor_pos_x, win->cursor_pos_y + offs);
                return(count);
            case 'C':
                offs = (int)strtol(string + (int)begin + 2, NULL, 10);
                codl_set_cursor_position(win, win->cursor_pos_x + offs, win->cursor_pos_y);
                return(count);
            case 'D':
                offs = (int)strtol(string + (int)begin + 2, NULL, 10);
                codl_set_cursor_position(win, win->cursor_pos_x - offs, win->cursor_pos_y);
                return(count);
            case 'E':
                offs = (int)strtol(string + (int)begin + 2, NULL, 10);
                codl_set_cursor_position(win, 0, win->cursor_pos_y + offs);
                return(count);
            case 'F':
                offs = (int)strtol(string + (int)begin + 2, NULL, 10);
                codl_set_cursor_position(win, 0, win->cursor_pos_y - offs);
                return(count);
            case 'G':
                codl_set_cursor_position(win, (int)strtol(string + begin + 2, NULL, 10),
                                         win->cursor_pos_y);
                return(count);
            case 'H':
            case 'f':
                if((string[begin + 2] == 'H') || (string[begin + 2] == 'f')) {
                    codl_set_cursor_position(win, 1, 1);
                } else if(string[begin + 2] == ';') {
                    tmp_cur_y = 1;
                    tmp_cur_x = (int)strtol(string + begin + 3, NULL, 10);
                } else {
                    tmp_cur_y = (int)strtol(string + begin + 2, &eptr, 10);
                    if((eptr[0] == 'f') || (eptr[0] == 'H')) {
                        tmp_cur_x = 1;
                    } else {
                        tmp_cur_x = (int)strtol(eptr + 1, NULL, 10);
                    }
                }

                codl_set_cursor_position(win, tmp_cur_x, tmp_cur_y);

                return(count);
            case 'J':
                tmp_cur_y = win->cursor_pos_y;
                num = (int)strtol(string + begin + 2, NULL, 10);

                switch(num) {
                    case 0:
                        codl_window_clear(win);
                    break;
                    case 1:
                        codl_buffer_scroll_down(win, tmp_cur_y + 1);
                        codl_buffer_scroll_up(win, tmp_cur_y + 1);
                        win->cursor_pos_y = tmp_cur_y;
                    break;
                }

                return(count);
            case 'K':
                num = (int)strtol(string + begin + 2, NULL, 10);

                switch(num) {
                    case 0:
                        for(count_1 = win->cursor_pos_x; count_1 < win->width; ++count_1) {
                            if(!codl_memset(win->window_buffer[count_1][win->cursor_pos_y], CELL_SIZE, 0, CELL_SIZE)) {
                                codl_set_fault(fault_enum, "Error memset(1) in __codl_parse_ansi_seq function");

                                return(count);
                            }
                        }

                        break;
                    case 1:
                        for(count_1 = 0; count_1 < win->cursor_pos_x; ++count_1) {
                            if(!codl_memset(win->window_buffer[count_1][win->cursor_pos_y], CELL_SIZE, 0, CELL_SIZE)) {
                                codl_set_fault(fault_enum, "Error memset(2) in __codl_parse_ansi_seq function");

                                return(count);
                            }
                        }

                        break;
                    case 2:
                        for(count_1 = 0; count_1 < win->width; ++count_1) {
                            if(!codl_memset(win->window_buffer[count_1][win->cursor_pos_y], CELL_SIZE, 0, CELL_SIZE)) {
                                codl_set_fault(fault_enum, "Error memset(3) in __codl_parse_ansi_seq function");

                                return(count);
                            }
                        }

                        break;
                }

                return(count);
            case 'S':
                codl_buffer_scroll_down(win, (int)strtol(string + begin + 2, NULL, 10));

                return(count);
            case 'T':
                codl_buffer_scroll_up(win, (int)strtol(string + begin + 2, NULL, 10));

                return(count);
            case 'm':
                __codl_parse_attributes_ansi_seq(win, string + begin);
                return(count);
            case 'n':
                return(count);
            case 's':
                codl_save_cursor_position(win);

                return(count);
            case 'u':
                codl_restore_cursor_position(win);

                return(count);
            case 'l':
                codl_cursor_mode(CODL_HIDE);

                return(0);
            case 'h':
                codl_cursor_mode(CODL_SHOW);

                return(0);
        }
    }

    return(count);
}


int __codl_assembly_to_buffer(codl_window *win) {
    int temp_x;
    int temp_y;
    int par_win_pos_x; 
    int par_win_pos_y; 
    int par_win_width; 
    int par_win_height;

    CODL_NULLPTR_MACRO(!win, "Window pointer for assembly to main buffer is NULL")
    CODL_NULLPTR_MACRO(!win->window_buffer, "Window buffer for assembly to main buffer is NULL")
    CODL_NULLPTR_MACRO(!assembly_window->window_buffer, "Assembly buffer is NULL")

    if(!win->window_visible) return(1);

    par_win_pos_x  = (win->parent_win) ? win->parent_win->x_position : 0;
    par_win_pos_y  = (win->parent_win) ? win->parent_win->y_position : 0;
    par_win_width  = (win->parent_win) ? win->parent_win->width  : assembly_window->width;
    par_win_height = (win->parent_win) ? win->parent_win->height : assembly_window->height;

    for(temp_y = 0; (temp_y < win->height) && ((win->ref_y_position + temp_y) < par_win_height) &&
                    ((win->y_position + temp_y) < assembly_window->height); ++temp_y) {

        if(buffer_diff[temp_y + win->y_position][MODIFIED]) {
            for(temp_x = 0; temp_x < win->width && 
                    (win->ref_x_position + temp_x) < par_win_width &&
                    (win->x_position + temp_x) < assembly_window->width &&
                    (win->x_position + temp_x) < buffer_diff[temp_y + win->y_position][LAST_MODIFIED]; ++temp_x) {

                if(((win->y_position + temp_y) > 0) && ((win->x_position + temp_x) > 0) &&
                        ((win->y_position + temp_y) >= par_win_pos_y) &&
                        ((win->x_position + temp_x) >= par_win_pos_x) && 
                        !(win->alpha && !win->window_buffer[temp_x][temp_y][0])) {

                    codl_memcpy(assembly_window->window_buffer[temp_x + win->x_position][temp_y + win->y_position], CELL_SIZE,
                            win->window_buffer[temp_x][temp_y], CELL_SIZE);
                }
            }
        }
    }

    return(1);
}


void __codl_puts_buffer(char *ptr, char *str, int start) {
    int count;

    for(count = 0; count < (int)codl_strlen(str); ++count) {
        ptr[start + count] = str[count];
    }
}


int __codl_display_buffer_string(int x_start, int temp_y, int string_width) {
    int count;
    int count_1;
    int count_2;
    int temp_x;
    char *ptr;
    char attr_buff[6]     = {CODL_BOLD, CODL_ITALIC, CODL_UNDERLINE, CODL_CROSSED_OUT, CODL_DIM};
    char attr_buff_num[6] = {'1',       '3',     '4',        '9',          '2'};
    char attr_assembly_buff[66] = "\033[";

    char prev_colour_bg = 0;
    char prev_colour_fg = 0;
    char prev_attribute = 0;

    char number_buff[10];

    CODL_NULLPTR_MACRO(!assembly_window->window_buffer, "Assembly buffer is NULL")

    for(temp_x = x_start; temp_x < string_width; ++temp_x) {
        ptr = assembly_window->window_buffer[temp_x < assembly_window->width ? temp_x : assembly_window->width - 1][temp_y];

        if(!mono_mode) {
            if((ptr[6] != prev_attribute) || (ptr[4] != prev_colour_bg) || (ptr[5] != prev_colour_fg)) {
                count_2 = 2;

                for((count_1 = 0); count_1 < 5; ++count_1) {
                    if((attr_buff[count_1] & ((ptr[6] | prev_attribute) ^ ptr[6])) == attr_buff[count_1]) {
                        attr_assembly_buff[count_2]     = '2';
                        attr_assembly_buff[count_2 + 1] = (attr_buff_num[count_1] == '1') ?
                                            '2' : attr_buff_num[count_1];
                        attr_assembly_buff[count_2 + 2] = ';';
                        count_2 += 3;
                    }
                }

                for(count_1 = 0; count_1 < 5; ++count_1) {
                    if((attr_buff[count_1] & ((ptr[6] | prev_attribute) ^ prev_attribute)) == attr_buff[count_1]) {
                        attr_assembly_buff[count_2]     = attr_buff_num[count_1];
                        attr_assembly_buff[count_2 + 1] = ';';
                        count_2 += 2;
                    }
                }

                if((ptr[6] & DEF_COLOUR_BG_BIT) != DEF_COLOUR_BG_BIT) {
                    __codl_puts_buffer(attr_assembly_buff, "49;", count_2);
                    count_2 += 3;

                } else {
                    __codl_puts_buffer(attr_assembly_buff, "48;5;", count_2);
                    count_2 += 5;

                    __codl_puts_buffer(attr_assembly_buff,
                               codl_itoa((int)(unsigned char)ptr[4], number_buff), count_2);
                    
                    count_2 += (int)codl_strlen(codl_itoa((int)(unsigned char)ptr[4], number_buff));

                    attr_assembly_buff[count_2] = ';';
                    ++count_2;


                }

                if((ptr[6] & DEF_COLOUR_FG_BIT) != DEF_COLOUR_FG_BIT) {
                    __codl_puts_buffer(attr_assembly_buff, "39;", count_2);
                    count_2 += 3;

                } else {
                    __codl_puts_buffer(attr_assembly_buff, "38;5;", count_2);
                    count_2 += 5;

                    __codl_puts_buffer(attr_assembly_buff,
                               codl_itoa((int)(unsigned char)ptr[5], number_buff), count_2);
                    
                    count_2 += (int)codl_strlen(codl_itoa((int)(unsigned char)ptr[5], number_buff));

                    attr_assembly_buff[count_2] = ';';
                    ++count_2;
                }


                attr_assembly_buff[count_2 - 1] = 'm';
                attr_assembly_buff[count_2]     =  0;

                fputs(attr_assembly_buff, stdout);

                prev_attribute = ptr[6];
                prev_colour_bg = ptr[4];
                prev_colour_fg = ptr[5];
            }
        }

        if(!ptr[0]) {
            putc(' ', stdout);
        } else {
            for(count = 0; count < 4; ++count) {
                if(ptr[count]) {  
                    putc(ptr[count], stdout);
                }
            }
        }
    }

    return(1);
}


int __codl_get_buffer_string_length(int temp_y) {
    char prev_colour_bg = 0;
    char prev_colour_fg = 0;
    char prev_attribute = 0;
    char prev_symbol    = 0;
    int string_width    = 0;
    int temp_x;
    char *ptr;

    CODL_NULLPTR_MACRO(!assembly_window->window_buffer, "Assembly buffer is NULL")

    for(temp_x = 0; temp_x < assembly_window->width; ++temp_x) {
            ptr = assembly_window->window_buffer[temp_x][temp_y];

            if(prev_symbol              || ptr[4] != prev_colour_bg ||
               ptr[5] != prev_colour_fg || ptr[6] != prev_attribute) {

                string_width     = temp_x;
                prev_colour_bg   = ptr[4];
                prev_colour_fg   = ptr[5];
                prev_attribute   = ptr[6];
        }

        prev_symbol = ptr[0];
    }

    return(string_width);
}


void  __codl_clear_buffer_diffs(void) {
    int temp_y;
    int count;

    for(temp_y = 0; temp_y < assembly_window->height; ++temp_y)
        if(buffer_diff[temp_y][MODIFIED])
            for(count = buffer_diff[temp_y][FIRST_MODIFIED]; count < buffer_diff[temp_y][LAST_MODIFIED]; ++count)
                codl_memset(assembly_window->window_buffer[count][temp_y], CELL_SIZE, 0, CELL_SIZE);

    return;
}


int __codl_display_diff(void) {
    int temp_x;
    int temp_y;
    int temp_ch;
    int string_width;

    CODL_NULLPTR_MACRO(!assembly_window->window_buffer, "Assembly buffer is NULL")
    CODL_NULLPTR_MACRO(!assembly_diff_window->window_buffer, "Assembly different buffer is NULL")

    for(temp_y = 0; temp_y < assembly_window->height; ++temp_y) {
        for(temp_x = buffer_diff[temp_y][FIRST_MODIFIED];
                temp_x < assembly_window->width && temp_x < buffer_diff[temp_y][LAST_MODIFIED]; ++temp_x) {

            for(temp_ch = 0; temp_ch < CELL_SIZE; ++temp_ch) {
                if(assembly_window->window_buffer[temp_x][temp_y][temp_ch] !=
                           assembly_diff_window->window_buffer[temp_x][temp_y][temp_ch]) {
                    string_width = buffer_diff[temp_y][LAST_MODIFIED] + 1;

                    printf("\033[%d;%dH", temp_y, temp_x);
                    __codl_display_buffer_string(temp_x, temp_y, string_width);

                    fputs("\033[0m", stdout);
                    if(temp_y != (assembly_window->height - 1)) {
                        putc('\n', stdout);
                    }

                    temp_x = assembly_window->width;
                    break;
                }
            }
        }
    }

    return(1);
}


int __codl_from_buff_to_diff(void) {
    int temp_x;
    int temp_y;

    CODL_NULLPTR_MACRO(!assembly_window->window_buffer, "Assembly buffer is NULL")
    CODL_NULLPTR_MACRO(!assembly_diff_window->window_buffer, "Assembly different buffer is NULL")

    for(temp_y = 0; temp_y < assembly_window->height; ++temp_y)
        if(buffer_diff[temp_y][MODIFIED])
            for(temp_x = buffer_diff[temp_y][FIRST_MODIFIED]; temp_x < buffer_diff[temp_y][LAST_MODIFIED]; ++temp_x)
                codl_memcpy(assembly_diff_window->window_buffer[temp_x][temp_y], CELL_SIZE,
                        assembly_window->window_buffer[temp_x][temp_y], CELL_SIZE);

    return(1);
}


void __codl_set_line_diff(codl_window *win, int x_pos, int y_pos) {
    int *tmpptr = NULL;

    if(y_pos > win->height || x_pos > win->width ||
            (x_pos + win->x_position) >= assembly_window->width || (y_pos + win->y_position) >= assembly_window->height) return;
    
    tmpptr = buffer_diff[y_pos + win->y_position];

    tmpptr[FIRST_MODIFIED] = tmpptr[FIRST_MODIFIED] > x_pos + win->x_position ?
        x_pos + win->x_position : tmpptr[FIRST_MODIFIED];

    tmpptr[LAST_MODIFIED]  = tmpptr[LAST_MODIFIED]  < x_pos + win->x_position ?
        x_pos + win->x_position : tmpptr[LAST_MODIFIED];

    tmpptr[MODIFIED] = 1;

    return;
}

void __codl_set_region_diff(int x_pos, int y_pos, int width, int height) {
    int y_tmp;

    for(y_tmp = 0; y_tmp < height; ++y_tmp) {
        __codl_set_line_diff(assembly_window, x_pos, y_tmp + y_pos);
        __codl_set_line_diff(assembly_window, x_pos + width, y_tmp + y_pos);
    }

    return;
}
