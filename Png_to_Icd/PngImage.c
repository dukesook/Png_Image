#include "PngImage.h"

#include "stb_image.h" //Library which include basic png operations
#include <stdbool.h>
#include <stdint.h>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define GRAY_SCALE (1)
#define GRAY_WITH_ALPHA_CHANNEL (2)
#define STANDARD_RGB (3)
#define RBG_WITH_ALPHA_CHANNEL (4)

//STRUCT
typedef struct { //Primary struct for storing PNG Image
    unsigned char* data;    //Pointer to the first pixel
    int width;              //Number of Pixels in a row
    int height;             //Number of Pixels in a column
    int channels;           // 1 channel  -> gray scale
} PngImage;                 // 2 channels -> gray scale with transparency
                            // 3 channels -> Pixel = Red Green & Blue
                            // 4 channels -> includes transparency

//GLOBAL VARIABLES
static bool littleEndian = false;
static bool leftShiftData = false;

//FUNCTION PROTOTYPES
static PngImage readPngImage(char* filePath);
static PngImage removeAlphaChannel(PngImage png);
static PngImage PngImage_toGray (PngImage png);
static void greyToTxt_dynamicPacket(PngImage png, char* outputPath, int packetSize);
static void freePngImage(PngImage png);
static void nextPixel(int* x, int* y, int width);
static void writePngImage(PngImage png, char* outputPath);
static void printPngImage(PngImage png);
static void writeTxt(char* txt, char* outpath);
static void finalWrite(PngImage png, FILE* output, int packetSize);
static void writeEOF(FILE* output);
static void writeMetaData(uint16_t x, FILE* output);
static void writeData(uint16_t x, FILE* output);
static void writeHeader(uint16_t x, uint16_t y, uint16_t packetSize, FILE* output);


//***************************************************//
//HEADER FUNCTION                                    //
void PngImage_Hex_txt(PngImage_Arguments args) {

    printf("PROCESS PNG\n");

    //VARIABLES
    char* inputPath = args.input;
    char* outputPath = args.output;
    littleEndian = args.littleEndian;
    leftShiftData = args.leftShiftData;

    //READ
    PngImage png = readPngImage(inputPath);

    //if packet size is not specified
    if (args.packetSize == 0) {
        args.packetSize = png.width;
    }

    //ALPHA CHANNEL
    PngImage png_noAlpha = removeAlphaChannel(png);

    //GRAY
    PngImage grayPng = PngImage_toGray(png_noAlpha);

    //WRITE - optional
//    writePngImage(grayPng, OUTPUT_8W_6H_COLUMNS);

    //PRINT PNG - optional
//    printPngImage(grayPng);

    //WRITE TO TXT
    greyToTxt_dynamicPacket(grayPng, outputPath, args.packetSize);

    //FREE MEMORY
//    freePngImage(png_noAlpha);
//    freePngImage(png);
//    freePngImage(grayPng);
}   //
//***************************************************//


//WRITE TO FILE
static void writeEOF(FILE* output) {
    char* eof = "66 FF FF FF FF 00 00\n";
    fprintf(output, "%s", eof);
}
static void writeMetaData(uint16_t x, FILE* output) {
    //The "leftShiftData" flag should not effect metadata

    //VARIABLES
    unsigned char* ptr;
    ptr = (unsigned char*) &x;

    if (littleEndian) {
        fprintf(output, "%02x ", (int) ptr[0]); //little end first
        fprintf(output, "%02x ", (int) ptr[1]);
    } else { //NO FLAGS
        fprintf(output, "%02x ", (int) ptr[1]);
        fprintf(output, "%02x ", (int) ptr[0]);
    }
}
static void writeData(uint16_t x, FILE* output) {

    //VARIABLES
    unsigned char* ptr;
    ptr = (unsigned char*) &x;

    if (littleEndian && leftShiftData) {
        //1 Convert to Little Endian
        //2 Right shift
        fprintf(output, "%02x ", (int) ptr[0]);
        fprintf(output, "%02x ", (int) ptr[0]);
    } else if (littleEndian) {
        fprintf(output, "%02x ", (int) ptr[0]); //little end first
        fprintf(output, "%02x ", (int) ptr[1]);
    } else if (leftShiftData) {
        fprintf(output, "%02x ", (int) ptr[0]);
        fprintf(output, "%02x ", (int) ptr[0]);
    } else { //NO FLAGS
        fprintf(output, "%02x ", (int) ptr[1]);
        fprintf(output, "%02x ", (int) ptr[0]);
    }


}
static void writeHeader(uint16_t x, uint16_t y, uint16_t packetSize, FILE* output) {

    //PRINT 66
    fprintf(output, "66 ");

    //PRINT X
    writeMetaData(x, output);

    //PRINT Y
    writeMetaData(y, output);

    //PRINT COUNT
    writeMetaData(packetSize, output);

}
static void finalWrite(PngImage png, FILE* output, int packetSize) {
    //ASSUMPTION - All arguments are valid

    //VARIABLES
    int x = 0;
    int y = 0;
    int remainingPixels = (png.width * png.height);
    unsigned char *p = png.data;

    while (packetSize < remainingPixels) {
        //FOR EACH PACKET
        writeHeader(x, y, packetSize, output);
        for (int i = 0; i < packetSize; i++) {
            writeData(*p, output);
            p++;
            remainingPixels--;
            nextPixel(&x, &y, png.width);
        }
        fprintf(output,"\n");
    }

    //LAST PACKET - remaining pixels
    writeHeader(x, y, remainingPixels, output);
    for (int i = 0; i < remainingPixels; i++) {
        writeData(*p, output);
        p++;
    }
    fprintf(output,"\n");

    //EOF - End of Frame
    writeEOF(output);
}

//HELPER FUNCTIONS
static PngImage readPngImage(char* filePath) {

    //LOCAL VARIABLES
    PngImage png;

    //GET IMAGE
    png.data = stbi_load(filePath, &png.width, &png.height, &png.channels, 0);

    //VERIFY
    if (png.data == NULL) {
        printf("File not found: ");
        printf("%s\n", filePath);
        exit(1);
    } else {
        size_t img_size = (png.width * png.height * png.channels);
        printf("   -Image loaded: ");
        printf("Width: %dpx,  Height: %dpx  Channels: %d ", png.width, png.height, png.channels);
        printf("Size: %d\n", img_size);
    }

    return png;
}
static PngImage removeAlphaChannel(PngImage png) {

    //VARIABLES
    int new_channels    = (png.channels == 4) ? 3 : 1;
    size_t img_size     = (png.width * png.height * png.channels);
    size_t new_img_size = (png.width * png.height * new_channels);

    //VERIFY
    if (png.channels == 3 || png.channels == 1) { // No alpha channel detected
        printf("   -No Alpha Channel Detected\n");
        return png;
    }
    else if (png.channels != 4 && png.channels != 2) {
        printf("removeAlphaChannel(): channel count unhandled: %d\n", png.channels);
        exit(1);
    }

    //ALLOCATE NEW IMAGE MEMORY
    unsigned char* new_img = malloc(new_img_size);
    if (new_img == NULL) {
        printf("removeAlphaChannel(): Failed to allocate memory");
        exit(1);
    }

    unsigned char red, green, blue, alpha;
    unsigned char* p, *np;
    for (p = png.data, np = new_img; p != png.data + img_size; p += png.channels, np += new_channels) {
        red = *p;
        green = *(p+1);
        blue = *(p+2);
        alpha = *(p+3);

        np[0] = red;
        np[1] = green;
        np[2] = blue;
    }

    PngImage newPng;
    newPng.data = new_img;
    newPng.width = png.width;
    newPng.height = png.height;
    newPng.channels = new_channels;

    printf("   -Alpha Channel Removed\n");
    return newPng;

}
static PngImage PngImage_toGray (PngImage png) {

    //VARIABLES
    //TODO - remove unneeded variables
    int width, height, channels;
    width = png.width;
    height = png.height;
    channels = png.channels;
    unsigned char *img = png.data;
    //TODO - end remove variables

    if (png.channels == 1) {
        printf("   -PNG is already Gray-Scale\n");
        return png;
    }

    int gray_channels = (channels == 4)? 2: 1;
    size_t img_size = (width * height * channels);
    size_t gray_img_size = width * height * gray_channels;

    //ALLOCATE GRAY IMAGE MEMORY
    unsigned char* gray_img = malloc(gray_img_size);
    if (gray_img == NULL) {
        printf("Unable to allocate memory\n");
        exit(1);
    }

    //p - input image
    //pg - output image
    unsigned char red, green, blue;
    for (unsigned char *p = img, *pg = gray_img; p != img + img_size; p += channels, pg += gray_channels) {
        red = *p;
        green = *(p+1);
        blue = *(p+2);
        *pg = (uint8_t) ((red + green + blue) / 3.0);
//        printf("%02x %02x %02x\n", red, green, blue);
        if (channels == 4) {
            *(pg + 1) = *(p + 3);
        }
    }

    png.data = gray_img;
    png.channels = gray_channels;
    printf("   -Converted to Gray-Scale\n");
    return png;
}
static void greyToTxt_dynamicPacket(PngImage png, char* outputPath, int packetSize) {

    //VARIABLES
    #pragma warning(suppress : 4996) //fopen is unsafe, compiler recommends using fopen_s() instead
    FILE* output = fopen(outputPath, "w");
    int fileSize = (png.width * png.height);

    //VERIFY - grayscale, output is valid
    if (png.channels != 1 && png.channels != 2) {
        printf("ERROR - Image has %d channels instead of 1\n", png.channels);
        printf("Please convert to grayscale\n");
        return;
    } else if (output == NULL) {
        printf("ERROR - %s was not found\n", outputPath);
        return;
    }
    if (png.channels == 2) {
        printf("ERROR - PngImage.c greyToTxt_dynamicPacket: channels == 2\n");
        printf("Remove alpha channel from grayscale\n");
        return;
    }
    if (packetSize > fileSize) {
        printf("WARNING - Packet Size: %d is larger than the entire file: %d\n", packetSize, fileSize);
        packetSize = fileSize;
    } else if (packetSize <= 0) {
        printf("ERROR - Invalid packetSize: %d\n", packetSize);
        exit(1);
    }

    //WRITE FILE
    printf("   -Begin Writing...\n\r");
    finalWrite(png, output, packetSize);
    printf("   -Write Complete\n\r");
    fclose(output);
}
static void nextPixel(int* x, int* y, int width) {
    int lastX = (width - 1);
    if (*x == lastX) {
        *x = 0;
        (*y)++;
    } else {
        (*x)++;
    }
}

//DEBUGGING
static void writePngImage(PngImage png, char* outputPath) {
    //Write PngImage struct to a file (outputPath) for viewing as a PNG image
    int stride = png.width * png.channels;
    stbi_write_png(outputPath, png.width, png.height, png.channels, png.data, stride);
}
static void freePngImage(PngImage png) {
    stbi_image_free(png.data);
}
static void printPngImage(PngImage png) {
    //LOCAL VARIABLES
    size_t img_size = png.width * png.height * png.channels;
    unsigned char red, green, blue, gray, alpha; //TODO - handle 16 bits / pixel by changing from char
    unsigned char* p;

    printf("printPngImage()\n");
    printf("width: %d  Height: %d  Channels: %d  Size: %d  \n", png.width, png.height, png.channels, img_size);

    //ITERATE THROUGH IMAGE
    //TODO - replace (png.data + img_size) with (endOfImage)
    switch (png.channels) {
        case GRAY_SCALE: //1 Channel
            for (p = png.data; p != png.data + img_size; p++) {
                printf("%02x \n", *p);
            }
            break;
        case GRAY_WITH_ALPHA_CHANNEL:
            for (p = png.data; p != png.data + img_size; p += png.channels) {
                gray = *p;
                alpha = *(p+1);
                printf("%02x %02x\n", gray, alpha);
            }
            break;
        case STANDARD_RGB: //3 Channels
            for (p = png.data; p != png.data + img_size; p += png.channels) {
                red = *p;
                green = *(p+1);
                blue = *(p+2);
                printf("%02x %02x %02x\n", red, green, blue);
            }
            break;
        case RBG_WITH_ALPHA_CHANNEL: //4 channels
            for (p = png.data; p != png.data + img_size; p += png.channels) {
                red = *p;
                green = *(p+1);
                blue = *(p+2);
                alpha = *(p+3);
                printf("%02x %02x %02x %02x\n", red, green, blue, alpha);
            }
            break;
        default:
            printf("Number of channels: %d is unaccounted for\n", png.channels);
    }

}


