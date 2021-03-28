#include "codl_internal.h"

static int __read_char_from_stdin(void) {
#if defined(__unix__)
    return getchar();
#elif defined(_WIN32)
    int key = 0;
    DWORD bytes_read = 0;
    DWORD unread_records = 0;
    BOOL success = FALSE;
    HANDLE stdin_handle = GetStdHandle(STD_INPUT_HANDLE);

    GetNumberOfConsoleInputEvents(stdin_handle, &unread_records);
    unread_records -= 1;

    if (!unread_records) {
        return EOF;
    }

    success = ReadConsole(stdin_handle, (LPDWORD) &key, 1, &bytes_read, NULL);

    return success && bytes_read ? key : EOF;
#endif /* __unix__ */
}

unsigned int codl_get_key(void) {
#if defined(__unix__)
    int  oldf;
    struct termios newt, oldt;
#elif defined(_WIN32)
    HANDLE stdin_handle = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
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
#elif defined(_WIN32)
    GetConsoleMode(stdin_handle, &mode);

    SetConsoleMode(stdin_handle, mode & (unsigned long) ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
#endif

    tmp = __read_char_from_stdin();

    if (tmp == EOF) {
#if defined(__unix__)
        tcsetattr(0, TCSANOW, &oldt);
        fcntl(0, F_SETFL, oldf);
#elif defined(_WIN32)
        SetConsoleMode(stdin_handle, mode);
#endif

        return(0);
    } else if((UTF8_CODEPOINT_4B & tmp) == UTF8_CODEPOINT_4B || 
              (UTF8_CODEPOINT_3B & tmp) == UTF8_CODEPOINT_3B ||
              (UTF8_CODEPOINT_2B & tmp) == UTF8_CODEPOINT_2B) {
        codl_memset(unicode_char, UNICODE_CHAR_SIZE, 0, UNICODE_CHAR_SIZE);

        for(; tmp != EOF && count < UNICODE_CHAR_SIZE; ++count) {
            unicode_char[count] = (char)tmp;
            tmp                 = __read_char_from_stdin();
        }

        return(CODL_KEY_UNICODE);
    }

    while(tmp != EOF) {
        tmp_key = (tmp_key * (tmp > 100 ? 1000 : 100)) + (unsigned int)tmp;
        tmp     = __read_char_from_stdin();
    }

#if defined(__unix__)
    tcsetattr(0, TCSANOW, &oldt);
    fcntl(0, F_SETFL, oldf);
#elif defined(_WIN32)
    SetConsoleMode(stdin_handle, mode);
#endif

    return(tmp_key);
}
