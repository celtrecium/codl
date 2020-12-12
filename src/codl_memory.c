#include "codl_internal.h"

void *codl_malloc_check(size_t size) {
    void *tmp = malloc(size);

    if(!tmp) {
        codl_set_fault(CODL_MEMORY_ALLOCATION_FAULT, "Memory allocation error");

        return(NULL);
    } else {
        return(tmp);
    }
} 


void *codl_realloc_check(void *ptrmem, size_t size) {
    void *tmp;

    tmp = realloc(ptrmem, size);

    if(!tmp) {
        codl_set_fault(CODL_MEMORY_ALLOCATION_FAULT, "Memory reallocation error");
        free(ptrmem);

        return(NULL);
    } else {
        ptrmem = tmp;

        return(tmp);
    }
}


void *codl_calloc_check(size_t number, size_t size) {
    void *tmp;

    tmp = calloc(number, size);

    if(!tmp) {
    codl_set_fault(CODL_MEMORY_ALLOCATION_FAULT, "Memory contiguous allocation error");

    return(NULL);
    }

    return(tmp);
}


int codl_memset(void *dest, codl_rsize_t destsize, int ch, codl_rsize_t count) {
    codl_rsize_t counter = 0;
    
    CODL_NULLPTR_MACRO(!dest, "Dest pointer for memset is NULL")
    CODL_INVSZ_MACRO(destsize, "Count number for memset is bigger than CODL_RSIZE_MAX")
    CODL_INVSZ_MACRO(count, "Dest size number for memset is bigger than CODL_RSIZE_MAX")

    for(; (counter < count) && (counter < destsize); ++counter) {
        *((unsigned char*)dest + counter) = (unsigned char)ch;
    }

    return(1);
}


int codl_memcpy(void *dest, codl_rsize_t destsize, const void *src, codl_rsize_t count) {
    codl_rsize_t counter;
    void *src_cpy = NULL;

    CODL_NULLPTR_MACRO(!dest, "Dest pointer for memcpy is NULL")
    CODL_NULLPTR_MACRO(!src, "Source pointer for memcpy is NULL")
    CODL_INVSZ_MACRO(destsize, "Count number for memcpy is bigger than CODL_RSIZE_MAX")
    CODL_INVSZ_MACRO(count, "Dest size number for memcpy is bigger than CODL_RSIZE_MAX")

    src_cpy = codl_malloc_check(count);
    
    for(counter = 0; counter < count; ++counter) {
        *((unsigned char*)src_cpy + counter) = *((const unsigned char*)src + counter);
    }

    for(counter = 0; (counter < count) && (counter < destsize); ++counter) {
        *((unsigned char*)dest + counter) = *((unsigned char*)src_cpy + counter);
    }

    free(src_cpy);
        
    return(1);
}


