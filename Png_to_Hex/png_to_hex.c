//Documentation: https://libspng.org/
//source code: https://github.com/randy408/libspng/blob/v0.7.2/examples/example.c

#include "spng.h"
#include "png_to_hex.h"
//#include <inttypes.h>
#include <stdio.h>

//COLOR
static const char* getColorType(enum spng_color_type color_type) {
    switch (color_type) {
        case SPNG_COLOR_TYPE_GRAYSCALE: return "grayscale";
        case SPNG_COLOR_TYPE_TRUECOLOR: return "RGB";
        case SPNG_COLOR_TYPE_INDEXED: return "indexed color";
        case SPNG_COLOR_TYPE_GRAYSCALE_ALPHA: return "grayscale + alpha";
        case SPNG_COLOR_TYPE_TRUECOLOR_ALPHA: return "RGB + alpha";
        default: return "(invalid)";
    }
}
static int getFormat(struct spng_ihdr ihdr) {

    /* Output format, does not depend on source PNG format except for
    SPNG_FMT_PNG, which is the PNG's format in host-endian or
    big-endian for SPNG_FMT_RAW.
    Note that for these two formats <8-bit images are left byte-packed */
    int fmt = SPNG_FMT_PNG;

    /* With SPNG_FMT_PNG indexed color images are output as palette indices,
       pick another format to expand them. */
    if (ihdr.color_type == SPNG_COLOR_TYPE_INDEXED)
        fmt = SPNG_FMT_RGB8;

    return fmt;
}
static unsigned int getChannelCount(uint8_t color_type) {
    //ihdr.color_type
    switch (color_type) {
    case SPNG_COLOR_TYPE_GRAYSCALE: return 1;
    case SPNG_COLOR_TYPE_TRUECOLOR: return 3;
    case SPNG_COLOR_TYPE_INDEXED:
        printf("ERROR - Unhandled image: indexed image\n");
        exit(1);
    case SPNG_COLOR_TYPE_GRAYSCALE_ALPHA: return 2;
    case SPNG_COLOR_TYPE_TRUECOLOR_ALPHA: return 4;
    default:
        printf("ERROR - Unhandled color_type: %x\n", color_type);
        exit(1);
    }
}

//SPNG FUNCTIONS
static int encode_image(void* image, size_t length, uint32_t width, uint32_t height, enum spng_color_type color_type, int bit_depth) {
    int fmt;
    int ret = 0;
    spng_ctx* ctx = NULL;
    struct spng_ihdr ihdr = { 0 }; /* zero-initialize to set valid defaults */

    /* Creating an encoder context requires a flag */
    ctx = spng_ctx_new(SPNG_CTX_ENCODER);

    /* Encode to internal buffer managed by the library */
    spng_set_option(ctx, SPNG_ENCODE_TO_BUFFER, 1);

    /* Alternatively you can set an output FILE* or stream with spng_set_png_file() or spng_set_png_stream() */

    /* Set image properties, this determines the destination image format */
    ihdr.width = width;
    ihdr.height = height;
    ihdr.color_type = color_type;
    ihdr.bit_depth = bit_depth;
    /* Valid color type, bit depth combinations: https://www.w3.org/TR/2003/REC-PNG-20031110/#table111 */

    spng_set_ihdr(ctx, &ihdr);

    /* When encoding fmt is the source format */
    /* SPNG_FMT_PNG is a special value that matches the format in ihdr */
    fmt = SPNG_FMT_PNG;

    /* SPNG_ENCODE_FINALIZE will finalize the PNG with the end-of-file marker */
    ret = spng_encode_image(ctx, image, length, fmt, SPNG_ENCODE_FINALIZE);

    if (ret)
    {
        printf("spng_encode_image() error: %s\n", spng_strerror(ret));
        goto encode_error;
    }

    size_t png_size;
    void* png_buf = NULL;

    /* Get the internal buffer of the finished PNG */
    png_buf = spng_get_png_buffer(ctx, &png_size, &ret);

    if (png_buf == NULL)
    {
        printf("spng_get_png_buffer() error: %s\n", spng_strerror(ret));
    }

    /* User owns the buffer after a successful call */
    free(png_buf);

encode_error:

    spng_ctx_free(ctx);

    return ret;
}
static spng_ctx* init_context() {
    spng_ctx* ctx = NULL;

    ctx = spng_ctx_new(0);

    if (ctx == NULL) {
        printf("ERROR - spng_ctx_new() failed\n"); exit(0);
    }

    /* Ignore and don't calculate chunk CRC's */
    spng_set_crc_action(ctx, SPNG_CRC_USE, SPNG_CRC_USE);

    /* Set memory usage limits for storing standard and unknown chunks,
   this is important when reading untrusted files! */
    size_t limit = 1024 * 1024 * 64;
    spng_set_chunk_limits(ctx, limit, limit);

    return ctx;
}
static struct spng_ihdr getHeader(spng_ctx* ctx) {
    struct spng_ihdr ihdr;

    int ret = spng_get_ihdr(ctx, &ihdr);

    if (ret) {
        printf("spng_get_ihdr() error: %s\n", spng_strerror(ret));
        exit(1);
    }

    return ihdr;
}
static struct spng_plte getPallete(spng_ctx* ctx) {
    struct spng_plte plte = { 0 };
    int ret = spng_get_plte(ctx, &plte);

    if (ret && ret != SPNG_ECHUNKAVAIL) {
        printf("spng_get_plte() error: %s\n", spng_strerror(ret));
        exit(1);
    }

    if (!ret) printf("palette entries: %u\n", plte.n_entries);

    return plte;

}
static unsigned int getImageSize(spng_ctx* ctx, int fmt) {
    size_t image_size;
    int ret = spng_decoded_image_size(ctx, fmt, &image_size);

    if (ret) {
        printf("ERROR - could not get image size\n");
        exit(1);
    } 

    return (unsigned int) image_size;

}
static void decodeImageSize(spng_ctx* ctx, int fmt, size_t* image_size) {
    int ret = spng_decoded_image_size(ctx, fmt, image_size);

    if (ret) {
        exit(1);
    }
}
static void decodeImage(spng_ctx* ctx, struct spng_ihdr ihdr, int fmt, unsigned char* image, size_t image_width) {
    /* Decode the image in one go */
/* ret = spng_decode_image(ctx, image, image_size, SPNG_FMT_RGBA8, 0);
if(ret)
{
    printf("spng_decode_image() error: %s\n", spng_strerror(ret));
    goto error;
}*/

/* Alternatively you can decode the image progressively,
   this requires an initialization step. */
    int ret = spng_decode_image(ctx, NULL, 0, fmt, SPNG_DECODE_PROGRESSIVE);

    if (ret) {
        printf("progressive spng_decode_image() error: %s\n", spng_strerror(ret));
        exit(1);
    }


    struct spng_row_info row_info = { 0 };

    do {
        ret = spng_get_row_info(ctx, &row_info);
        if (ret) break;

        ret = spng_decode_row(ctx, image + row_info.row_num * image_width, image_width);
    } while (!ret);

    if (ret != SPNG_EOI) {
        printf("progressive decode error: %s\n", spng_strerror(ret));

        if (ihdr.interlace_method)
            printf("last pass: %d, scanline: %u\n", row_info.pass, row_info.scanline_idx);
        else
            printf("last row: %u\n", row_info.row_num);
    }
}
static void printHeader(struct spng_ihdr ihdr) {
    char* color_type = getColorType(ihdr.color_type);
    unsigned int channelCount = getChannelCount(ihdr.color_type);
    unsigned int bits_per_pixel = ihdr.bit_depth;
    unsigned int bit_depth = bits_per_pixel * channelCount; //spng defines "bit_depth" differently than Windows 10

    printf("   Pixel Width......%u\n", ihdr.width);
    printf("   Pixel Height.....%u\n", ihdr.height);
    printf("   Color Type.......%s\n", color_type);
    printf("   Bits/pixel.......%u\n", bits_per_pixel);
    printf("   Channel Count....%d\n", channelCount);
    printf("   Bit Depth........%d\n", bit_depth);
}

//PNG OPERATIONS
static FILE* getFile(char* inputPath) {
    FILE* input_fp;
#pragma warning(suppress : 4996) //should use fopen_s() instead
    input_fp = fopen(inputPath, "rb");

    if (input_fp == NULL) {
        printf("ERROR - file not found: %s\n", inputPath);
        exit(1);
    }
    return input_fp;
}
static PngImage removeAlphaChannel(PngImage png) {

    //VARIABLES
    int new_channels = (png.channels == 4) ? 3 : 1;
    size_t img_size = (png.width_pixels * png.height_pixels * png.channels);
    size_t new_img_size = (png.width_pixels * png.height_pixels * new_channels);

    //VERIFY
    if (png.channels == 3 || png.channels == 1) { // No alpha channel detected
        printf("   -No Alpha Channel Detected\n");
        return png;
    }
    else if (png.channels != 4 && png.channels != 2) {
        printf("removeAlphaChannel(): channel count unhandled: %d\n", png.channels);
        exit(1);
    }
    else {
        printf("Alpha Channel Detected\n");
        printf("ERROR - Alpha channel handling have not yet been implemented\n");
        exit(1);
    }

    ////ALLOCATE NEW IMAGE MEMORY
    //unsigned char* new_img = malloc(new_img_size);
    //if (new_img == NULL) {
    //    printf("removeAlphaChannel(): Failed to allocate memory");
    //    exit(1);
    //}

    //unsigned char red, green, blue, alpha;
    //unsigned char* p, * np;
    //for (p = png.data, np = new_img; p != png.data + img_size; p += png.channels, np += new_channels) {
    //    red = *p;
    //    green = *(p + 1);
    //    blue = *(p + 2);
    //    alpha = *(p + 3);

    //    np[0] = red;
    //    np[1] = green;
    //    np[2] = blue;
    //}

    PngImage newPng;
    //newPng.data = new_img;
    //newPng.width_pixels = png.width_pixels;
    //newPng.height_pixels = png.height_pixels;
    //newPng.channels = new_channels;

    //printf("   -Alpha Channel Removed\n");
    return newPng;

}
static PngImage PngImage_toGray(PngImage png) {

    printf("    Converting to grayscale...\n");

    //VARIABLES
    unsigned int width = png.width_pixels;
    unsigned int height = png.height_pixels;
    unsigned int channels = png.channels;
    unsigned char* img = png.data;
    int gray_channels = (channels == 4) ? 2 : 1;
    size_t img_size = (width * height * channels * png.bytes_per_channel);
    size_t gray_img_size = width * height * gray_channels * png.bytes_per_channel;


    if (png.channels == 1) {
        printf("   -PNG is already Gray-Scale\n");
        return png;
    }

    //ALLOCATE GRAY IMAGE MEMORY
    unsigned char* gray_img = malloc(gray_img_size);
    if (gray_img == NULL) {
        printf("Unable to allocate memory\n");
        exit(1);
    }

    //p - input image
    //pg - output image
    if (png.bytes_per_channel == 1) {
        unsigned char red, green, blue;
        unsigned char grayValue;
        for (unsigned char* p = img, *pg = gray_img; p != img + img_size; p += channels, pg += gray_channels) {
            red = *p;
            green = *(p + 1);
            blue = *(p + 2);
            grayValue = (unsigned char)((red + green + blue) / 3.0);
            *pg = grayValue;

            //printf("%02x %02x %02x = %02x\n", red, green, blue, grayValue);
            if (channels == 4) {
                *(pg + 1) = *(p + 3);
            }
        }
        PngImage png_gray = png;
        png_gray.data = gray_img;
        png_gray.image_size_bytes = png.image_size_pixels * png.bytes_per_channel;
        png_gray.color_type = "grayscale_noAlpha";
        png_gray.channels = gray_channels;
        printf("   -Converted to Gray-Scale\n");
        return png_gray;
    }
    else if (png.bytes_per_channel == 2) {
        uint16_t red, green, blue;
        uint16_t grayValue;
        for (uint16_t* p = img, *pg = gray_img; p != img + img_size; p += channels, pg += gray_channels) {
            red = *p;
            green = *(p + 1);
            blue = *(p + 2);
            grayValue = (uint16_t)((red + green + blue) / 3.0);
            *pg = grayValue;

            //printf("%02x %02x %02x = %02x\n", red, green, blue, grayValue);
            if (channels == 4) {
                *(pg + 1) = *(p + 3);
            }
        }
        PngImage png_gray = png;
        png_gray.data = gray_img;
        png_gray.image_size_bytes = png.image_size_pixels * png.bytes_per_channel;
        png_gray.color_type = "grayscale_noAlpha";
        png_gray.channels = gray_channels;
        printf("   -Converted to Gray-Scale\n");
        return png_gray;
    }
    else {
        printf("ERROR - unhandled bytes_per_channel: %d\n", png.bytes_per_channel);
        exit(1);
    }

}
static PngImage readImage(PngImage png) {

    printf("READ IMAGE \n");

    //GET FILE
    FILE* png_fp = getFile(png.input);

    //GET CONTEXT
    spng_ctx* ctx = init_context();

    //CONNECT FILE -> CONTEXT
    spng_set_png_file(ctx, png_fp); /* or _buffer(), _stream() */

    //HEADER INFO
    struct spng_ihdr ihdr = getHeader(ctx);
    int fmt = getFormat(ihdr);
    size_t  imageSizeBytes = getImageSize(ctx, fmt);
    unsigned int  image_width_bytes = imageSizeBytes / ihdr.height;

    //DECODE IMAGE SIZE
    decodeImageSize(ctx, fmt, & imageSizeBytes);

    unsigned char* data = malloc(imageSizeBytes);
    if (data == NULL) {
        exit(1);
    }

    //DECODE IMAGE
    decodeImage(ctx, ihdr, fmt, data, image_width_bytes);

    png.data = data;
    png.width_pixels = ihdr.width;
    png.height_pixels = ihdr.height;
    png.image_size_pixels = ihdr.width * ihdr.height;
    png.image_size_bytes = imageSizeBytes;
    png.bytes_per_channel = (ihdr.bit_depth == 8) ? 1 : 2;
    png.color_type = getColorType(ihdr.color_type);
    png.channels = getChannelCount(ihdr.color_type);
    size_t packetSize_pixels = png.packetSize == 0 ? png.width_pixels : png.packetSize;
    png.packetSize = packetSize_pixels;

    printf("   Image read successful\n");
    printHeader(ihdr);

    return png;
}

//TO TXT
static void writeByte(uint8_t x, FILE* output_fp) {
    fprintf(output_fp, "%02x ", x);
}
static void writePixel(FILE* output_fp, unsigned char** byte, unsigned int bytes_per_channel) {
    if (bytes_per_channel == 1) {
        writeByte(0, output_fp);
        writeByte(**byte, output_fp);
        (*byte)++;
    } else if (bytes_per_channel == 2) {
        writeByte(**byte, output_fp);
        (*byte)++;
        writeByte(**byte, output_fp);
        (*byte)++;
    } else {
        printf("ERROR - Unhandled bytes_per_channel count: %d\n", bytes_per_channel);
        exit(1);
    }
}
static void writeSinglePacket(unsigned int* remainingPixels, FILE* output_fp, unsigned char** byte, unsigned int bytes_per_channel, unsigned int packetSize_pixels) {
    
    for (int i = 0; i < packetSize_pixels; i++) {
        writePixel(output_fp, byte, bytes_per_channel);
        (*remainingPixels)--;
    }
    fprintf(output_fp, "\n");
}
static void png_to_txt(PngImage png) {

    //VARIABLES
    unsigned int packetSize_pixels = png.packetSize;
    unsigned int bytes_per_channel = png.bytes_per_channel;
    unsigned int packetSize_bytes = packetSize_pixels * bytes_per_channel;
    unsigned int width = png.width_pixels;
    unsigned int height = png.height_pixels;
    unsigned int remainingPixels = (width * height);
    unsigned char* baseAddress = png.data;
    unsigned char* currentByte = png.data;
    FILE* output_fp;

    fopen_s(&output_fp, png.output, "w");
    if (output_fp == NULL) {
        printf("ERROR - failed to open: %s\n", png.output);
        exit(1);
    }
      
    while (packetSize_pixels <= remainingPixels) {
        writeSinglePacket(&remainingPixels, output_fp, &currentByte, bytes_per_channel, packetSize_pixels);
    }
     
    //LAST PACKET - remaining pixels
    if (remainingPixels > 0) {
        writeSinglePacket(&remainingPixels, output_fp, &currentByte, bytes_per_channel, remainingPixels);
    }

    printf("SUCCESS\n   .png converted to .txt\n");
}

//STDOUT
static void printImage(PngImage png) {
    
    //VARIABLES
    const char* color_name = png.color_type;
    unsigned int offset;
    unsigned int bytes_per_channel = png.bytes_per_channel; //VERIFY
    unsigned int channels = png.channels;
    unsigned int byteWidth = (png.width_pixels) * channels * bytes_per_channel;
    unsigned int pixelSize = channels * bytes_per_channel;
    unsigned char* image = png.data;
#define GRAYSCALE 1
    //PRINT IMAGE
    if (channels == 3 && bytes_per_channel == 1) {
        for (unsigned int row = 0; row < (png.height_pixels); row++) {
            for (unsigned int pix = 0; pix < byteWidth; pix += pixelSize) {
                offset = pix + (row * byteWidth);
                printf("(%02x %02x %02x) ", *(image + offset), *(image + offset + 1), *(image + offset + 2));
            }
            printf("\n");
        }
    } 
    else if (channels == 3 && bytes_per_channel == 2) {
        for (unsigned int row = 0; row < (png.height_pixels); row++) {
            for (unsigned int pix = 0; pix < byteWidth; pix += pixelSize) {
                offset = pix + (row * byteWidth);
                printf("(%02x %02x %02x %02x %02x %02x) ",
                    *(image + offset + 0), *(image + offset + 1),
                    *(image + offset + 2), *(image + offset + 3),
                    *(image + offset + 4), *(image + offset + 5));
            }
            printf("\n");
        }
    }
    else if (channels == GRAYSCALE && bytes_per_channel == 2) {
        for (unsigned int row = 0; row < (png.height_pixels); row++) {
            for (unsigned int pix = 0; pix < byteWidth; pix += pixelSize) {
                offset = pix + (row * byteWidth);
                printf("(%02x %02x) ", *(image + offset + 0), *(image + offset + 1));
            }
            printf("\n");
        }
    }
    else if (channels == GRAYSCALE && bytes_per_channel == 1) {
        for (unsigned int row = 0; row < (png.height_pixels); row++) {
            for (unsigned int col = 0; col < byteWidth; col++) {
                offset = col + (row * byteWidth);
                printf("(%02x) ", *(image + offset));
            }
            printf("\n");
        }
    }
    else {
        printf("ERROR - Cannot print image: channels count: %d. bytes_per_channel: %d\n", channels, bytes_per_channel);
    }
}

//HEADER FUNCTIONS
int png_to_hex(PngImage png) {
    
    //GET IMAGE
    png = readImage(png);
    //printImage(png);

    //REMOVE ALPHA CHANNEL
    png = removeAlphaChannel(png);

    //CONVERT TO GRAYSCALE
    PngImage png_gray = PngImage_toGray(png);
    //printImage(png_gray);
     
    //WRITE
    png_to_txt(png_gray);



    return 0;
}

//spng_ctx_free(ctx);
//free(image);


/*zlib
* spng.c has a dependency on zlib.
* The source code won't compile on another computer because it can't find zlib
* I want to include zlib in my project
* It's not currently in my project, rather it's in some directly that's dynamically linked.
*   zlib.dll is produced as a result. 
* 
* zlib copies
* 1. C:\msys64\home\devon\zlib-1.2.11   size: 2.84 MB
* 2. C:\msys64\mingw64\include
* 3. C:\dev\libs\c\zlib                 size: 35.7 MB
* 
* Building generates C:\Users\devon\VisualStudio\Png_Image\x64\Release\zlib1.dll


delete #1: compiled just fine 
delete #3: compiled just fine
delete #2: compiled just fine
*/




