/*

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org> */ 

#include <stdio.h>
#include <math.h>
#include "srtm_reader.h"

int SRTM_get_elevation(float latitude, float longitude) {
    /* Calculate file name */
    char filename[512];
    sprintf(filename, "%s/%c%02d%c%03d.hgt",
                    SRTM_FOLDER,
                    (latitude >= 0) ? 'N' : 'S',
                    (int)floor(latitude),
                    (longitude >= 0) ? 'E' : 'W',
                    (int)fabs(floor(longitude)));
    /* Open file */
    FILE* file = fopen(filename, "rb");
    if (file != NULL) {
        /* Automatically check for SRTM1 or SRTM3. */
        int block_size;
        
        #ifdef SRTM_AUTO_CHECK_DATABASE
        fseek(file, 0L, SEEK_END);
        if (ftell(file) > 1201*1201*2)
            block_size = 3601;
        else block_size = 1201;
        #else
        block_size = SRTM_BLOCK_SIZE;
        #endif
        
        int lat_row = (int)((latitude - floor(latitude)) * (block_size - 1));
        int lon_row = (int)((longitude - floor(longitude)) * (block_size - 1));
        lat_row = block_size - 1 - lat_row;
        
        /* Read 16 bit word from file */
        unsigned char read_buff[2];
        fseek(file,(lat_row*block_size+lon_row)*2, SEEK_SET);
        fread(&read_buff, 2, 1, file);
        fclose(file);
        
        /* Convert to little endian */
        int16_t elevation = (read_buff[0] << 8) | read_buff[1];

        return (int)elevation;
    }
    /* If the file fails to open, return -32767 */
    return -32767;
}
