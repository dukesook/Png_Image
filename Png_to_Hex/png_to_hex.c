//Documentation: https://libspng.org/
//source code: https://github.com/randy408/libspng/blob/v0.7.2/examples/example.c

#include "spng.h"
#include "png_to_hex.h"
//#include <inttypes.h>
#include <stdio.h>

static unsigned int getChannelCount(uint8_t color_type) {
    switch (color_type) {
        case SPNG_COLOR_TYPE_GRAYSCALE: return 1;
        case SPNG_COLOR_TYPE_TRUECOLOR: return 3;
        case SPNG_COLOR_TYPE_INDEXED: 
            printf("Unhandled image: indexed image\n");
            exit(1);
        case SPNG_COLOR_TYPE_GRAYSCALE_ALPHA: return 2;
        case SPNG_COLOR_TYPE_TRUECOLOR_ALPHA: return 4;
        default: return "(invalid)";
    }
}
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
static const char* color_type_str(enum spng_color_type color_type) {
    switch (color_type) {
        case SPNG_COLOR_TYPE_GRAYSCALE: return "grayscale";
        case SPNG_COLOR_TYPE_TRUECOLOR: return "truecolor";
        case SPNG_COLOR_TYPE_INDEXED: return "indexed color";
        case SPNG_COLOR_TYPE_GRAYSCALE_ALPHA: return "grayscale with alpha";
        case SPNG_COLOR_TYPE_TRUECOLOR_ALPHA: return "truecolor with alpha";
        default: return "(invalid)";
    }
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
static FILE* getFile(char* inputPath) {
    FILE* png;
    #pragma warning(suppress : 4996) //should use fopen_s() instead
    png = fopen(inputPath, "rb");

    if (png == NULL) {
        printf("error opening input file %s\n", inputPath);
        exit(1);
    }
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
static size_t getImageSize(spng_ctx* ctx, int fmt) {
    size_t image_size;
    int ret = spng_decoded_image_size(ctx, fmt, &image_size);

    if (ret) {
        printf("ERROR - could not get image size\n");
        exit(1);
    }

    return image_size;

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

    //ALLOCATE NEW IMAGE MEMORY
    unsigned char* new_img = malloc(new_img_size);
    if (new_img == NULL) {
        printf("removeAlphaChannel(): Failed to allocate memory");
        exit(1);
    }

    unsigned char red, green, blue, alpha;
    unsigned char* p, * np;
    for (p = png.data, np = new_img; p != png.data + img_size; p += png.channels, np += new_channels) {
        red = *p;
        green = *(p + 1);
        blue = *(p + 2);
        alpha = *(p + 3);

        np[0] = red;
        np[1] = green;
        np[2] = blue;
    }

    PngImage newPng;
    newPng.data = new_img;
    newPng.width_pixels = png.width_pixels;
    newPng.height_pixels = png.height_pixels;
    newPng.channels = new_channels;

    printf("   -Alpha Channel Removed\n");
    return newPng;

}
static PngImage PngImage_toGray(PngImage png) {

    //VARIABLES
    unsigned int width = png.width_pixels;
    unsigned int height = png.height_pixels;
    unsigned int channels = png.channels;
    unsigned char* img = png.data;
    int gray_channels = (channels == 4) ? 2 : 1;
    size_t img_size = (width * height * channels);
    size_t gray_img_size = width * height * gray_channels;


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
    unsigned char red, green, blue;
    for (unsigned char* p = img, *pg = gray_img; p != img + img_size; p += channels, pg += gray_channels) {
        red = *p;
        green = *(p + 1);
        blue = *(p + 2);
        *pg = (uint8_t)((red + green + blue) / 3.0);
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
static PngImage readImage(PngImage png) {

    //GET FILE
    FILE* png_fp = getFile(png.input);

    //GET CONTEXT
    spng_ctx* ctx = init_context();

    //CONNECT FILE -> CONTEXT
    spng_set_png_file(ctx, png_fp); /* or _buffer(), _stream() */

    //HEADER INFO
    struct spng_ihdr ihdr = getHeader(ctx);
    //printHeaderInfo(ihdr);
    int fmt = getFormat(ihdr);
    size_t image_size_bytes = getImageSize(ctx, fmt);
    size_t image_width_bytes = image_size_bytes / ihdr.height;

    //DECODE IMAGE SIZE
    decodeImageSize(ctx, fmt, &image_size_bytes);

    unsigned char* data = malloc(image_size_bytes);
    if (data == NULL) {
        exit(1);
    }

    //DECODE IMAGE
    decodeImage(ctx, ihdr, fmt, data, image_width_bytes);

    png.data = data;
    png.width_pixels = ihdr.width;
    png.height_pixels = ihdr.height;
    png.image_size_pixels = ihdr.width * ihdr.height;
    png.image_size_bytes = image_size_bytes; //VERIFY
    png.bytes_per_channel = ihdr.bit_depth; //VERIFY
    png.color_type = ihdr.color_type;
    png.channels = getChannelCount(png.color_type);

    return png;
}

//OUTPUT
static void writeData(unsigned int x, FILE* output) {
    fprintf(output, "%02x ", x);
}
static void nextPixel(int* x, int* y, int width) {
    int lastX = (width - 1);
    if (*x == lastX) {
        *x = 0;  
        (*y)++; //Next row
    }
    else {
        (*x)++; //next pixel
    }
}

static void png_to_txt(PngImage png) {

    //VARIABLES
    size_t packetSize = png.packetSize == 0? png.width_pixels : png.packetSize;
    FILE* output;
    unsigned int width = png.width_pixels;
    unsigned int height = png.height_pixels;
    unsigned int x = 0;
    unsigned int y = 0;
    unsigned int remainingPixels = (width * height);
    unsigned char* p = png.data;

    fopen_s(&output, png.output, "w");
    if (output == NULL) {
        printf("ERROR - failed to open: %s\n", png.output);
        exit(1);
    }
      
    while (packetSize < remainingPixels) {
        //FOR EACH PACKET
        for (int i = 0; i < packetSize; i++) {
            writeData(*p, output);
            p++;
            remainingPixels--;
            nextPixel(&x, &y, width);
        }
        fprintf(output, "\n");
    }
     
    //LAST PACKET - remaining pixels
    for (int i = 0; i < remainingPixels; i++) {
        writeData(*p, output);
        p++;
    }
    fprintf(output, "\n");
}
static void printHeaderInfo(struct spng_ihdr ihdr) {
    const char* color_name = color_type_str(ihdr.color_type);

    printf("META DATA\n");
    printf("   Width: %u\n"
        "   Height: %u\n"
        "   Bit Depth: %u\n"
        "   Color Type: %u - %s\n",
        ihdr.width, ihdr.height, ihdr.bit_depth, ihdr.color_type, color_name);

    //printf("compression method: %u\n"
    //    "filter method: %u\n"
    //    "interlace method: %u\n",
    //    ihdr.compression_method, ihdr.filter_method, ihdr.interlace_method);

}
static void printImage(PngImage png) {
    
    //VARIABLES
    const char* color_name = png.color_type;
    int offset;
    int bytes_per_channel = png.bytes_per_channel; //VERIFY
    int channels = (color_name == "grayscale" ? 1 : 3);
    int byteWidth = (png.width_pixels) * channels * bytes_per_channel;
    int pixelSize = channels * bytes_per_channel;
    unsigned char* image = png.data;
#define GRAYSCALE 1
    //PRINT IMAGE
    if (channels == 3 && bytes_per_channel == 1) {
        for (int row = 0; row < (png.height_pixels); row++) {
            for (int pix = 0; pix < byteWidth; pix += pixelSize) {
                offset = pix + (row * byteWidth);
                printf("(%02x %02x %02x) ", *(image + offset), *(image + offset + 1), *(image + offset + 2));
            }
            printf("\n");
        }
    }
    else if (channels == 3 && bytes_per_channel == 2) {
        for (int row = 0; row < (png.height_pixels); row++) {
            for (int pix = 0; pix < byteWidth; pix += pixelSize) {
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
        for (int row = 0; row < (png.height_pixels); row++) {
            for (int pix = 0; pix < byteWidth; pix += pixelSize) {
                offset = pix + (row * byteWidth);
                printf("(%02x %02x) ", *(image + offset + 0), *(image + offset + 1));
            }
            printf("\n");
        }
    }
    else {
        for (int row = 0; row < (png.height_pixels); row++) {
            for (int col = 0; col < byteWidth; col++) {
                offset = col + (row * byteWidth);
                printf("%02x ", *(image + offset));
            }
            printf("\n");
        }
    }
}

//HEADER FUNCTIONS
int pnt_to_hex(PngImage png) {
    
    //GET IMAGE
    png = readImage(png);

    //PRINT IMAGE
    printImage(png);

    //REMOVE ALPHA CHANNEL
    png = removeAlphaChannel(png);

    //CONVERT TO GRAYSCALE
    png = PngImage_toGray(png);

    //PRINT IMAGE
    printImage(png);

    //WRITE
    png_to_txt(png);



    return 0;
}

//spng_ctx_free(ctx);
//free(image);