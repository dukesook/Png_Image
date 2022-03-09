#pragma once

#include <stdbool.h>

typedef struct {
    char* input;            //path to source image
    char* output;           //path to output image
    int packetSize;         //number pixels per line
    bool littleEndian;      //true = little Endian, default = false = Big Endian
    bool leftShiftData;     //deprecated
    unsigned char* data;    //Pointer to the first pixel
    unsigned int width_pixels;     //Width in PIXELS, not bytes
    unsigned int height_pixels;    //Height in PIXELS, not bytes
    unsigned int image_size_pixels;//Width * Height 
    unsigned int image_size_bytes; //Width * Height * bytes_per_pixel
    unsigned int bytes_per_channel;
    char* color_type;
    unsigned int channels;
} PngImage;

int pnt_to_hex(PngImage pngArguments);
