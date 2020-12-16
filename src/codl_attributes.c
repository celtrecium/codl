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
