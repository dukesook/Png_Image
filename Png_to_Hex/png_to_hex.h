#pragma once

#include <stdbool.h>

typedef struct {
    char* input;
    char* output;
    int packetSize;
    bool littleEndian;
    bool leftShiftData;
} PngImage_Arguments;

int pnt_to_hex(PngImage_Arguments pngArguments);
