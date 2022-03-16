
Png_to_Hex.exe 
	Takes the path of a png image, converts the image to grayscale,
	and outputs the contents as a .txt file.

-s FLAG
	Specifies the number of pixels in each packet.
	Expects a decimal number.
	Packets are separated by \n
	Without the -s flag, the packet size defaults to the image width.

-i FLAG

EXAMPLE
	$ Png_to_Hex image.png
	$ Png_to_Hex -s 16 image.png	//Set packet size to be 16

TEST CASES
	You can double click on TEST_RUN.bat to run Png_to_Hex on the sample images
	You can run TEST_CLEAR.bat to delete the "output" folder from previous runs.

CONSTRAINTS
	The image must be a .png file. JPEG and other file extensions are not yet supported
	Images with Alpha Channels are not yet supported. 
	

