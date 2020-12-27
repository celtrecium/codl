#include "codl_internal.h"


unsigned int codl_get_key(void) {
#if defined(__unix__)
    int  oldf;
    struct termios newt, oldt;
#elif defined(_WIN32) || defined(__CYGWIN__)
    DWORD old_mode = 0;
    DWORD mode = 0;
    char key[10];
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
  GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &old_mode);
  mode = (DWORD)(~ENABLE_LINE_INPUT);

  SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), mode);
  setvbuf(stdin, NULL, _IONBF, 0);
  codl_memset(key, 10, 0, 10);
#endif

#if defined(__unix__)
    tmp = getchar();

    if(tmp == EOF) {
        tcsetattr(0, TCSANOW, &oldt);
        fcntl(0, F_SETFL, oldf);
#elif defined(_WIN32) || defined(__CYGWIN__)
    if(!kbhit()) {
        SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), old_mode);
#endif
        return(0);
    } else if((UTF8_CODEPOINT_4B & tmp) == UTF8_CODEPOINT_4B || 
              (UTF8_CODEPOINT_3B & tmp) == UTF8_CODEPOINT_3B ||
              (UTF8_CODEPOINT_2B & tmp) == UTF8_CODEPOINT_2B) {
        codl_memset(unicode_char, UNICODE_CHAR_SIZE, 0, UNICODE_CHAR_SIZE);

        for(; tmp != EOF && count < UNICODE_CHAR_SIZE; ++count) {
            unicode_char[count] = (char)tmp;
            tmp                 = fgetc(stdin);
        }

        return(CODL_KEY_UNICODE);
    }

#if defined(__unix__)
    while(tmp != EOF) {
        tmp_key = (tmp_key * (tmp > 100 ? 1000 : 100)) + (unsigned int)tmp;
        tmp     = getchar();
    }

    tcsetattr(0, TCSANOW, &oldt);
    fcntl(0, F_SETFL, oldf);
#elif defined(_WIN32) || defined(__CYGWIN__)

    for(count = 0; key[count];) {
        tmp_key = (tmp_key * (key[count] > 100 ? 1000 : 100)) + (unsigned int)key[count];
        *key = getch();
    }

    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), old_mode);
#endif

    return(tmp_key);
}
