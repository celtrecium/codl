#include "codl.h"

#define CELL_SIZE 7
#define UTF8_CODEPOINT_4B 0xF0
#define UTF8_CODEPOINT_3B 0xE0
#define UTF8_CODEPOINT_2B 0xC0
#define DEF_COLOUR_BG_BIT 0x20
#define DEF_COLOUR_FG_BIT 0x40
#define DEF_COLOURS       0x60
#define UNICODE_CHAR_SIZE 0x04
#define MODIFIED          0x00
#define FIRST_MODIFIED    0x01
#define LAST_MODIFIED     0x02

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

typedef struct codl_window_list {
    codl_window **list;
    int *order;
    int size;
} codl_window_list;

extern char diff_is;
extern char mono_mode;
extern char *fault_string;
extern char *unicode_char;
extern char frame_symbols[8][5];
extern int  tab_width;
extern int  frame_colours[8];
extern int  codl_initialized;

extern CODL_FAULTS fault_enum;

extern codl_window *assembly_window;
extern codl_window *assembly_diff_window;

extern codl_window_list window_list;

#if defined(__unix__)
extern struct termios stored_settings;
#elif defined(_WIN32) || defined(__CYGWIN__)
extern DWORD stored_mode;
#endif

extern int **buffer_diff;

int  __codl_reverse(char *string);
void __codl_int_swap(int *num1, int *num2);
int  __codl_clear_window_buffer(codl_window *win);
void __codl_parse_attributes_ansi_seq(codl_window *win, char *string);
int  __codl_parse_ansi_seq(codl_window *win, char *string, size_t begin);
int  __codl_assembly_to_buffer(codl_window *win);
int  __codl_display_buffer_string(int x_start, int temp_y, int string_width);
int  __codl_get_buffer_string_length(int temp_y);
int  __codl_display_diff(void);
int  __codl_from_buff_to_diff(void);
void __codl_puts_buffer(char *ptr, char *str, int start);
void __codl_set_line_diff(codl_window *win, int x_pos, int y_pos);
void __codl_set_region_diff(int x_pos, int y_pos, int width, int height);

