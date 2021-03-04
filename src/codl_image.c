#include "../include/codl_internal.h"

codl_image *codl_load_image(const char *filename) { 
    int temp_x;
    int temp_y;
    int count;
    int width;
    int height;
    FILE *input = fopen(filename, "r");
    codl_image *img = codl_malloc_check(sizeof(codl_image));
    
    CODL_NULLPTR_MACRO(!input, "Error open file for load buffer")
    CODL_NULLPTR_MACRO(!img, "Memory allocation fault")
      
    fread(&width, sizeof(int), 1, input);
    fread(&height, sizeof(int), 1, input);

    img->width  = width;
    img->height = height;

    img->image_buffer = codl_malloc_check((size_t)width * sizeof(char**));

    if(!img->image_buffer) {
        codl_set_fault(fault_enum, "Image buffer memory allocation error");

        return(NULL);
    }
    
    for(temp_x = 0; temp_x < width; ++temp_x) {
        img->image_buffer[temp_x] = codl_malloc_check((size_t)height * sizeof(char*));
        if(!img->image_buffer[temp_x]) {
            codl_set_fault(fault_enum, "Image buffer memory allocation error");

            return(NULL);
        }
      
        for(temp_y = 0; temp_y < height; ++temp_y) {
            img->image_buffer[temp_x][temp_y] = codl_malloc_check(CELL_SIZE * sizeof(char));

            if(!img->image_buffer[temp_x][temp_y]) {
                codl_set_fault(fault_enum, "Image buffer memory allocation error");

                return(NULL);
            }
          
            if(!codl_memset(img->image_buffer[temp_x][temp_y], CELL_SIZE, 0, CELL_SIZE)) {
                codl_set_fault(fault_enum, "Error memset in create image function");

                return(NULL);
            }
        }
    }

    
    for(temp_y = 0; (temp_y < height) && (temp_y < img->height); ++temp_y) {
        fseek(input, (long int)((2 * sizeof(int)) +
                                ((size_t)(temp_y * width)) * sizeof(char) * CELL_SIZE), SEEK_SET);

        for(temp_x = 0; (temp_x < width) && (temp_x < img->width); ++temp_x)
            for(count = 0; count < CELL_SIZE; ++count)
                fread(&img->image_buffer[temp_x][temp_y][count], sizeof(char), 1, input);
    }

    fclose(input);

    return(img);
}

int codl_clear_image(codl_image *img) {
    int temp_x = 0;
    int temp_y = 0;

    CODL_NULLPTR_MACRO(!img, "Image pointer for clear is NULL");

    for(temp_x = 0; temp_x < img->width; ++temp_x) {
        for(temp_y = 0; temp_y < img->height; ++temp_y)
            free(img->image_buffer[temp_x][temp_y]);

        free(img->image_buffer[temp_x]);
    }

    free(img->image_buffer);
    free(img);

    img = NULL;
    
    return(1);
}

int codl_image_to_window(codl_window *win, codl_image *img, int x_pos, int y_pos, int x_reg, int y_reg, int width, int height) {
    int temp_x;
    int temp_y;

    CODL_NULLPTR_MACRO(!win, "Window pointer for load image is NULL")
    CODL_NULLPTR_MACRO(!win->window_buffer, "Window buffer for load image is NULL")

    for(temp_y = y_reg; (temp_y < height) && ((temp_y + y_pos - y_reg) < win->height) && temp_y < img->height; ++temp_y) {
        if(temp_y < 0 || temp_y + y_pos - y_reg < 0)
            continue;
    
        for(temp_x = x_reg; (temp_x < width) && ((temp_x + x_pos - x_reg) < win->width && temp_x < img->width); ++temp_x) {
            if(temp_x < 0 || temp_x + x_pos - x_reg < 0)
                continue;

            codl_memcpy(win->window_buffer[temp_x + x_pos - y_reg][temp_y + y_pos - x_reg],
                        CELL_SIZE, img->image_buffer[temp_x][temp_y], CELL_SIZE);
        }
    }

    __codl_set_region_diff(win->x_position, win->y_position, win->width, win->height);

    return(1);


    return(1);
}
