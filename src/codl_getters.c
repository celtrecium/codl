#include "codl_internal.h"

codl_window *codl_get_term(void) {
    return(assembly_window);
}


int codl_get_tab_width(void) {
    return(tab_width);
}


int codl_get_num_of_wins(void) {
    return(window_list.size);
}


char *codl_get_stored_key(void) {
    return(unicode_char);
}
