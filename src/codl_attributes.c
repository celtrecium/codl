#include "codl_internal.h"

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
