#include "codl_internal.h"

codl_window *codl_create_window(codl_window *p_win, int layer, int x_pos, int y_pos, int width, int height) {
    codl_window *win = NULL;
    int count;
    int temp_width;
    int temp_height;
    int *temp_layers;

    if(!codl_initialized) return(0);
    
    win = codl_malloc_check(sizeof(codl_window));
    if(!win) {
        codl_set_fault(fault_enum, "Error allocation memory for create window");

        return(NULL);
    }

    win->parent_win = !p_win ? assembly_window : p_win;

    win->x_position = (p_win) ? p_win->x_position + x_pos : x_pos;
    win->y_position = (p_win) ? p_win->y_position + y_pos : y_pos;

    win->ref_x_position = x_pos;
    win->ref_y_position = y_pos;

    win->width = width;
    win->height = height;

    win->cursor_pos_x   = 0;
    win->cursor_pos_y   = 0;
    win->s_cur_pos_x    = 0;
    win->s_cur_pos_y    = 0;
    win->colour_bg      = 256;
    win->colour_fg      = 256;
    win->alpha          = 0;
    win->text_attribute = 0;
    win->window_visible = 1;
	
    win->window_buffer = codl_malloc_check((size_t)width * sizeof(char**));
    CODL_ALLOC_MACRO(win->window_buffer, "Window buffer memory allocation error")

    for(temp_width = 0; temp_width < width; ++temp_width) {
        win->window_buffer[temp_width] = codl_malloc_check((size_t)height * sizeof(char*));
        if(!win->window_buffer[temp_width]) {
            codl_set_fault(fault_enum, "Window buffer memory allocation error");

            return(NULL);
        }

        for(temp_height = 0; temp_height < height; ++temp_height) {
            win->window_buffer[temp_width][temp_height] = codl_malloc_check((size_t)(CELL_SIZE * sizeof(char)));
            if(!win->window_buffer[temp_width][temp_height]) {
                codl_set_fault(fault_enum, "Window buffer memory allocation error");

                return(NULL);
            }

            if(!codl_memset(win->window_buffer[temp_width][temp_height], CELL_SIZE, 0, CELL_SIZE)) {
                codl_set_fault(fault_enum, "Error memset in create window function");

                return(NULL);
            }
        }
    }

    win->layer = layer;

    ++window_list.size;

    window_list.list = codl_realloc_check(window_list.list, (size_t)window_list.size * sizeof(codl_window*));
    CODL_ALLOC_MACRO(window_list.list, "Windows list buffer memory reallocation error")

    window_list.list[window_list.size - 1] = win;

    window_list.order = codl_realloc_check(window_list.order, (size_t)window_list.size * sizeof(int));
    CODL_ALLOC_MACRO(window_list.order, "Window order list buffer memory allocation error")

    temp_layers = codl_malloc_check((size_t)window_list.size * sizeof(int));
    CODL_ALLOC_MACRO(temp_layers, "Temproary layers list buffer memory allocation error")

    for(count = 0; count < window_list.size; ++count) {
        temp_layers[count] = window_list.list[count]->layer;
         window_list.order[count] = count;
    }

    for(count = 0; count < window_list.size - 1; ++count) {
        if(temp_layers[count] > temp_layers[count + 1]) {
            __codl_int_swap(&temp_layers[count], &temp_layers[count + 1]);
            __codl_int_swap(&window_list.order[count],  &window_list.order[count + 1]);

            count = 0;
        }
    }

    free(temp_layers);

    return(win);
}


int codl_resize_window(codl_window *win, int width, int height) {
    int temp_x;
    int temp_y;

    CODL_NULLPTR_MACRO(!win, "Window pointer for resize is NULL")
    CODL_NULLPTR_MACRO(!win->window_buffer, "Window buffer for resize is NULL")

    if(width < win->width) {
        for(temp_x = width; temp_x < win->width; ++temp_x) {
            for(temp_y = 0; temp_y < win->height; ++temp_y) {
                free(win->window_buffer[temp_x][temp_y]);
            }

            free(win->window_buffer[temp_x]);
        }

        win->window_buffer = codl_realloc_check(win->window_buffer, (size_t)width * sizeof(char**));
        CODL_ALLOC_MACRO(win->window_buffer, "Window buffer resize memory reallocation is NULL")

        win->width = width;
    } else if(width > win->width) {
        win->window_buffer = codl_realloc_check(win->window_buffer, (size_t)width * sizeof(char**));
        CODL_ALLOC_MACRO(win->window_buffer, "Window buffer resize memory reallocation error")

        for(temp_x = win->width; temp_x < width; ++temp_x) {
            win->window_buffer[temp_x] = codl_malloc_check((size_t)win->height * sizeof(char*));
            CODL_ALLOC_MACRO(win->window_buffer[temp_x], "Window buffer resize memory allocation error")

            for(temp_y = 0; temp_y < win->height; ++temp_y) {
                win->window_buffer[temp_x][temp_y]    = codl_malloc_check(CELL_SIZE * sizeof(char));
                CODL_ALLOC_MACRO(win->window_buffer[temp_x][temp_y], "Window buffer resize memory allocation error")
                    if(!codl_memset(win->window_buffer[temp_x][temp_y], CELL_SIZE, 0, CELL_SIZE)) {
                    codl_set_fault(fault_enum, "Error memset(1) in resize window function");

                    return(0);
                }
            }
        }

        win->width = width;
    }

    if(height < win->height) {
        for(temp_x = 0; temp_x < win->width; ++temp_x) {
            for(temp_y = height; temp_y < win->height; ++temp_y) {
                free(win->window_buffer[temp_x][temp_y]);
            }

            win->window_buffer[temp_x] = codl_realloc_check(win->window_buffer[temp_x], (size_t)height * sizeof(char*));
            CODL_ALLOC_MACRO(win->window_buffer[temp_x], "Window buffer resize memory reallocation error")
        }

        win->height = height;
    } else if(height > win->height) {
        for(temp_x = 0; temp_x < win->width; ++temp_x) {
            win->window_buffer[temp_x] = codl_realloc_check(win->window_buffer[temp_x], (size_t)height * sizeof(char*));
            CODL_ALLOC_MACRO(win->window_buffer[temp_x], "Window buffer resize memory reallocation error")

            for(temp_y = win->height; temp_y < height; ++temp_y) {
                win->window_buffer[temp_x][temp_y] = codl_malloc_check(CELL_SIZE * sizeof(char));
                CODL_ALLOC_MACRO(win->window_buffer[temp_x][temp_y], "Window buffer resize memory allocation error")
                if(!codl_memset(win->window_buffer[temp_x][temp_y], CELL_SIZE, 0, CELL_SIZE)) {
                    codl_set_fault(fault_enum, "Error memset(2) in resize window");

                    return(0);
                }
            }
        }

        win->height = height;
    }

    return(1);
}


int codl_change_window_position(codl_window *win, int new_x_pos, int new_y_pos) {
    int count;

    CODL_NULLPTR_MACRO(!win, "Window pointer for change position is NULL")
    __codl_set_region_diff(win->x_position, win->y_position, win->width, win->height);

    if(win->parent_win) {
        win->ref_x_position = new_x_pos;
        win->ref_y_position = new_y_pos;

        for(count = 0; count < window_list.size; ++count) {
            if(window_list.list[count]->parent_win) {
                window_list.list[count]->x_position =
                    window_list.list[count]->parent_win->x_position + window_list.list[count]->ref_x_position;
                window_list.list[count]->y_position =
                    window_list.list[count]->parent_win->y_position + window_list.list[count]->ref_y_position;
            }
        }
    }

    __codl_set_region_diff(win->x_position, win->y_position, win->width, win->height);

    return(1);
}


int codl_destroy_window(codl_window *win) {
    int count;
    int *temp_layers;

    CODL_NULLPTR_MACRO(!win, "Window pointer for terminate window is NULL")
    CODL_NULLPTR_MACRO(win == assembly_window, "Window pointer for terminate window is NULL")

    __codl_clear_window_buffer(win);
    free(win);

    if(window_list.size > 1) {
        for(count = 0; window_list.list[count] != win; ++count);

        for(; count < window_list.size - 1; ++count) {
            window_list.order[count] = window_list.order[count + 1];
            window_list.list[count]  = window_list.list[count + 1];
        }
    }

    --window_list.size;

    if(window_list.size) {
        window_list.list  = codl_realloc_check(window_list.list,  (size_t)window_list.size * sizeof(codl_window*));
        CODL_ALLOC_MACRO(window_list.list, "Windows list buffer memory reallocation error")
        window_list.order = codl_realloc_check(window_list.order, (size_t)window_list.size * sizeof(int));
        CODL_ALLOC_MACRO(window_list.order, "Windows order buffer memory reallocation error")

        temp_layers = codl_malloc_check((size_t)window_list.size * sizeof(int));
        CODL_ALLOC_MACRO(temp_layers, "Temproary layers buffer memory allocation error")

        for(count = 0; count < window_list.size; ++count) {
            temp_layers[count] = window_list.list[count]->layer;
             window_list.order[count] = count;
        }

        for(count = 0; count < window_list.size - 1; ++count) {
            if(temp_layers[count] > temp_layers[count + 1]) {    
                __codl_int_swap(&temp_layers[count], &temp_layers[count + 1]);
                __codl_int_swap(&window_list.order[count],  &window_list.order[count + 1]);

                count = 0;
            }
        }

        free(temp_layers);
    } else {
        free(window_list.list);
        free(window_list.order);
    }

    return(1);
}

int codl_terminate_window(codl_window *win) {
    return(codl_destroy_window(win));
}

int codl_change_layer(codl_window *win, int layer) {
    int count;
    int *temp_layers;

    CODL_NULLPTR_MACRO(!win, "Window pointer for change position is NULL")
    CODL_NULLPTR_MACRO(win == assembly_window, "Window pointer for change layer is assembly window pointer")

    win->layer = layer;

    temp_layers = codl_malloc_check((size_t)window_list.size * sizeof(int));

    for(count = 0; count < window_list.size; ++count) {
        temp_layers[count] = window_list.list[count]->layer;
        window_list.order[count]  = count;
    }

    for(count = 0; count < window_list.size - 1; ++count) {
        if(temp_layers[count] > temp_layers[count + 1]) {    
            __codl_int_swap(&temp_layers[count], &temp_layers[count + 1]);
            __codl_int_swap(&window_list.order[count],  &window_list.order[count + 1]);

            count = 0;
        }
    }

    free(temp_layers);

    return(1);
}


int codl_set_window_visible(codl_window *win, CODL_SWITCH visible) {
    CODL_NULLPTR_MACRO(!win, "Window pointer for set visible is NULL")

    win->window_visible = (int)visible;
    __codl_set_region_diff(win->x_position, win->y_position, win->width, win->height);

    return(1);
}


int codl_set_alpha(codl_window *win, CODL_SWITCH alpha) {
    CODL_NULLPTR_MACRO(!win, "Window pointer for set alpha mode is NULL")

    win->alpha = (char)alpha;

    return(1);
}


int codl_window_clear(codl_window *win) {
    int count;
    int count_1;

    CODL_NULLPTR_MACRO(!win, "Window pointer for clear is NULL")

    __codl_set_region_diff(win->x_position, win->y_position, win->width, win->height);

    for(count = 0; count < win->width; ++count) {
        for(count_1 = 0; count_1 < win->height; ++count_1) {
            if(!codl_memset(win->window_buffer[count][count_1], CELL_SIZE, 0, CELL_SIZE)) {
                codl_set_fault(fault_enum, "Error memset in window clear function");

                return(0);
            }
        }
    }

    return(1);
}


int codl_resize_term(void) {
    int term_width  = 0;
    int term_height = 0;
    int count;

    codl_get_term_size(&term_width, &term_height);
    ++term_width;
    ++term_height;

    if(assembly_window->width != term_width || assembly_window->height != term_height) {
        if(assembly_window->height < term_height) {
            buffer_diff = codl_realloc_check(buffer_diff, (size_t)term_height * sizeof(int*));

            for(count = assembly_window->height; count < term_height; ++count) {
                buffer_diff[count] = codl_malloc_check(3 * sizeof(int));
                codl_memset(buffer_diff[count], 3, 0, 3);
            }
        } else {
            for(count = term_height; count < assembly_window->height; ++count) {
                free(buffer_diff[count]);

                buffer_diff[count] = NULL;
            }

            buffer_diff = codl_realloc_check(buffer_diff, (size_t)term_width * sizeof(int*));
        }

        codl_resize_window(assembly_window,      term_width, term_height);
        codl_resize_window(assembly_diff_window, term_width, term_height);
        codl_clear();
        diff_is = 0;

        return(1);
    }

    return(0);
}


