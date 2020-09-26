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
	char ***window_buffer;
} codl_window;

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

typedef struct codl_frame_colours {
	int col_0;
	int col_1;
	int col_2;
	int col_3;
	int col_4;
	int col_5;
	int col_6;
	int col_7;
} codl_frame_colours;

typedef struct codl_windows_list {
	codl_window **list;
	int *order;
	int size;
} codl_windows_list;

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
	CODL_INVALID_SIZE
} CODL_FAULTS;

typedef size_t codl_rsize_t;

char *codl_get_fault_string(void);
CODL_FAULTS codl_get_fault_enum(void);
void *codl_malloc_check(int size);
void *codl_realloc_check(void *ptrmem, int size);
void *codl_calloc_check(size_t number, int size);
int codl_memset(void *dest, codl_rsize_t destsize, int ch, codl_rsize_t count);
int codl_memcpy(void *dest, codl_rsize_t destsize, const void *src, codl_rsize_t count);
size_t codl_strlen(char *string);
char *codl_itoa(int num, char *string);
void codl_clear(void);
void codl_cursor_mode(CODL_CURSOR cur);
void codl_echo(void);
void codl_noecho(void);
int  codl_kbhit(void);
int  codl_get_term_size(int *width, int *height);
int  codl_create_window(codl_window *win, codl_window *p_win, codl_windows_list *wls, int layer, int x_pos, int y_pos,
				int width, int height);
int  codl_create_windows_list(codl_windows_list *wls);
int  codl_initialize(void);
int  codl_resize_window(codl_window *win, int width, int height);
int  codl_change_window_position(codl_window *win, codl_windows_list *wls, int new_x_pos, int new_y_pos);
int  codl_terminate_window(codl_window *win, codl_windows_list *wls);
int  codl_add_window_to_list(codl_window *win, codl_windows_list *wls);
int  codl_clear_by_list(codl_windows_list *wls);
void codl_end(void);
int  codl_change_layer(codl_window *win, codl_windows_list *wls, int layer);
int  codl_buffer_scroll_down(codl_window *win, int down);
int  codl_buffer_scroll_up(codl_window *win, int up);
int  codl_set_cursor_position(codl_window *win, int x_pos, int y_pos);
int  codl_save_cursor_position(codl_window *win);
int  codl_restore_cursor_position(codl_window *win);
int  codl_set_colour(codl_window *win, int bg, int fg);
int  codl_set_attribute(codl_window *win, char attribute);
int  codl_add_attribute(codl_window *win, char attribute);
int  codl_remove_attribute(codl_window *win, char attribute);
int  codl_set_alpha(codl_window *win, CODL_SWITCH alpha);
int  codl_window_clear(codl_window *win);
int  codl_write(codl_window *win, char *string);
int  codl_save_buffer_to_file(codl_window *win, const char *filename);
int  codl_load_buffer_from_file(codl_window *win, const char *filename, int x_pos, int y_pos);
int  codl_redraw(void);
int  codl_redraw_diff(void);
int  codl_display(codl_windows_list *wls);
int  codl_display_window(codl_window *win);
int  codl_resize_term(void);
int  codl_line(codl_window *win, int x1, int y1, int x2, int y2, char *symbol);
int  codl_rectangle(codl_window *win, int x0_pos, int y0_pos, int x1_pos, int y1_pos, char *symbol);
int  codl_replace_attributes(codl_window *win, int x0_pos, int y0_pos, int x1_pos, int y1_pos);
codl_frame_symbols codl_set_frame_symbols(char *ch_0, char *ch_1, char *ch_2, char *ch_3, char *ch_4, char *ch_5,
			char *ch_6, char *ch_7);
int  codl_frame(codl_window *win, codl_frame_colours *colour_fg, int x0_pos, int y0_pos, int x1_pos, int y1_pos, codl_frame_symbols *frsym);
codl_frame_colours codl_set_frame_colours(int fg_0, int fg_1, int fg_2, int fg_3, int fg_4, int fg_5, int fg_6, int fg_7);
void codl_monochrome_mode(CODL_SWITCH mode);
codl_window *codl_get_term(void);
int codl_get_tab_width(void);
void codl_set_tab_width(int width);

#endif /* CODL_H */
