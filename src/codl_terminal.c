#include "codl_internal.h"

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
