#include "codl_internal.h"

char diff_is             = 0;
char mono_mode           = 0;
char *fault_string       = NULL;
char *unicode_char       = NULL;
char frame_symbols[8][5] = {"│", "│", "─", "─", "┌", "┐", "└", "┘"};
int  frame_colours[8]    = {7, 0, 7, 0, 7, 0, 7, 0};
int  tab_width           = 8;
int codl_initialized     = 0;
CODL_FAULTS fault_enum   = CODL_NOT_INITIALIZED;

codl_window *assembly_window;
codl_window *assembly_diff_window;

codl_window_list window_list;

struct termios stored_settings;


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
    unicode_char = codl_malloc_check(UNICODE_CHAR_SIZE * sizeof(char));

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


void codl_set_tab_width(int width) {
    tab_width = width;
}