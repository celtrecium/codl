#include "codl.h"

#define CELL_SIZE 7
#define UTF8_CODEPOINT_4B 0xF0
#define UTF8_CODEPOINT_3B 0xE0
#define UTF8_CODEPOINT_2B 0xC0
#define DEF_COLOUR_BG_BIT 0x20
#define DEF_COLOUR_FG_BIT 0x40
#define DEF_COLOURS       0x60
#define UNICODE_CHAR_SIZE 0x04

#define CODL_ALLOC_MACRO(win_alloc, string_err)                     \
    if(!(win_alloc)) {                                              \
        codl_set_fault(CODL_MEMORY_ALLOCATION_FAULT, string_err);   \
                                                                    \
        return(0);                                                  \
    }

#define CODL_NULLPTR_MACRO(ptr, string_err)                         \
    if(ptr) {                                                       \
        codl_set_fault(CODL_NULL_POINTER, string_err);              \
                                                                    \
        return(0);                                                  \
    }

#define CODL_INVSZ_MACRO(num, string_err)                           \
    if(num > CODL_RSIZE_MAX) {                                      \
        codl_set_fault(CODL_INVALID_SIZE, string_err);              \
                                                                    \
        return(0);                                                  \
    }


typedef struct codl_frame_symbols {
    char *ch_0;
    char *ch_1;
    char *ch_2;
    char *ch_3;
    char *ch_4;
    char *ch_5;
    char *ch_6;
    char *ch_7;
} codl_frame_symbols;

typedef struct codl_window_list {
    codl_window **list;
    int *order;
    int size;
} codl_window_list;

static char diff_is = 0;
static char mono_mode = 0;
static char *fault_string = NULL;
static char *unicode_char = NULL;
static int  tab_width = 8;
static CODL_FAULTS fault_enum = CODL_NOT_INITIALIZED;

static codl_window *assembly_window;
static codl_window *assembly_diff_window;

static codl_window_list window_list;

static int  __codl_reverse(char *string);
static void __codl_int_swap(int *num1, int *num2);
static int  __codl_clear_window_buffer(codl_window *win);
static void __codl_parse_attributes_ansi_seq(codl_window *win, char *string);
static int  __codl_parse_ansi_seq(codl_window *win, char *string, size_t begin);
static int  __codl_assembly_to_buffer(codl_window *win);
static int  __codl_display_buffer_string(int x_start, int temp_y, int string_width);
static int  __codl_get_buffer_string_length(int temp_y);
static int  __codl_display_diff(void);
static int  __codl_from_buff_to_diff(void);
static void __codl_puts_buffer(char *ptr, char *str, int start);
static int  frame_colours[8]    = {7, 0, 7, 0, 7, 0, 7, 0};
static char frame_symbols[8][5] = {"│", "│", "─", "─", "┌", "┐", "└", "┘"};
static struct termios stored_settings;
static int codl_initialized = 0;

int codl_set_fault(CODL_FAULTS fault_en, char *fault_str) {
    int length = 0;
    int count;
    char *str_ptr;

    if(!codl_initialized) {
	    fputs("Library is not initialized, error message: ", stderr);
	    fputs(fault_str, stderr);
	    fputc('\n', stderr);
	    
	    return(0);
    }
    
    fault_enum = fault_en;

    str_ptr = fault_str;

    while(*str_ptr++) {
        ++length;
    }

    if(fault_string) {
	    free(fault_string);

	    fault_string = NULL;
    }
    
    fault_string = malloc(((size_t)length * (int)sizeof(char) + 1));

    if(!fault_string) {
        fputs("Memory allocation fault\n", stderr);

        return(0);
    }

    for(count = 0; count < length; ++count) {
        fault_string[count] = fault_str[count];
    }

    fault_string[count] = 0;

    return(1);
}


char *codl_get_fault_string(void) {
    return(fault_string);
}


CODL_FAULTS codl_get_fault_enum(void) {
    return(fault_enum);
}


void *codl_malloc_check(int size) {
    void *tmp = malloc((size_t)size);

    if(!tmp) {
        codl_set_fault(CODL_MEMORY_ALLOCATION_FAULT, "Memory allocation error");

        return(NULL);
    } else {
        return(tmp);
    }
} 


void *codl_realloc_check(void *ptrmem, int size) {
    void *tmp;

    tmp = realloc(ptrmem, (size_t)size);

    if(!tmp) {
        codl_set_fault(CODL_MEMORY_ALLOCATION_FAULT, "Memory reallocation error");
        free(ptrmem);

        return(NULL);
    } else {
        ptrmem = tmp;

        return(tmp);
    }
}


void *codl_calloc_check(size_t number, int size) {
    void *tmp;

    tmp = calloc(number, (size_t)size);

    if(!tmp) {
    codl_set_fault(CODL_MEMORY_ALLOCATION_FAULT, "Memory contiguous allocation error");

    return(NULL);
    }

    return(tmp);
}


int codl_memset(void *dest, codl_rsize_t destsize, int ch, codl_rsize_t count) {
    codl_rsize_t counter = 0;
    
    CODL_NULLPTR_MACRO(!dest, "Dest pointer for memset is NULL")
    CODL_INVSZ_MACRO(destsize, "Count number for memset is bigger than CODL_RSIZE_MAX")
    CODL_INVSZ_MACRO(count, "Dest size number for memset is bigger than CODL_RSIZE_MAX")

    for(; (counter < count) && (counter < destsize); ++counter) {
        *((unsigned char*)dest + counter) = (unsigned char)ch;
    }

    return(1);
}


int codl_memcpy(void *dest, codl_rsize_t destsize, const void *src, codl_rsize_t count) {
    codl_rsize_t counter;
    void *src_cpy = NULL;

    CODL_NULLPTR_MACRO(!dest, "Dest pointer for memcpy is NULL")
    CODL_NULLPTR_MACRO(!src, "Source pointer for memcpy is NULL")
    CODL_INVSZ_MACRO(destsize, "Count number for memcpy is bigger than CODL_RSIZE_MAX")
    CODL_INVSZ_MACRO(count, "Dest size number for memcpy is bigger than CODL_RSIZE_MAX")

    src_cpy = codl_malloc_check((int)count);
    
    for(counter = 0; counter < count; ++counter) {
        *((unsigned char*)src_cpy + counter) = *((const unsigned char*)src + counter);
    }

    for(counter = 0; (counter < count) && (counter < destsize); ++counter) {
        *((unsigned char*)dest + counter) = *((unsigned char*)src_cpy + counter);
    }

    free(src_cpy);
        
    return(1);
}


size_t codl_strlen(char *string) {
    size_t length = 0;

    if(!string) {
        codl_set_fault(CODL_NULL_POINTER, "String pointer is NULL");

        return(0);
    }

    while(*string++) {
        ++length;
    }

    return(length);
}


size_t codl_string_length(char *string) {
    size_t length = 0;
    int count  = 0;

    for(; string[count]; ++length) {
        if(string[count] == '\033' && string[count + 1] == '[') {
            count += 2;

            while((string[count] < 'A' || string[count] > 'Z') &&
                  (string[count] < 'a' || string[count] > 'z') && string[count]) ++count;

            ++count;
            
            if(!string[count]) --length;
        }

        if((UTF8_CODEPOINT_4B & string[count]) == UTF8_CODEPOINT_4B) {
            count += 4;
        } else if((UTF8_CODEPOINT_3B & string[count]) == UTF8_CODEPOINT_3B) {
            count += 3;
        } else if((UTF8_CODEPOINT_2B & string[count]) == UTF8_CODEPOINT_2B) {
            count += 2;
        } else if(string[count]) {
            ++count;
        }
    }

    return(length);
}

/* K&R realisation of reverse, itoa */

static int __codl_reverse(char *string) {
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


char *codl_itoa(int num, char *string) {
    int i = 0;
    int sign;

    if(!string) {
        codl_set_fault(CODL_NULL_POINTER, "String pointer is NULL");

        return(0);
    }

    if ((sign = num) < 0) {
        num = -num;
    }

    do {
        string[i++] = (char)(num % 10 + '0');
    } while ((num /= 10) > 0);

    if (sign < 0) {
        string[i++] = '-';
    }

    string[i] = 0;
    __codl_reverse(string);

    return(string);
}


void codl_clear(void) {
    fputs("\033[49;39m\033[H\033[2J", stdout);
}


void codl_cursor_mode(CODL_CURSOR cur) {
    switch(cur){
        case CODL_SHOW:
            fputs("\033[?25h", stdout);
            break;

        case CODL_HIDE:
            fputs("\033[?25l", stdout);
            break;
    }
}


int codl_echo(void) {
    if(!codl_initialized) return(0);

    tcsetattr(0, TCSANOW, &stored_settings);

    return(1);
}


int codl_noecho(void) {
    struct termios noecho_settings;

    if(!codl_initialized) return(0);
    
    noecho_settings = stored_settings;
    noecho_settings.c_lflag &= (unsigned int)(~ICANON & ~ECHO);
    noecho_settings.c_cc[VTIME] = 0;
    noecho_settings.c_cc[VMIN]  = 1;

    tcsetattr(0, TCSANOW, &noecho_settings);

    return(1);
}


int codl_kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(unsigned int)(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF) {
        ungetc(ch, stdin);

        return(1);
    }

    return(0);
}

unsigned int codl_get_key(void) {
    struct termios newt, oldt;
    int  oldf;
    int tmp = 0;
    int count = 0;
    unsigned int tmp_key = 0;
    
    tcgetattr(0, &oldt);
    newt = oldt;
    newt.c_lflag &= (unsigned int)(~ICANON & ~ECHO);
    tcsetattr(0, TCSANOW, &newt);
    oldf = fcntl(0, F_SETFL, 0);
    fcntl(0, F_SETFL, oldf | O_NONBLOCK);

    tmp = getchar();

    if(tmp == EOF) {
        tcsetattr(0, TCSANOW, &oldt);
        fcntl(0, F_SETFL, oldf);
        
        return(0);
    } else if((UTF8_CODEPOINT_4B & tmp) == UTF8_CODEPOINT_4B || 
              (UTF8_CODEPOINT_3B & tmp) == UTF8_CODEPOINT_3B ||
              (UTF8_CODEPOINT_2B & tmp) == UTF8_CODEPOINT_2B) {
        codl_memset(unicode_char, UNICODE_CHAR_SIZE, 0, UNICODE_CHAR_SIZE);

        for(; tmp != EOF && count < UNICODE_CHAR_SIZE; ++count) {
            unicode_char[count] = (char)tmp;
            tmp                 = getchar();
        }

        return(CODL_KEY_UNICODE);
    }

    while(tmp != EOF) {
        tmp_key = (tmp_key * (tmp > 100 ? 1000 : 100)) + (unsigned int)tmp;
        tmp     = getchar();
    }

    tcsetattr(0, TCSANOW, &oldt);
    fcntl(0, F_SETFL, oldf);
    
    return(tmp_key);
}


int codl_get_term_size(int *width, int *height) {
    struct winsize size;

    if(!width || !height) {
        codl_set_fault(CODL_NULL_POINTER, "Width/height pointer is NULL");

        return(0);
    }

    ioctl( 0, TIOCGWINSZ, (char *) &size );

    *height = size.ws_row;
    *width = size.ws_col;

    return(1);
}


static void __codl_int_swap(int *num1, int *num2) {
    *num1 ^= *num2;
    *num2 ^= *num1;
    *num1 ^= *num2;
}


codl_window *codl_create_window(codl_window *p_win, int layer, int x_pos, int y_pos, int width, int height) {
    codl_window *win = NULL;
    int count;
    int temp_width;
    int temp_height;
    int *temp_layers;

    if(!codl_initialized) return(0);
    
    win = codl_malloc_check((int)sizeof(codl_window));
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
	
    win->window_buffer = codl_malloc_check(width * (int)sizeof(char**));
    CODL_ALLOC_MACRO(win->window_buffer, "Window buffer memory allocation error")

    for(temp_width = 0; temp_width < width; ++temp_width) {
        win->window_buffer[temp_width] = codl_malloc_check(height * (int)sizeof(char*));
        if(!win->window_buffer[temp_width]) {
            codl_set_fault(fault_enum, "Window buffer memory allocation error");

            return(NULL);
        }

        for(temp_height = 0; temp_height < height; ++temp_height) {
            win->window_buffer[temp_width][temp_height] = codl_malloc_check((size_t)(CELL_SIZE * (int)sizeof(char)));
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

    window_list.list = codl_realloc_check(window_list.list, window_list.size * (int)sizeof(codl_window*));
    CODL_ALLOC_MACRO(window_list.list, "Windows list buffer memory reallocation error")

    window_list.list[window_list.size - 1] = win;

    window_list.order = codl_realloc_check(window_list.order, window_list.size * (int)sizeof(int));
    CODL_ALLOC_MACRO(window_list.order, "Window order list buffer memory allocation error")

    temp_layers = codl_malloc_check(window_list.size * (int)sizeof(int));
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


int codl_initialize(void) {
    int width;
    int height;

    if(codl_initialized) {
        codl_set_fault(0, "Library is already initialized");

        return(0);
    }

    codl_initialized = 1;
    
    tcgetattr(0, &stored_settings);

    codl_set_fault(0, "OK");
    unicode_char = codl_malloc_check(UNICODE_CHAR_SIZE * (int)sizeof(char));

    codl_clear();
    codl_noecho();
    codl_cursor_mode(CODL_HIDE);
    codl_get_term_size(&width, &height);
    
    ++width;
    ++height;

    window_list.list  = NULL;
    window_list.order = NULL;
    window_list.size  = 0;
    
    assembly_window      = codl_create_window(NULL, -1, 0, 0, width, height);
    assembly_diff_window = codl_create_window(NULL, -1, 0, 0, width, height);

    if(!assembly_window || !assembly_diff_window) {
	    codl_set_fault(fault_enum, "Memory allocation for library initialization failed");
	    codl_end();

	    return(0);
    }

    return(1);
}


static int __codl_clear_window_buffer(codl_window *win) {
    int temp_width;
    int temp_height;

    CODL_NULLPTR_MACRO(!win, "Window pointer for clear is NULL")
    
    for(temp_width = 0; temp_width < win->width; ++temp_width) {    
        for(temp_height = 0; temp_height < win->height; ++temp_height) {
            free(win->window_buffer[temp_width][temp_height]);
        }

        free(win->window_buffer[temp_width]);
    }

    free(win->window_buffer);

    return(1);
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

        win->window_buffer = codl_realloc_check(win->window_buffer, width * (int)sizeof(char**));
        CODL_ALLOC_MACRO(win->window_buffer, "Window buffer resize memory reallocation is NULL")

        win->width = width;
    } else if(width > win->width) {
        win->window_buffer = codl_realloc_check(win->window_buffer, width * (int)sizeof(char**));
        CODL_ALLOC_MACRO(win->window_buffer, "Window buffer resize memory reallocation error")

        for(temp_x = win->width; temp_x < width; ++temp_x) {
            win->window_buffer[temp_x] = codl_malloc_check(win->height * (int)sizeof(char*));
            CODL_ALLOC_MACRO(win->window_buffer[temp_x], "Window buffer resize memory allocation error")

            for(temp_y = 0; temp_y < win->height; ++temp_y) {
                win->window_buffer[temp_x][temp_y]    = codl_malloc_check(CELL_SIZE * (int)sizeof(char));
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

            win->window_buffer[temp_x] = codl_realloc_check(win->window_buffer[temp_x], height * (int)sizeof(char*));
            CODL_ALLOC_MACRO(win->window_buffer[temp_x], "Window buffer resize memory reallocation error")
        }

        win->height = height;
    } else if(height > win->height) {
        for(temp_x = 0; temp_x < win->width; ++temp_x) {
            win->window_buffer[temp_x] = codl_realloc_check(win->window_buffer[temp_x], height * (int)sizeof(char*));
            CODL_ALLOC_MACRO(win->window_buffer[temp_x], "Window buffer resize memory reallocation error")

            for(temp_y = win->height; temp_y < height; ++temp_y) {
                win->window_buffer[temp_x][temp_y] = codl_malloc_check(CELL_SIZE * (int)sizeof(char));
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

    return(1);
}


int codl_terminate_window(codl_window *win) {
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
        window_list.list  = codl_realloc_check(window_list.list,  window_list.size * (int)sizeof(codl_window*));
        CODL_ALLOC_MACRO(window_list.list, "Windows list buffer memory reallocation error")
        window_list.order = codl_realloc_check(window_list.order, window_list.size * (int)sizeof(int));
        CODL_ALLOC_MACRO(window_list.order, "Windows order buffer memory reallocation error")

        temp_layers = codl_malloc_check(window_list.size * (int)sizeof(int));
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


int codl_end(void) {
    int count;

    if(!codl_initialized) return(0);

    for(count = 0; count < window_list.size; ++count) {
        __codl_clear_window_buffer(window_list.list[count]);
        free(window_list.list[count]);
    }

    free(window_list.order);
    free(window_list.list);
    window_list.size = 0;
    codl_cursor_mode(CODL_SHOW);
    codl_echo();
    codl_initialized = 0;
    
    if(fault_string) {
	      free(fault_string);
	      fault_string = NULL;
    }

    if(unicode_char) free(unicode_char);
    
    return(1);
}


int codl_change_layer(codl_window *win, int layer) {
    int count;
    int *temp_layers;

    CODL_NULLPTR_MACRO(!win, "Window pointer for change position is NULL")
    CODL_NULLPTR_MACRO(win == assembly_window, "Window pointer for change layer is assembly window pointer")

    win->layer = layer;

    temp_layers = codl_malloc_check(window_list.size * (int)sizeof(int));

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


int codl_set_window_visible(codl_window *win, CODL_SWITCH visible) {
    CODL_NULLPTR_MACRO(!win, "Window pointer for set visible is NULL")

    win->window_visible = (int)visible;

    return(1);
}


int codl_set_colour(codl_window *win, int bg, int fg) {
    CODL_NULLPTR_MACRO(!win, "Window pointer for set colour is NULL")
    CODL_NULLPTR_MACRO(!win->window_buffer, "Window buffer for set colour is NULL")

    if(bg != 256 && fg != 256) {
        if(bg < 256) {
            win->colour_bg = (char) bg;
        }

        if(fg < 256) {
            win->colour_fg = (char) fg;
        }

        win->text_attribute |= DEF_COLOURS;

    } else if(bg == 256 && fg != 256) {
        win->colour_bg = 256;

        if(fg < 256) {
            win->colour_fg = (char) fg;
        }

        win->text_attribute |= DEF_COLOUR_FG_BIT;

        if((win->text_attribute & DEF_COLOUR_BG_BIT) == DEF_COLOUR_BG_BIT) {
            win->text_attribute ^= DEF_COLOUR_BG_BIT;
        }

    } else if(fg == 256 && bg != 256) {
        win->colour_fg = 256;

        if(bg < 256) {
            win->colour_bg = (char) bg;
        }

        win->text_attribute |= DEF_COLOUR_BG_BIT;

        if((win->text_attribute & DEF_COLOUR_FG_BIT) == DEF_COLOUR_FG_BIT) {
            win->text_attribute ^= DEF_COLOUR_FG_BIT;
        }

    } else {
        win->colour_bg = 256;
        win->colour_fg = 256;

        if((win->text_attribute & DEF_COLOUR_FG_BIT) == DEF_COLOUR_FG_BIT) {
            win->text_attribute ^= DEF_COLOUR_FG_BIT;
        }

        if((win->text_attribute & DEF_COLOUR_BG_BIT) == DEF_COLOUR_BG_BIT) {
            win->text_attribute ^= DEF_COLOUR_BG_BIT;
        }
    }

    return(1);
}


int codl_set_attribute(codl_window *win, char attribute) {
    char t_attr_tmp;

    CODL_NULLPTR_MACRO(!win, "Window pointer for set attribute is NULL")
    CODL_NULLPTR_MACRO(!win->window_buffer, "Window buffer for set attribute is NULL")

    t_attr_tmp = win->text_attribute;
    win->text_attribute = attribute;


    if((t_attr_tmp & DEF_COLOUR_BG_BIT) == DEF_COLOUR_BG_BIT) {
        win->text_attribute |= DEF_COLOUR_BG_BIT;
    }

    if((t_attr_tmp & DEF_COLOUR_FG_BIT) == DEF_COLOUR_FG_BIT) {
        win->text_attribute |= DEF_COLOUR_FG_BIT;
    }

    return(1);
}


int codl_add_attribute(codl_window *win, char attribute) {
    CODL_NULLPTR_MACRO(!win, "Window pointer for add attribute is NULL")
    CODL_NULLPTR_MACRO(!win->window_buffer, "Window buffer for add attribute is NULL")

    win->text_attribute |= attribute;

    return(1);
}


int codl_remove_attribute(codl_window *win, char attribute) {
    CODL_NULLPTR_MACRO(!win, "Window pointer for remove attribute is NULL")
    CODL_NULLPTR_MACRO(!win->window_buffer, "Window buffer for remove attribute is NULL")

    win->text_attribute |= attribute;
    if((win->text_attribute & attribute) == attribute) {
        win->text_attribute ^= attribute;
    }

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


static void __codl_parse_attributes_ansi_seq(codl_window *win, char *string) {
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


static int __codl_parse_ansi_seq(codl_window *win, char *string, size_t begin) {
    int count     = 0;
    int count_1   = 0;
    int tmp_cur_y = 0;
    int tmp_cur_x = 0;
    int num       = 0;
    char *eptr;
    size_t length = codl_strlen(string);

    for(; count < (int)length; ++count) {
        switch(string[count + (int)begin]) {
            case 'A':
                codl_set_cursor_position(win, win->cursor_pos_x, win->cursor_pos_y - 1);
                return(count);
            case 'B':
                codl_set_cursor_position(win, win->cursor_pos_x, win->cursor_pos_y + 1);
                return(count);
            case 'C':
                codl_set_cursor_position(win, win->cursor_pos_x + 1, win->cursor_pos_y);
                return(count);
            case 'D':
                codl_set_cursor_position(win, win->cursor_pos_x - 1, win->cursor_pos_y);
                return(count);
            case 'E':
                codl_set_cursor_position(win, 0, win->cursor_pos_y + 1);
                return(count);
            case 'F':
                codl_set_cursor_position(win, 0, win->cursor_pos_y - 1);
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

    fwrite(&win->width, (int)sizeof(int), 1, output);
    fwrite(&win->height, (int)sizeof(int), 1, output);

    for(temp_y = 0; temp_y < win->height; ++temp_y) {
        for(temp_x = 0; temp_x < win->width; ++temp_x) {
            for(count = 0; count < CELL_SIZE; ++count) {
                fwrite(&win->window_buffer[temp_x][temp_y][count], (int)sizeof(char), 1, output);
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

    fread(&width, (int)sizeof(int), 1, input);
    fread(&height, (int)sizeof(int), 1, input);

    for(temp_y = 0; (temp_y < height) && ((temp_y + y_pos) < win->height); ++temp_y) {
        fseek(input, (2 * (int)sizeof(int)) + (temp_y * width * (int)sizeof(char) * CELL_SIZE), SEEK_SET);

        for(temp_x = 0; (temp_x < width) && ((temp_x + x_pos) < win->width); ++temp_x) {
            for(count = 0; count < CELL_SIZE; ++count) {
                fread(&win->window_buffer[temp_x + x_pos][temp_y + y_pos][count], (int)sizeof(char), 1, input);
            }
        }
    }

    fclose(input);

    return(1);
}


static int __codl_assembly_to_buffer(codl_window *win) {
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

        for(temp_x = 0; (temp_x < win->width) && ((win->ref_x_position + temp_x) < par_win_width) &&
                        ((win->x_position + temp_x) < assembly_window->width); ++temp_x) {

            if(((win->y_position + temp_y) > 0) && ((win->x_position + temp_x) > 0) &&
                    ((win->y_position + temp_y) >= par_win_pos_y) &&
                    ((win->x_position + temp_x) >= par_win_pos_x) && 
                    !(win->alpha && !win->window_buffer[temp_x][temp_y][0])) {
                
                codl_memcpy(assembly_window->window_buffer[temp_x + win->x_position][temp_y + win->y_position], CELL_SIZE,
                        win->window_buffer[temp_x][temp_y], CELL_SIZE);
            }
        }
    }

    return(1);
}


static void __codl_puts_buffer(char *ptr, char *str, int start) {
    int count;

    for(count = 0; count < (int)codl_strlen(str); ++count) {
        ptr[start + count] = str[count];
    }
}


static int __codl_display_buffer_string(int x_start, int temp_y, int string_width) {
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
        ptr = assembly_window->window_buffer[temp_x][temp_y];

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


static int __codl_get_buffer_string_length(int temp_y) {
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


int codl_redraw(void) {
    int temp_y;
    int string_width;

    CODL_NULLPTR_MACRO(!assembly_window->window_buffer, "Assembly buffer is NULL")

    fputs("\033[0;0H", stdout);
    for(temp_y = 0; temp_y < assembly_window->height; ++temp_y) {
        string_width = __codl_get_buffer_string_length(temp_y);
        __codl_display_buffer_string(1, temp_y, string_width);

        fputs("\033[0m\033[K", stdout);
        if(temp_y != (assembly_window->height - 1)) {
            putc('\n', stdout);
        }
    }

    return(1);
}


static int __codl_display_diff(void) {
    int temp_x;
    int temp_y;
    int temp_ch;
    int string_width;

    CODL_NULLPTR_MACRO(!assembly_window->window_buffer, "Assembly buffer is NULL")
    CODL_NULLPTR_MACRO(!assembly_diff_window->window_buffer, "Assembly different buffer is NULL")

    for(temp_y = 0; temp_y < assembly_window->height; ++temp_y) {
        for(temp_x = 0; temp_x < assembly_window->width; ++temp_x) {

            for(temp_ch = 0; temp_ch < CELL_SIZE; ++temp_ch) {
                if(assembly_window->window_buffer[temp_x][temp_y][temp_ch] !=
                           assembly_diff_window->window_buffer[temp_x][temp_y][temp_ch]) {
                    string_width = __codl_get_buffer_string_length(temp_y);

                    printf("\033[%d;%dH", temp_y, temp_x);
                    __codl_display_buffer_string(temp_x, temp_y, string_width);

                    fputs("\033[0m\033[K", stdout);
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


static int __codl_from_buff_to_diff(void) {
    int temp_x;
    int temp_y;
    int temp_ch;

    CODL_NULLPTR_MACRO(!assembly_window->window_buffer, "Assembly buffer is NULL")
    CODL_NULLPTR_MACRO(!assembly_diff_window->window_buffer, "Assembly different buffer is NULL")

    for(temp_x = 0; temp_x < assembly_window->width; ++temp_x) {
        for(temp_y = 0; temp_y < assembly_window->height; ++temp_y) {
            for(temp_ch = 0; temp_ch < CELL_SIZE; ++temp_ch) {
                assembly_diff_window->window_buffer[temp_x][temp_y][temp_ch] =
                        assembly_window->window_buffer[temp_x][temp_y][temp_ch];
            }
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

    codl_window_clear(assembly_window);

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


int codl_resize_term(void) {
    int term_width  = 0;
    int term_height = 0;

    codl_get_term_size(&term_width, &term_height);
    ++term_width;
    ++term_height;

    if(assembly_window->width != term_width || assembly_window->height != term_height) {
        codl_resize_window(assembly_window,      term_width, term_height);
        codl_resize_window(assembly_diff_window, term_width, term_height);
        codl_clear();
        diff_is = 0;

        return(1);
    }

    return(0);
}


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

int codl_set_frame_symbols(char *ch_0, char *ch_1, char *ch_2, char *ch_3,
        char *ch_4, char *ch_5, char *ch_6, char *ch_7) {
    if(!(
        codl_memcpy(frame_symbols[0], 5, ch_0, 5) &&
        codl_memcpy(frame_symbols[1], 5, ch_1, 5) &&
        codl_memcpy(frame_symbols[2], 5, ch_2, 5) &&
        codl_memcpy(frame_symbols[3], 5, ch_3, 5) &&
        codl_memcpy(frame_symbols[4], 5, ch_4, 5) &&
        codl_memcpy(frame_symbols[5], 5, ch_5, 5) &&
        codl_memcpy(frame_symbols[6], 5, ch_6, 5) &&
        codl_memcpy(frame_symbols[7], 5, ch_7, 5))) {
        codl_set_fault(fault_enum, "Error memcpy in codl_set_frame_symbols");

        return(0);
    }

    return(1);
}

int codl_set_frame_colours(int fg_0, int fg_1, int fg_2, int fg_3, int fg_4, int fg_5, int fg_6, int fg_7) {
    frame_colours[0] = fg_0;
    frame_colours[1] = fg_1;
    frame_colours[2] = fg_2;
    frame_colours[3] = fg_3;
    frame_colours[4] = fg_4;
    frame_colours[5] = fg_5;
    frame_colours[6] = fg_6;
    frame_colours[7] = fg_7;

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


codl_window *codl_get_term(void) {
    return(assembly_window);
}


int codl_get_tab_width(void) {
    return(tab_width);
}


int codl_get_num_of_wins(void) {
    return(window_list.size);
}


void codl_set_tab_width(int width) {
    tab_width = width;
}


char *codl_get_stored_key(void) {
    return(unicode_char);
}
