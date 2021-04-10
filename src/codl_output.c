#include "codl_internal.h"

int codl_redraw(void) {
    int temp_y;
    int string_width;

    CODL_NULLPTR_MACRO(!assembly_window->window_buffer, "Assembly buffer is NULL")

    fputs("\033[1;1H", stdout);

    for(temp_y = 0; temp_y < assembly_window->height; ++temp_y) {
        string_width = __codl_get_buffer_string_length(temp_y);
        __codl_display_buffer_string(0, temp_y, string_width);

        if(string_width != assembly_window->width)
            fputs("\033[0m\033[K", stdout);
        else
            fputs("\033[0m", stdout);

        
        if(temp_y != assembly_window->height - 1) {
            putc('\n', stdout);
        }
    }

    return(1);
}


int codl_redraw_diff(void) {
    CODL_NULLPTR_MACRO(!assembly_window->window_buffer, "Assembly buffer is NULL")

    if(!diff_is) {
        codl_redraw();
    } else {
        __codl_display_diff();
    }

    __codl_from_buff_to_diff();

    diff_is = 1;

    return(1);
}


int codl_display(void) {
    int count;
    char buffer[BUFSIZ];
    
    __codl_clear_buffer_diffs();

    setbuf(stdout, buffer);

    for(count = 0; count < window_list.size; ++count) {
        if(window_list.list[window_list.order[count]] != assembly_window && 
            window_list.list[window_list.order[count]] != assembly_diff_window) 
            __codl_assembly_to_buffer(window_list.list[window_list.order[count]]);
    }
    
    if(!diff_is) {
        codl_redraw();
    } else {
        __codl_display_diff();
    }

    __codl_from_buff_to_diff();

    diff_is = 1;
   
    for(count = 0; count < assembly_window->height; ++count)
        codl_memset(buffer_diff[count], 3 * sizeof(int), 0, 3 * sizeof(int));

    fflush(stdout);
    setbuf(stdout, NULL);

    return(1);
}


int codl_display_window(codl_window *win) {
    CODL_NULLPTR_MACRO(!win, "Window pointer for display is NULL")
    CODL_NULLPTR_MACRO(win == assembly_window, "Window pointer is assembly window")

    __codl_assembly_to_buffer(win);

    if(!diff_is) {
        codl_redraw();
    } else {
        __codl_display_diff();
    }

    __codl_from_buff_to_diff();

    diff_is = 1;

    return(1);
}


