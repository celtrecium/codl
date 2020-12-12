#include "codl_internal.h"

int codl_kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(unsigned int)(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF) {
        ungetc(ch, stdin);

        return(1);
    }

    return(0);
}


unsigned int codl_get_key(void) {
    struct termios newt, oldt;
    int  oldf;
    int tmp = 0;
    int count = 0;
    unsigned int tmp_key = 0;
    
    tcgetattr(0, &oldt);
    newt = oldt;
    newt.c_lflag &= (unsigned int)(~ICANON & ~ECHO);
    tcsetattr(0, TCSANOW, &newt);
    oldf = fcntl(0, F_SETFL, 0);
    fcntl(0, F_SETFL, oldf | O_NONBLOCK);

    tmp = getchar();

    if(tmp == EOF) {
        tcsetattr(0, TCSANOW, &oldt);
        fcntl(0, F_SETFL, oldf);
        
        return(0);
    } else if((UTF8_CODEPOINT_4B & tmp) == UTF8_CODEPOINT_4B || 
              (UTF8_CODEPOINT_3B & tmp) == UTF8_CODEPOINT_3B ||
              (UTF8_CODEPOINT_2B & tmp) == UTF8_CODEPOINT_2B) {
        codl_memset(unicode_char, UNICODE_CHAR_SIZE, 0, UNICODE_CHAR_SIZE);

        for(; tmp != EOF && count < UNICODE_CHAR_SIZE; ++count) {
            unicode_char[count] = (char)tmp;
            tmp                 = getchar();
        }

        return(CODL_KEY_UNICODE);
    }

    while(tmp != EOF) {
        tmp_key = (tmp_key * (tmp > 100 ? 1000 : 100)) + (unsigned int)tmp;
        tmp     = getchar();
    }

    tcsetattr(0, TCSANOW, &oldt);
    fcntl(0, F_SETFL, oldf);
    
    return(tmp_key);
}


int codl_get_term_size(int *width, int *height) {
    struct winsize size;

    if(!width || !height) {
        codl_set_fault(CODL_NULL_POINTER, "Width/height pointer is NULL");

        return(0);
    }

    ioctl( 0, TIOCGWINSZ, (char *) &size );

    *height = size.ws_row;
    *width = size.ws_col;

    return(1);
}
