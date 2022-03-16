cls
mkdir output

::TO HEX
Png_to_Hex 8_bit_images\8x6_columns.png            output\8b_8x6_columns.txt
Png_to_Hex 8_bit_images\8x6_rgb.png                output\8b_8x6_rgb.txt
Png_to_Hex 8_bit_images\10x5_black.png             output\8b_10x5_black.txt
Png_to_Hex 8_bit_images\10x5_gradient.png          output\8b_10x5_gradient.txt
Png_to_Hex 8_bit_images\MrCat.png                  output\8b_MrCat.txt
Png_to_Hex 16_bit_images\16b_5x3_blue.png          output\16b_5x3_blue.txt
Png_to_Hex 16_bit_images\16b_5x4_rows.png          output\16b_5x4_rows.txt
Png_to_Hex 16_bit_images\16b_16x2_gray_counter.png output\16b_16x2_gray_counter.txt
Png_to_Hex 16_bit_images\16b_16x2_rgb_counter.png  output\16b_16x2_rgb_counter.txt
Png_to_Hex 16_bit_images\16b_bird.png              output\16b_bird.txt
Png_to_Hex 16_bit_images\16b_gradient.png          output\16b_gradient.txt
Png_to_Hex 16_bit_images\16b_grayscale.png         output\16b_grayscale.txt

::VERIFY
fc key\8b_8x6_columns.txt        output\8b_8x6_columns.txt 			>  output\results.txt
fc key\8b_8x6_rgb.txt            output\8b_8x6_rgb.txt				>> output\results.txt
fc key\8b_10x5_black.txt         output\8b_10x5_black.txt			>> output\results.txt
fc key\8b_10x5_gradient.txt      output\8b_10x5_gradient.txt		>> output\results.txt
fc key\16b_5x3_blue.txt          output\16b_5x3_blue.txt			>> output\results.txt
fc key\16b_5x4_rows.txt          output\16b_5x4_rows.txt			>> output\results.txt
fc key\16b_16x2_gray_counter.txt output\16b_16x2_gray_counter.txt	>> output\results.txt
fc key\16b_16x2_rgb_counter.txt  output\16b_16x2_rgb_counter.txt	>> output\results.txt
fc key\16b_bird.txt              output\16b_bird.txt				>> output\results.txt
fc key\16b_gradient.txt          output\16b_gradient.txt			>> output\results.txt
fc key\16b_grayscale.txt         output\16b_grayscale.txt			>> output\results.txt

:: fc key\MrCat.txt               output\8b_MrCat.txt
:: MrCat.png has an alpha channel which is not yet handled



::-s FLAG
Png_to_Hex  -s 1   8_bit_images\8x6_rgb.png  output\8b_sFlag1.txt
Png_to_Hex  -s 5   8_bit_images\8x6_rgb.png  output\8b_sFlag5.txt
Png_to_Hex  -s 17  8_bit_images\8x6_rgb.png  output\8b_sFlag17.txt
Png_to_Hex  -s 60  8_bit_images\8x6_rgb.png  output\8b_sFlag60.txt

Png_to_Hex  -s 1   16_bit_images\16b_16x2_rgb_counter.png  output\16b_sFlag1.txt
Png_to_Hex  -s 5   16_bit_images\16b_16x2_rgb_counter.png  output\16b_sFlag5.txt
Png_to_Hex  -s 17  16_bit_images\16b_16x2_rgb_counter.png  output\16b_sFlag17.txt
Png_to_Hex  -s 60  16_bit_images\16b_16x2_rgb_counter.png  output\16b_sFlag60.txt

::VERIFY -s FLAG
fc output\8b_sFlag1.txt   key\8b_sFlag1.txt		>> output\results.txt
fc output\8b_sFlag5.txt   key\8b_sFlag5.txt		>> output\results.txt
fc output\8b_sFlag17.txt  key\8b_sFlag17.txt	>> output\results.txt
fc output\8b_sFlag60.txt  key\8b_sFlag60.txt	>> output\results.txt

fc output\16b_sFlag1.txt  key\16b_sFlag1.txt	>> output\results.txt
fc output\16b_sFlag5.txt  key\16b_sFlag5.txt	>> output\results.txt
fc output\16b_sFlag17.txt key\16b_sFlag17.txt	>> output\results.txt
fc output\16b_sFlag60.txt key\16b_sFlag60.txt	>> output\results.txt




::To ICD
Png_to_Hex -s 0   -i   16_bit_images\16b_16x2_rgb_counter.png  output\16b_sFlag0_iFlag.txt
Png_to_Hex -s 1   -i   16_bit_images\16b_16x2_rgb_counter.png  output\16b_sFlag1_iFlag.txt
Png_to_Hex -s 3   -i   16_bit_images\16b_16x2_rgb_counter.png  output\16b_sFlag3_iFlag.txt
Png_to_Hex -s 40  -i   16_bit_images\16b_16x2_rgb_counter.png  output\16b_sFlag40_iFlag.txt

Png_to_Hex -s 0   -i   8_bit_images\8x6_columns.png  output\8b_sFlag0_iFlag.txt
Png_to_Hex -s 1   -i   8_bit_images\8x6_columns.png  output\8b_sFlag1_iFlag.txt
Png_to_Hex -s 3   -i   8_bit_images\8x6_columns.png  output\8b_sFlag3_iFlag.txt
Png_to_Hex -s 40  -i   8_bit_images\8x6_columns.png  output\8b_sFlag40_iFlag.txt

::VERIFY
fc key\16b_sFlag0_iFlag.txt  output\16b_sFlag0_iFlag.txt   	 >> output\results.txt
fc key\16b_sFlag1_iFlag.txt  output\16b_sFlag1_iFlag.txt	 >> output\results.txt
fc key\16b_sFlag3_iFlag.txt  output\16b_sFlag3_iFlag.txt	 >> output\results.txt
fc key\16b_sFlag40_iFlag.txt output\16b_sFlag40_iFlag.txt	 >> output\results.txt
															 
fc key\8b_sFlag0_iFlag.txt   output\8b_sFlag0_iFlag.txt		 >> output\results.txt
fc key\8b_sFlag1_iFlag.txt   output\8b_sFlag1_iFlag.txt		 >> output\results.txt
fc key\8b_sFlag3_iFlag.txt   output\8b_sFlag3_iFlag.txt		 >> output\results.txt
fc key\8b_sFlag40_iFlag.txt  output\8b_sFlag40_iFlag.txt	 >> output\results.txt

 
::LITTLE ENDIAN
Png_to_Hex  -e  8_bit_images\8x6_columns.png                output\8b_8x6_columns_eFlag.txt
Png_to_Hex  -e  -i  8_bit_images\8x6_columns.png             output\8b_8x6_columns_eFlag_iFlag.txt
Png_to_Hex  -e  16_bit_images\16b_16x2_rgb_counter.png      output\16b_16x2_rgb_counter_eFlag.txt
Png_to_Hex  -e  -i  16_bit_images\16b_16x2_rgb_counter.png   output\16b_16x2_rgb_counter_eFlag_iFlag.txt
Png_to_Hex  -e  -i  16_bit_images\16b_5x3_blue.png			  output\16b_5x3_blue_eFlag_iFlag.txt

fc key\8b_8x6_columns_eFlag.txt				output\8b_8x6_columns_eFlag.txt				>> output\results.txt
fc key\16b_16x2_rgb_counter_eFlag.txt		output\16b_16x2_rgb_counter_eFlag.txt		>> output\results.txt
fc key\8b_8x6_columns_eFlag_iFlag.txt		output\8b_8x6_columns_eFlag_iFlag.txt		>> output\results.txt
fc key\16b_16x2_rgb_counter_eFlag_iFlag.txt	output\16b_16x2_rgb_counter_eFlag_iFlag.txt	>> output\results.txt
fc key\16b_5x3_blue_eFlag_iFlag.txt			output\16b_5x3_blue_eFlag_iFlag.txt	>> output\results.txt



pause