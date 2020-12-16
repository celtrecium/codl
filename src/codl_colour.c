#include "codl_internal.h"


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
