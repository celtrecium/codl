#include "codl_internal.h"

int codl_set_fault(CODL_FAULTS fault_en, const char *fault_str) {
    int length = 0;
    int count;
    char *str_ptr;

    if(!codl_initialized) {
	      fputs("Library is not initialized, error message: ", stderr);
	      fputs(fault_str, stderr);
        fputc('\n', stderr);
	    
	      return(0);
    }
    
    fault_enum = fault_en;

    str_ptr = fault_str;

    while(*str_ptr++) {
        ++length;
    }

    if(fault_string) {
	      free(fault_string);

	      fault_string = NULL;
    }
    
    fault_string = malloc(((size_t)length * sizeof(char) + 1));

    if(!fault_string) {
        fputs("Memory allocation fault\n", stderr);

        return(0);
    }

    for(count = 0; count < length; ++count) {
        fault_string[count] = fault_str[count];
    }

    fault_string[count] = 0;

    return(1);
}


char *codl_get_fault_string(void) {
    return(fault_string);
}


CODL_FAULTS codl_get_fault_enum(void) {
    return(fault_enum);
}


