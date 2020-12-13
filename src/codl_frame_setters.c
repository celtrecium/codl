#include "codl_internal.h"


int codl_set_frame_symbols(char *ch_0, char *ch_1, char *ch_2, char *ch_3,
        char *ch_4, char *ch_5, char *ch_6, char *ch_7) {
    if(!(
        codl_memcpy(frame_symbols[0], 5, ch_0, 5) &&
        codl_memcpy(frame_symbols[1], 5, ch_1, 5) &&
        codl_memcpy(frame_symbols[2], 5, ch_2, 5) &&
        codl_memcpy(frame_symbols[3], 5, ch_3, 5) &&
        codl_memcpy(frame_symbols[4], 5, ch_4, 5) &&
        codl_memcpy(frame_symbols[5], 5, ch_5, 5) &&
        codl_memcpy(frame_symbols[6], 5, ch_6, 5) &&
        codl_memcpy(frame_symbols[7], 5, ch_7, 5))) {
        codl_set_fault(fault_enum, "Error memcpy in codl_set_frame_symbols");

        return(0);
    }

    return(1);
}

int codl_set_frame_colours(int fg_0, int fg_1, int fg_2, int fg_3, int fg_4, int fg_5, int fg_6, int fg_7) {
    frame_colours[0] = fg_0;
    frame_colours[1] = fg_1;
    frame_colours[2] = fg_2;
    frame_colours[3] = fg_3;
    frame_colours[4] = fg_4;
    frame_colours[5] = fg_5;
    frame_colours[6] = fg_6;
    frame_colours[7] = fg_7;

    return(1);
}

