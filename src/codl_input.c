#include "codl_internal.h"

#if defined(_WIN32) || defined(__CYGWIN__)

#define KEY_ARRAY_SIZE 26

static CODL_KEYS __virtual_key_code_to_key(WORD virtual_key_code) {
    int count;

    WORD virtual_key_codes[KEY_ARRAY_SIZE] = {
        VK_UP,
        VK_LEFT,
        VK_DOWN,
        VK_RIGHT,
        VK_PRIOR,
        VK_NEXT,
        VK_HOME,
        VK_END,
        VK_INSERT,
        VK_DELETE,
        VK_F1,
        VK_F2,
        VK_F3,
        VK_F4,
        VK_F5,
        VK_F6,
        VK_F7,
        VK_F8,
        VK_F9,
        VK_F10,
        VK_F11,
        VK_F12,
        VK_ESCAPE,
        VK_TAB,
        VK_BACK,
        VK_RETURN
    };

    CODL_KEYS keys[KEY_ARRAY_SIZE] = {
        CODL_KEY_UP,
        CODL_KEY_LEFT,
        CODL_KEY_DOWN,
        CODL_KEY_RIGHT,
        CODL_KEY_PGUP,
        CODL_KEY_PGDOWN,
        CODL_KEY_HOME,
        CODL_KEY_END,
        CODL_KEY_INSERT,
        CODL_KEY_DELETE,
        CODL_KEY_F1,
        CODL_KEY_F2,
        CODL_KEY_F3,
        CODL_KEY_F4,
        CODL_KEY_F5,
        CODL_KEY_F6,
        CODL_KEY_F7,
        CODL_KEY_F8,
        CODL_KEY_F9,
        CODL_KEY_F10,
        CODL_KEY_F11,
        CODL_KEY_F12,
        CODL_KEY_ESC,
        CODL_KEY_TAB,
        CODL_KEY_BACKSPACE,
        CODL_KEY_RETURN
    };

    for(count = 0; count < KEY_ARRAY_SIZE; ++count) {
        if(virtual_key_codes[count] == virtual_key_code) {
            return keys[count];
        }
    }

    return 0;
}
#endif

static int __read_char(void) {
#if defined(__unix__)
    return getchar();
#elif defined(_WIN32) || defined(__CYGWIN__)
    HANDLE stdin_handle = GetStdHandle(STD_INPUT_HANDLE);
    int key = 0;
    INPUT_RECORD input_rec;
    DWORD unread_records = 0;
    DWORD unread_events = 0;
    BOOL success = FALSE;

    memset(&input_rec, 0, sizeof(INPUT_RECORD));

    while (TRUE) {
        GetNumberOfConsoleInputEvents(stdin_handle, &unread_events);

        if(!unread_events) {
            break;
        }

        success = ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &input_rec, 1, &unread_records);

        if(!success || !unread_records) {
            break;
        }

        key = __virtual_key_code_to_key(input_rec.Event.KeyEvent.wVirtualKeyCode);

        if(!key) {
            key = (int) input_rec.Event.KeyEvent.uChar.AsciiChar;
        }
        
        return key;
    }

    return EOF;
#endif
}

unsigned int codl_get_key(void) {
#if defined(__unix__)
    int  oldf;
    struct termios newt, oldt;
    unsigned int tmp_key = 0;
#elif defined(_WIN32) || defined(__CYGWIN__)
    HANDLE stdin_handle = GetStdHandle(STD_INPUT_HANDLE);
    DWORD old_mode = 0;
#endif
    int key = 0;
    int count = 0;

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

/*
 * Windows does not require this loop with an algorithm, 
 * because special keys are assigned in the __read_char() function
 */
#if defined(__unix__)
    while(key != EOF) {
        tmp_key = (tmp_key * (key > 100 ? 1000 : 100)) + (unsigned int)key;
        key     = __read_char();
    }

    key = (int)tmp_key;
#endif

#if defined(__unix__)
    tcsetattr(0, TCSANOW, &oldt);
    fcntl(0, F_SETFL, oldf);
#elif defined(_WIN32) || defined(__CYGWIN__)
    SetConsoleMode(stdin_handle, old_mode);
#endif

    return((unsigned int)key);
}
