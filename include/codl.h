#ifndef CODL_H
#define CODL_H

#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#if defined(_WIN32) || defined(__CYGWIN__)
# if defined(_MSC_VER)
#  define CODL_API __declspec(dllexport)
# elif (defined(__GNUC__) && (__GNUC__ >= 4))
#  define CODL_API __attribute__(dllexport)
# else
#  define CODL_API
# endif /* _MSC_VER */
#elif defined(__unix__)
# if (defined(__GNUC__) && (__GNUC__ >= 4))
#  define CODL_API __attribute__((visibility("default")))
# else
#  define CODL_API
# endif
#else
# define CODL_API
#endif /* _WIN32 || __CYGWIN__ */

#define CODL_NO_ATTRIBUTES 0
#define CODL_BOLD          1
#define CODL_ITALIC        2
#define CODL_UNDERLINE     4
#define CODL_CROSSED_OUT   8
#define CODL_DIM           16

#define CODL_RSIZE_MAX     (SIZE_MAX >> 1)

typedef struct codl_window {
struct codl_window *parent_win;
    int x_position;
    int y_position;
    int ref_x_position;
    int ref_y_position;
    int width;
    int height;
    int layer;
    int cursor_pos_x;
    int cursor_pos_y;
    int s_cur_pos_x;
    int s_cur_pos_y;
    int colour_bg;
    int colour_fg;
    char alpha;
    char text_attribute;
    int window_visible;
    char ***window_buffer;
} codl_window;

typedef enum CODL_COLOURS {
    CODL_BLACK          = 0,
    CODL_RED            = 1,
    CODL_GREEN          = 2,
    CODL_BROWN          = 3,
    CODL_BLUE           = 4,
    CODL_MAGENTA        = 5,
    CODL_CYAN           = 6,
    CODL_WHITE          = 7,
    CODL_GRAY           = 8,
    CODL_BRIGHT_RED     = 9,
    CODL_BRIGHT_GREEN   = 10,
    CODL_YELLOW         = 11,
    CODL_BRIGHT_BLUE    = 12,
    CODL_BRIGHT_MAGENTA = 13,
    CODL_BRIGHT_CYAN    = 14,
    CODL_BRIGHT_WHITE   = 15,
    CODL_DEFAULT_COLOUR = 256
} CODL_COLOURS;

typedef enum CODL_KEYS {
    CODL_KEY_UP            = 0x4427d,
    CODL_KEY_LEFT          = 0x44280,
    CODL_KEY_DOWN          = 0x4427e,
    CODL_KEY_RIGHT         = 0x4427f,
    CODL_KEY_PGUP          = 0x10a389e6,
    CODL_KEY_PGDOWN        = 0x10a38dce,
    CODL_KEY_HOME          = 0x10a37a46,
    CODL_KEY_END           = 0x10a385fe,
    CODL_KEY_INSERT        = 0x10a37e2e,
    CODL_KEY_DELETE        = 0x10a38216,
    CODL_KEY_F1            = 0x43ddc,
    CODL_KEY_F2            = 0x43ddd,
    CODL_KEY_F3            = 0x43dde,
    CODL_KEY_F4            = 0x43ddf,
    CODL_KEY_F5            = 0x7fdc61a6,
    CODL_KEY_F6            = 0x7fdc6976,
    CODL_KEY_F7            = 0x7fdc6d5e,
    CODL_KEY_F8            = 0x7fdc7146,
    CODL_KEY_F9            = 0x7fddd4be,
    CODL_KEY_F10           = 0x7fddd8a6,
    CODL_KEY_F11           = 0x7fdde076,
    CODL_KEY_F12           = 0x7fdde45e,
    CODL_KEY_UNICODE       = 0x554e4943, /* codl_get_key returns CODL_KEY_UNICODE if key is unicode*/ 
    CODL_KEY_UC_A          = 0x41, /* Uppercase keys */
    CODL_KEY_UC_B          = 0x42,
    CODL_KEY_UC_C          = 0x43,
    CODL_KEY_UC_D          = 0x44,
    CODL_KEY_UC_E          = 0x45,
    CODL_KEY_UC_F          = 0x46,
    CODL_KEY_UC_G          = 0x47,
    CODL_KEY_UC_H          = 0x48,
    CODL_KEY_UC_I          = 0x49,
    CODL_KEY_UC_J          = 0x4a,
    CODL_KEY_UC_K          = 0x4b,
    CODL_KEY_UC_L          = 0x4c,
    CODL_KEY_UC_M          = 0x4d,
    CODL_KEY_UC_N          = 0x4e,
    CODL_KEY_UC_O          = 0x4f,
    CODL_KEY_UC_P          = 0x50,
    CODL_KEY_UC_Q          = 0x51,
    CODL_KEY_UC_R          = 0x52,
    CODL_KEY_UC_S          = 0x53,
    CODL_KEY_UC_T          = 0x54,
    CODL_KEY_UC_U          = 0x55,
    CODL_KEY_UC_V          = 0x56,
    CODL_KEY_UC_W          = 0x57,
    CODL_KEY_UC_X          = 0x58,
    CODL_KEY_UC_Y          = 0x59,
    CODL_KEY_UC_Z          = 0x5a,
    CODL_KEY_LC_A          = 0x61, /* Lowercase keys */
    CODL_KEY_LC_B          = 0x62,
    CODL_KEY_LC_C          = 0x63,
    CODL_KEY_LC_D          = 0x64,
    CODL_KEY_LC_E          = 0x65,
    CODL_KEY_LC_F          = 0x66,
    CODL_KEY_LC_G          = 0x67,
    CODL_KEY_LC_H          = 0x68,
    CODL_KEY_LC_I          = 0x69,
    CODL_KEY_LC_J          = 0x6a,
    CODL_KEY_LC_K          = 0x6b,
    CODL_KEY_LC_L          = 0x6c,
    CODL_KEY_LC_M          = 0x6d,
    CODL_KEY_LC_N          = 0x6e,
    CODL_KEY_LC_O          = 0x6f,
    CODL_KEY_LC_P          = 0x70,
    CODL_KEY_LC_Q          = 0x71,
    CODL_KEY_LC_R          = 0x72,
    CODL_KEY_LC_S          = 0x73,
    CODL_KEY_LC_T          = 0x74,
    CODL_KEY_LC_U          = 0x75,
    CODL_KEY_LC_V          = 0x76,
    CODL_KEY_LC_W          = 0x77,
    CODL_KEY_LC_X          = 0x78,
    CODL_KEY_LC_Y          = 0x79,
    CODL_KEY_LC_Z          = 0x7a,
    CODL_KEY_0             = 0x30,
    CODL_KEY_1             = 0x31,
    CODL_KEY_2             = 0x32,
    CODL_KEY_3             = 0x33,
    CODL_KEY_4             = 0x34,
    CODL_KEY_5             = 0x35,
    CODL_KEY_6             = 0x36,
    CODL_KEY_7             = 0x37,
    CODL_KEY_8             = 0x38,
    CODL_KEY_9             = 0x39,
    CODL_KEY_SPACE         = 0x20, /*   */
    CODL_KEY_EXCLAM        = 0x21, /* ! */
    CODL_KEY_AT            = 0x40, /* @ */
    CODL_KEY_QUOTE         = 0x22, /* " */
    CODL_KEY_APOSTROPHE    = 0x27, /* ' */
    CODL_KEY_SHARP         = 0x23, /* # */
    CODL_KEY_DOLLAR        = 0x24, /* $ */
    CODL_KEY_SEMICOLON     = 0x3b, /* ; */
    CODL_KEY_PERCENT       = 0x25, /* % */
    CODL_KEY_CARET         = 0x5e, /* ^ */
    CODL_KEY_COLON         = 0x3a, /* : */
    CODL_KEY_DOT           = 0x2e, /* . */
    CODL_KEY_COMMA         = 0x2c, /* , */
    CODL_KEY_PIPE          = 0x7c, /* | */
    CODL_KEY_BACKSLASH     = 0x5c, /* \ */
    CODL_KEY_SLASH         = 0x2f, /* / */
    CODL_KEY_TILDE         = 0x7e, /* ~ */
    CODL_KEY_GRAVE_ACCENT  = 0x60, /* ` */
    CODL_KEY_L_SQ_BRACKET  = 0x5b, /* [ */
    CODL_KEY_R_SQ_BRACKET  = 0x5d, /* ] */
    CODL_KEY_L_FIG_BRACKET = 0x7b, /* { */
    CODL_KEY_R_FIG_BRACKET = 0x7d, /* } */
    CODL_KEY_L_BRACKET     = 0x28, /* ( */
    CODL_KEY_R_BRACKET     = 0x29, /* ) */
    CODL_KEY_LESS          = 0x3c, /* < */
    CODL_KEY_MORE          = 0x3e, /* > */
    CODL_KEY_QUESTION_MARK = 0x3f, /* ? */
    CODL_KEY_ASTERISK      = 0x2a, /* * */
    CODL_KEY_MINUS         = 0x2d, /* - */
    CODL_KEY_PLUS          = 0x2b, /* + */
    CODL_KEY_EQUAL         = 0x3d, /* = */
    CODL_KEY_AMPERSAND     = 0x26, /* & */
    CODL_KEY_LOW_LINE      = 0x5f, /* _ */
    CODL_KEY_ESC           = 0x1b, /* Escape symbol     \e */
    CODL_KEY_TAB           = 0x09, /* Tabulation symbol \t */
    CODL_KEY_BACKSPACE     = 0x7f, /* Backspace            */
    CODL_KEY_RETURN        = 0x0A  /* Enter, Return,    \n */
} CODL_KEYS;

typedef enum CODL_CURSOR {
    CODL_SHOW,
    CODL_HIDE
} CODL_CURSOR;

typedef enum CODL_SWITCH {
    CODL_ENABLE  = 1,
    CODL_DISABLE = 0
} CODL_SWITCH;

typedef enum CODL_FAULTS {
    CODL_MEMORY_ALLOCATION_FAULT,
    CODL_NULL_POINTER,
    CODL_INVALID_SIZE,
    CODL_NOT_INITIALIZED
} CODL_FAULTS;

typedef size_t codl_rsize_t;

CODL_API int  codl_set_fault(CODL_FAULTS fault_en, const char *fault_str);
CODL_API char *codl_get_fault_string(void);
CODL_API CODL_FAULTS codl_get_fault_enum(void);
CODL_API void *codl_malloc_check(size_t size);
CODL_API void *codl_realloc_check(void *ptrmem, size_t size);
CODL_API void *codl_calloc_check(size_t number, size_t size);
CODL_API int  codl_memset(void *dest, codl_rsize_t destsize, int ch, codl_rsize_t count);
CODL_API int  codl_memcpy(void *dest, codl_rsize_t destsize, const void *src, codl_rsize_t count);
CODL_API size_t codl_strlen(const char *string);
CODL_API size_t codl_string_length(const char *string);
CODL_API char *codl_itoa(int num, char *string);
CODL_API void codl_clear(void);
CODL_API void codl_cursor_mode(CODL_CURSOR cur);
CODL_API int  codl_echo(void);
CODL_API int  codl_noecho(void);
CODL_API int  codl_kbhit(void);
CODL_API unsigned int codl_get_key(void);
CODL_API int  codl_get_term_size(int *width, int *height);
CODL_API codl_window *codl_create_window(codl_window *p_win, int layer, int x_pos, int y_pos, int width, int height);
CODL_API int  codl_initialize(void);
CODL_API int  codl_resize_window(codl_window *win, int width, int height);
CODL_API int  codl_change_window_position(codl_window *win, int new_x_pos, int new_y_pos);
CODL_API int  codl_destroy_window(codl_window *win);
CODL_API int  codl_terminate_window(codl_window *win); /* LEGACY FUNCTION, REFERENCES TO codl_destroy_window FUNCTION */
CODL_API int  codl_end(void);
CODL_API int  codl_change_layer(codl_window *win, int layer);
CODL_API int  codl_buffer_scroll_down(codl_window *win, int down);
CODL_API int  codl_buffer_scroll_up(codl_window *win, int up);
CODL_API int  codl_set_cursor_position(codl_window *win, int x_pos, int y_pos);
CODL_API int  codl_save_cursor_position(codl_window *win);
CODL_API int  codl_restore_cursor_position(codl_window *win);
CODL_API int  codl_set_window_visible(codl_window *win, CODL_SWITCH visible);
CODL_API int  codl_set_colour(codl_window *win, int bg, int fg);
CODL_API int  codl_set_attribute(codl_window *win, char attribute);
CODL_API int  codl_add_attribute(codl_window *win, char attribute);
CODL_API int  codl_remove_attribute(codl_window *win, char attribute);
CODL_API int  codl_set_alpha(codl_window *win, CODL_SWITCH alpha);
CODL_API int  codl_window_clear(codl_window *win);
CODL_API int  codl_write(codl_window *win, char *string);
CODL_API int  codl_save_buffer_to_file(codl_window *win, const char *filename);
CODL_API int  codl_load_buffer_from_file(codl_window *win, const char *filename, int x_pos, int y_pos);
CODL_API int  codl_redraw(void);
CODL_API int  codl_redraw_diff(void);
CODL_API int  codl_display(void);
CODL_API int  codl_display_window(codl_window *win);
CODL_API int  codl_resize_term(void);
CODL_API int  codl_line(codl_window *win, int x1, int y1, int x2, int y2, char *symbol);
CODL_API int  codl_rectangle(codl_window *win, int x0_pos, int y0_pos, int x1_pos, int y1_pos, char *symbol);
CODL_API int  codl_replace_attributes(codl_window *win, int x0_pos, int y0_pos, int x1_pos, int y1_pos);
CODL_API int  codl_set_frame_symbols(char *ch_0, char *ch_1, char *ch_2, char *ch_3, char *ch_4, char *ch_5, 
                                     char *ch_6, char *ch_7);
CODL_API int  codl_frame(codl_window *win, int x0_pos, int y0_pos, int x1_pos, int y1_pos);
CODL_API int  codl_set_frame_colours(int fg_0, int fg_1, int fg_2, int fg_3, int fg_4, int fg_5, int fg_6, int fg_7);
CODL_API void codl_monochrome_mode(CODL_SWITCH mode);
CODL_API codl_window *codl_get_term(void);
CODL_API int  codl_get_tab_width(void);
CODL_API void codl_set_tab_width(int width);
CODL_API int  codl_get_num_of_wins(void);
CODL_API char *codl_get_stored_key(void);
CODL_API int  codl_input_form(codl_window *win, char **str, int pos_x, int pos_y, size_t size);

#endif /* CODL_H */
