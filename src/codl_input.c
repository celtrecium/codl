#include "codl_internal.h"


unsigned int codl_get_key(void) {
#if defined(__unix__)
    int  oldf;
    struct termios newt, oldt;
#elif defined(_WIN32) || defined(__CYGWIN__)
    DWORD old_mode = 0;
#endif
    int tmp              = 0;
    int count            = 0;
    unsigned int tmp_key = 0;
    
#if defined(__unix__)
    tcgetattr(0, &oldt);
    newt = oldt;
    newt.c_lflag &= (unsigned int)(~ICANON & ~ECHO);
    tcsetattr(0, TCSANOW, &newt);
    oldf = fcntl(0, F_SETFL, 0);
    fcntl(0, F_SETFL, oldf | O_NONBLOCK);
#elif defined(_WIN32) || defined(__CYGWIN__)
#endif

    tmp = getchar();

    if(tmp == EOF) {
#if defined(__unix__)
        tcsetattr(0, TCSANOW, &oldt);
        fcntl(0, F_SETFL, oldf);
#endif

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

#if defined(__unix__)
    tcsetattr(0, TCSANOW, &oldt);
    fcntl(0, F_SETFL, oldf);
#endif

    return(tmp_key);
}