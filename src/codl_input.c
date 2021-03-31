#include "codl_internal.h"

static int __read_char(void) {
#if defined(__unix__)
    return getchar();
#elif defined(_WIN32) || defined(__CYGWIN__)
    HANDLE stdin_handle = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD input_rec;
    DWORD unread_records = 0;
    DWORD unread_events = 0;
    KEY_EVENT_RECORD *key_event;
    BOOL success;
    int key = EOF;

    memset(&input_rec, 0, sizeof(INPUT_RECORD));

    while(TRUE) {
        success = GetNumberOfConsoleInputEvents(stdin_handle, &unread_events);

        if(!success || !unread_events) {
            break;
        }

        success = ReadConsoleInput(stdin_handle, &input_rec, 1, &unread_records);

        if(!success || !unread_records) {
            break;
        }

        key_event = &input_rec.Event.KeyEvent;

        if(input_rec.EventType != KEY_EVENT || !key_event->bKeyDown) {
            continue;
        }

        key = key_event->uChar.AsciiChar;
        
        /* For CODL_KEY_RETURN. */
        if(key == '\r') {
            key = '\n';
        }
        
        break;
    }

    return key;
#endif
}

unsigned int codl_get_key(void) {
#if defined(__unix__)
    int  oldf;
    struct termios newt, oldt;
#elif defined(_WIN32) || defined(__CYGWIN__)
    HANDLE stdin_handle = GetStdHandle(STD_INPUT_HANDLE);
    DWORD old_mode = 0;
#endif
    int key = 0;
    int count = 0;
    unsigned int tmp_key = 0;

#if defined(__unix__)
    tcgetattr(0, &oldt);
    newt = oldt;
    newt.c_lflag &= (unsigned int)(~ICANON & ~ECHO);
    tcsetattr(0, TCSANOW, &newt);
    oldf = fcntl(0, F_SETFL, 0);
    fcntl(0, F_SETFL, oldf | O_NONBLOCK);
#elif defined(_WIN32) || defined(__CYGWIN__)
    GetConsoleMode(stdin_handle, &old_mode);
    
    SetConsoleMode(stdin_handle, old_mode & (uint64_t) ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
#endif

    key = __read_char();

    if(key == EOF) {
#if defined(__unix__)
        tcsetattr(0, TCSANOW, &oldt);
        fcntl(0, F_SETFL, oldf);
#elif defined(_WIN32) || defined(__CYGWIN__)
        SetConsoleMode(stdin_handle, old_mode);
#endif

        return(0);
    }

    if((UTF8_CODEPOINT_4B & key) == UTF8_CODEPOINT_4B || 
       (UTF8_CODEPOINT_3B & key) == UTF8_CODEPOINT_3B ||
       (UTF8_CODEPOINT_2B & key) == UTF8_CODEPOINT_2B) {
        codl_memset(unicode_char, UNICODE_CHAR_SIZE, 0, UNICODE_CHAR_SIZE);

        for(; key != EOF && count < UNICODE_CHAR_SIZE; ++count) {
            unicode_char[count] = (char)key;
            key                 = __read_char();
        }

        return(CODL_KEY_UNICODE);
    }

    while(key != EOF) {
        tmp_key = (tmp_key * (key > 100 ? 1000 : 100)) + (unsigned int)key;
        key     = __read_char();
    }

    key = (int)tmp_key;

#if defined(__unix__)
    tcsetattr(0, TCSANOW, &oldt);
    fcntl(0, F_SETFL, oldf);
#elif defined(_WIN32) || defined(__CYGWIN__)
    SetConsoleMode(stdin_handle, old_mode);
#endif

    return((unsigned int)key);
}
