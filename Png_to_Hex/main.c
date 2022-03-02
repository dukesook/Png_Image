/*update 4.10*/
#include <stdio.h>
#include "PngImage.h"
#include "CommandLineHandler.h"

//INPUTS
#define INPUT_4x3_rows "input/4x3_rows.png"
#define INPUT_10W_5H_BLACK "input/10w_5h_black.png"
#define INPUT_10W_5H_GRADIENT "input/10w_5h_gradient.png"
#define INPUT_8W_6H_RGB "input/8w_6h_rgb.png"
#define INPUT_8W_6H_COLUMNS "input/8w_6h_columns.png"
#define INPUT_MR_CAT "input/MrCat.png"
#define INPUT_BRIDGE "input/Bridge_2560x2160.png"

//OUTPUTS
#define OUTPUT_4x3_rows "out/4x3_rows.txt"
#define OUTPUT_10W_5H_BLACK "out/10w_5h_black.txt"
#define OUTPUT_10W_5H_GRADIENT "out/10w_5h_gradient.txt"
#define OUTPUT_8w_6h_RGB "out/8w_6h_rgb.txt"
#define OUTPUT_8W_6H_COLUMNS "out/8w_6h_columns.txt"
#define OUTPUT_MR_CAT "out/MrCat.txt"
#define OUTPUT_BRIDGE "out/Bridge_2560x2160.txt"

#define ALL_FLAGS "set:"
#define INPUT_INDEX 1
#define OUTPUT_INDEX 2
//PROTOTYPES
static void demo_pngToTxt();
static void demo_dynamicPackages(int packetSize);
static void printStrings(int argc, char *argv[]);
static void printDocumentation();


int main(int argc, char *argv[]) {
    printf("Converting PNG...\n");

    PngImage_Arguments arguments;
    arguments = CommandLineHandler_handler(argc, argv);
    PngImage_Hex_txt(arguments);

    printf("END OF PROGRAM\n\n");
    return 0;
}





//HELPER FUNCTIONS

static void printStrings(int argc, char *argv[]) {
    for (int i = 0; i < argc; i++) {
        printf("%d. %s\n", i, argv[i]);
    }
}
