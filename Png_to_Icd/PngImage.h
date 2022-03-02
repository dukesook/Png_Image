#ifndef PROCESSPNG_PNGIMAGE_H
#define PROCESSPNG_PNGIMAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//INPUTS
#define INPUT_10W_5H_BLACK "input/10w_5h_black.png"
#define INPUT_10W_5H_GRADIENT "input/10w_5h_gradient.png"
#define INPUT_8W_6H_RGB "input/8w_6h_rgb.png"
#define INPUT_8W_6H_COLUMNS "input/8w_6h_columns.png"
#define INPUT_MR_CAT "input/MrCat.png"
#define INPUT_BRIDGE "input/Bridge_2560x2160.png"

//OUTPUTS
#define OUTPUT_10W_5H_BLACK "out/10w_5h_black.txt"
#define OUTPUT_10W_5H_GRADIENT "out/10w_5h_gradient.txt"
#define OUTPUT_8w_6h_RGB "out/8w_6h_rgb.txt"
#define OUTPUT_8W_6H_COLUMNS "out/8w_6h_columns.txt"
#define OUTPUT_MR_CAT "out/MrCat.txt"
#define OUTPUT_BRIDGE "out/Bridge_2560x2160.txt"

typedef struct {
    char* input;
    char* output;
    int packetSize;
    bool littleEndian;
    bool leftShiftData;
} PngImage_Arguments;

//HEADER PROTOTYPES
void PngImage_Hex_txt(PngImage_Arguments arguments);




#endif //PROCESSPNG_PNGIMAGE_H
