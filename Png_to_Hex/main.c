
#include <stdio.h>
#include "png_to_hex.h"
#include "commandLineHandler.h"

int main(int argc, char** argv) {
	printf("test\n");

	PngImage_Arguments arguments;
	arguments = CommandLineHandler_getArgs(argc, argv);
	pnt_to_hex(arguments);


	return 0;
}