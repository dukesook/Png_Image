
#include "CommandLineHandler.h"
//#include <unistd.h> //getopt, optarg, optind
#include "getopt.h" //Widows version of getopt
#include <ctype.h> //isprint

#define INPUT_4x3_rows "input/4x3_rows.png"
#define OUTPUT_4x3_rows "out/4x3_rows.txt"

//LEAF FUNCTIONS - Do not call another function. Lowest Levels of logic
static bool endsWith(char* word, char* desiredEnd) {
    int wordLength = strlen(word);
    int desiredEndLength = strlen(desiredEnd);
    char* wordEnd = word + (wordLength - desiredEndLength); //move pointer from the beginning of the word to desired location

    if (wordLength <= desiredEndLength) {
        return false;
    } else if (!strcmp(wordEnd, desiredEnd)) { //strcmp returns 0 if the two words are the same
        return true;
    } else {
        return false;
    }
}
static void printDocumentation() {
    printf("pngToIcd.exe\n");
    printf("ARGUMENTS\n");
    printf("\targ1: input path:  e.g. input/MrCat.png\n");
    printf("\targ2: output path: e.g. out/MrCat.txt (optional)\n");

    printf("FLAGS\n");
    printf("\t-s 16: Set Packet Size to 16\n");
    printf("\t-e:    Little Endian\n");
    printf("\t-t:    Left Shift Data by 1 byte. Useful if the input image data is 1 byte and 2 byte data is desired\n");

    printf("DEFAULT VALUES:\n");
    printf("\tPacket Size: Image width\n");
    printf("\tEndianness: Big Endian\n");

    printf("EXAMPLES:\n");
    printf("\tSimple............pngToIcd.exe  input/MrCat.png\n");
    printf("\tSpecify output....pngToIcd.exe  input/MrCat.png  output/MrCat.txt\n");
    printf("\tSet Packet Size...pngToIcd.exe  -s  128 input/MrCat.png  output/MrCat.txt\n");
    printf("\tLittle Endian.....pngToIcd.exe  -e  input/MrCat.png  output/MrCat.txt\n");

    printf("CONSTRAINTS:\n");
    printf("\t-The input image MUST be a .png file.\n");
    printf("\t-Flags must go before all arguments\n");
    printf("\t-Output file must be a .txt file\n");
    printf("\t-Both grayscale and RBG images are handled\n");
    printf("\t-Images with & without transparent aplpha channels are handled\n");


}
static char* generateOutputPath(char* input) {
    //ASSUMPTION - input is the path to a .png file

    //VERIFY LENGTH
    int inputLength = (int) strlen(input);
    if (inputLength < 5) {
        printf("ERROR - Input Path: %s\n", input);
        printf(".....The filename is too short\n");
        printDocumentation();
        exit(1);
    }

    char* output = malloc(inputLength + 1);
    strcpy(output, input);

    int pdex = inputLength - 3;
    output[pdex + 0] = 't';
    output[pdex + 1] = 'x';
    output[pdex + 2] = 't';

    return output;

}


//HELPER FUNCTIONS
static void printPngArgs(PngImage_Arguments args) {

    if (args.input == NULL) {
        printf("input is NULL\n");
    } else if (args.output == NULL) {
        printf("output is NULL\n");
    }

    printf("SETTINGS\n");
    printf("   input............%s\n", args.input);
    printf("   output:..........%s\n", args.output);
    if (args.packetSize == 0) {
        printf("   Packet Size:.....Not specified, default to image width\n");
    } else {
        printf("   Packet Size:.....%d\n", args.packetSize);
    }
    printf("   littleEndian:....%d\n", args.littleEndian);
    printf("   leftShiftData:...%d\n", args.leftShiftData);
}
static PngImage_Arguments initArgs() {
    PngImage_Arguments args;
    args.input = NULL;
    args.output = NULL;
    args.packetSize = 0;
    args.littleEndian = false;
    args.leftShiftData = false;
    return args;
}
static PngImage_Arguments extractArgs(int argc, char* argv[]) {
    //VARIABLES
    PngImage_Arguments args = initArgs();

    int c;
    //SET OPTIONS
    while ((c = getopt (argc, argv, "ets:")) != -1) {
        switch (c) {
            case 'e':
                args.littleEndian = true;
                break;
            case 't':
                args.leftShiftData = true;
                break;
            case 's':
                args.packetSize = atoi(optarg);
                break;
            case '?':
                if (optopt == 's') {
                    fprintf(stderr, "Option -%s requires an argument.\n", optopt);
                } else if (isprint(optopt)) {
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                } else {
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                }
                exit(1);
            default:
                abort();
        }
    }

    //ARGUMENTS
    int remainingArgs = argc - optind;
    for (int i = 0; i < remainingArgs; i++, optind++) {
//        printf("%d. argv[%d] = %s\n", i, optind, argv[optind]);
        if (i == 0)
            args.input = argv[optind];
        else if (i == 1)
            args.output = argv[optind];
        else if (i <= 2) {
            printf("ERROR - extra argument found index: %s\n", argv[optind]);
            char c = argv[optind][0];
            if (c == '-') {
                printf("   For now, Flags must go before all arguments. \n   Sorry :(\n");
            }
            exit(1);
        }
    }
    return args;
}
static void validateArgs(PngImage_Arguments* args) {

    //INPUT - must be a png
    if (args->input == NULL) {
        printf("ERROR - Input Path not specified\n");
        printDocumentation();
        exit(1);
    }
    if (!endsWith(args->input, ".png")) {
        printf("ERROR - Input Path: %s\n", args->input);
        printf(".....The Input Path must have the .png extension\n");
        printDocumentation();
        exit(1);
    }

    //OUTPUT
    if (args->output == NULL) { //predict output if not specified
        args->output = generateOutputPath(args->input);
    } else if (!endsWith(args->output, ".txt")) {
        printf("ERROR - Output Path: %s\n", args->output);
        printf(".....The Output Path must have the .txt extension\n");
        printDocumentation();
        exit(1);
    }

    //PACKET SIZE
    if (args->packetSize < 0) {
        printf("ERROR - Packet Size: %s\n", args->output);
        printf(".....The Packet Size must be a positive\n");
        printDocumentation();
        exit(1);
    }

}


//HEADER FUNCTION
PngImage_Arguments CommandLineHandler_handler(int argc, char* argv[]) {

    //EXTRACT
    PngImage_Arguments args = extractArgs(argc, argv);

    //VALIDATE
    validateArgs(&args);

    //PRINT - optional
    printPngArgs(args);

    return args;
}



void learn_getopt() {
    printf("VARIABLES\n");
    printf("opterr: 0 = no error\n");
    printf("optopt: unknown options & options missing args are stored here\n");
    printf("optind: next index of argv \n");
    printf("optarg: Holds the option's arguments (if it exists)\n");
    printf("\n");
    printf("getopt(argc, argv, options\n");
    printf(".....returns -1 when there are no more options\n");
    printf(".....returns ? for unrecognized options\n");
    printf("OPTIONS\n");
    printf(".....List all valid options\n");
    printf(".....Requires Arguments - followed by ':'\n");
    printf(".....Argument is Optional  - followed by '::'\n");
    printf(".....first non-option stops option processing\n");
}


