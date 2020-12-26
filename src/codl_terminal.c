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
#if defined(__unix__)
    if(!codl_initialized) return(0);

    tcsetattr(0, TCSANOW, &stored_settings);
#elif defined(_WIN32) || defined(__CYGWIN__)
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), stored_mode);
#endif

    return(1);
}


int codl_noecho(void) {
#if defined(__unix__)
    struct termios noecho_settings;

    if(!codl_initialized) return(0);
    
    noecho_settings = stored_settings;
    noecho_settings.c_lflag &= (unsigned int)(~ICANON & ~ECHO);
    noecho_settings.c_cc[VTIME] = 0;
    noecho_settings.c_cc[VMIN]  = 1;

    tcsetattr(0, TCSANOW, &noecho_settings);
#elif defined(_WIN32) || defined(__CYGWIN__)
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), (DWORD)(~ENABLE_LINE_INPUT));
#endif

    return(1);
}


int codl_get_term_size(int* width, int* height) {
#if defined(__unix__)
    struct winsize size;

    if (!width || !height) {
        codl_set_fault(CODL_NULL_POINTER, "Width/height pointer is NULL");

        return(0);
    }

    ioctl(0, TIOCGWINSZ, (char*)&size);

    *height = size.ws_row;
    *width = size.ws_col;
#elif defined(_WIN32) || defined(__CYGWIN__)
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *width  = csbi.srWindow.Right  - csbi.srWindow.Left + 1;
    *height = csbi.srWindow.Bottom - csbi.srWindow.Top  + 1;
#endif

    return(1);
}
