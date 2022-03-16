#pragma once

#include <stdbool.h>

typedef struct {
    char* input;            //path to source image
    char* output;           //path to output image
    bool littleEndian;      //true = little Endian, default = false = Big Endian
    bool icd;               //add meta data for icd camera
    //bool leftShiftData;     //deprecated
    char* color_type;
    unsigned char* data;    //Pointer to the first pixel
    unsigned int packet_pixel_count;         //number pixels per line
    unsigned int width_pixels;     //Width in PIXELS, not bytes
    unsigned int height_pixels;    //Height in PIXELS, not bytes
    unsigned int image_size_pixels;//Width * Height 
    unsigned int image_size_bytes; //Width * Height * bytes_per_pixel
    unsigned int bytes_per_channel;
    unsigned int channels;
} PngImage;

int png_to_hex(PngImage pngArguments);
