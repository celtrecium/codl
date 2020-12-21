#include "codl_internal.h"

int  codl_input_form(codl_window *win, char **str, int pos_x, int pos_y, size_t size) {
    codl_window *inp_win = NULL;
    char **inp_str       = NULL;
    int count            = 0;
    int count_1          = 0;
    int pos              = 0;
    size_t len           = 1;
    unsigned int key     = 0;

    CODL_NULLPTR_MACRO(!win, "Window pointer for input form is NULL")
    CODL_NULLPTR_MACRO(!win->window_buffer, "Window buffer pointer for input form is NULL")
    
    if(str) {
        free(*str);
        *str = NULL;
    }

    inp_str = codl_malloc_check(size * sizeof(char*));
    for(count = 0; (size_t)count < size; ++count) {
        inp_str[count] = codl_malloc_check(4 * sizeof(char));
        codl_memset(inp_str[count], 4, 0, 4);
    }

    inp_win = codl_create_window(win, win->layer + 1, pos_x, pos_y, (int)size, 1);
    codl_set_colour(inp_win, win->colour_bg, win->colour_fg);
    codl_rectangle(inp_win, 0, 0, (int)size, 1, " ");
    codl_write(inp_win, "█");

    codl_display();
    
    while(key != CODL_KEY_RETURN) {
        key = codl_get_key();

        switch(key) {
            case 0:
                break;

            case CODL_KEY_UNICODE:
                codl_memcpy(inp_str[pos], 4, codl_get_stored_key(), 4);
                codl_set_cursor_position(inp_win, pos, 0);
                codl_write(inp_win, inp_str[pos]);
                pos += (size_t)pos < size - 1 ? 1 : 0;
                codl_set_cursor_position(inp_win, pos, 0);
                codl_write(inp_win, "█");
                codl_display();

                break;

            case CODL_KEY_BACKSPACE:
                codl_set_cursor_position(inp_win, pos, 0);
                codl_write(inp_win, " ");
                pos -= pos ? 1 : 0;
                codl_set_cursor_position(inp_win, pos, 0);
                codl_write(inp_win, "█");
                codl_memset(inp_str[pos], 4, 0, 4);
                codl_display();
                
                break;

            default:
                if(key < ' ' || key > '~') break;
                codl_memset(inp_str[pos], 4, 0, 4);
                *inp_str[pos] = (char)key;
                codl_set_cursor_position(inp_win, pos, 0);
                codl_write(inp_win, inp_str[pos]);
                pos += (size_t)pos < size - 1 ? 1 : 0;
                codl_set_cursor_position(inp_win, pos, 0);
                codl_write(inp_win, "█");
                codl_display();
                
                break;
        }
    }

    for(count = 0, pos = 0; (size_t)count < size; ++count) 
        for(count_1 = 0; count_1 < 4 && inp_str[count][count_1]; ++count_1, ++len) 

    *str = codl_malloc_check(len * sizeof(char));
    codl_memset(*str, len, 0, len);

    for(count = 0, pos = 0; (size_t)count < size; ++count) {
        for(count_1 = 0; count_1 < 4 && inp_str[count][count_1]; ++count_1, ++pos) 
            (*str)[pos] = inp_str[count][count_1];
        
        free(inp_str[count]);
    }
    
    free(inp_str);

    codl_destroy_window(inp_win);

    return(1);
}
