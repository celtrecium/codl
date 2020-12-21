#include "codl_internal.h"

size_t codl_strlen(const char *string) {
    size_t length = 0;

    if(!string) {
        codl_set_fault(CODL_NULL_POINTER, "String pointer is NULL");

        return(0);
    }

    while(*string++) {
        ++length;
    }

    return(length);
}


size_t codl_string_length(const char *string) {
    size_t length = 0;
    int count  = 0;

    for(; string[count]; ++length) {
        if(string[count] == '\033' && string[count + 1] == '[') {
            count += 2;

            while((string[count] < 'A' || string[count] > 'Z') &&
                  (string[count] < 'a' || string[count] > 'z') && string[count]) ++count;

            ++count;
            
            if(!string[count]) --length;
        }

        if((UTF8_CODEPOINT_4B & string[count]) == UTF8_CODEPOINT_4B) {
            count += 4;
        } else if((UTF8_CODEPOINT_3B & string[count]) == UTF8_CODEPOINT_3B) {
            count += 3;
        } else if((UTF8_CODEPOINT_2B & string[count]) == UTF8_CODEPOINT_2B) {
            count += 2;
        } else if(string[count]) {
            ++count;
        }
    }

    return(length);
}

char *codl_itoa(int num, char *string) {
    int i = 0;
    int sign;

    if(!string) {
        codl_set_fault(CODL_NULL_POINTER, "String pointer is NULL");

        return(0);
    }

    if ((sign = num) < 0) {
        num = -num;
    }

    do {
        string[i++] = (char)(num % 10 + '0');
    } while ((num /= 10) > 0);

    if (sign < 0) {
        string[i++] = '-';
    }

    string[i] = 0;
    __codl_reverse(string);

    return(string);
}
