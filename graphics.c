/**
* This code aims to generate a simple histogram's graphic
* in JPEG format
*/

#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <jerror.h>

typedef struct {
	unsigned long int width, height;
	unsigned char *pixel_data;
} image_data;

int store_jpeg_img(image_data *img_data, char *filename) {
	struct jpeg_compress_struct info;
	struct jpeg_error_mgr err;

	unsigned char *row_buffer;

	FILE* f_handle;

	f_handle = fopen(filename, "wb");

	if (f_handle == NULL) printf("Error: open output file");

	info.err = jpeg_std_error(&err);
	jpeg_create_compress(&info);

	jpeg_stdio_dest(&info, f_handle);

	info.image_width = img_data->width;
	info.image_height = img_data->height;
	info.input_components = 1;
	info.in_color_space = JCS_GRAYSCALE;

	jpeg_set_defaults(&info);
	jpeg_set_quality(&info, 100, TRUE);

	jpeg_start_compress(&info, TRUE);

	while (info.next_scanline < info.image_height) {
		row_buffer = &(img_data->pixel_data[info.next_scanline * img_data->width]);
		jpeg_write_scanlines(&info, &row_buffer, 1);
	}

	jpeg_finish_compress(&info);
	fclose(f_handle);

	jpeg_destroy_compress(&info);
	return 0;
}

void create_gradient(unsigned char *pixels) {
	int level_pixels = 4;
	int width = 1030;
	int gray_level = 0;

	for (int col = 0 + 3; col < (1024 + 3); col+=level_pixels, gray_level++) {
		for (int row = 360; row < 367; row++) {
			for (int i = 0; i < level_pixels; i++) {
				pixels[row*width + (col+i)] = gray_level;
			}
		}
	}
}

void create_columns(unsigned char *pixels, unsigned int *normalized_histogram) {
	int level_pixels = 4;
	int width = 1030;

	for (int col = 0 + 3; col < (1024 + 3); col+=level_pixels) {
		printf("%d ", col/level_pixels);
		printf("nh[%d] = %d  ", col/level_pixels, normalized_histogram[col/level_pixels]);
		for (int row = 359; row > (359 - normalized_histogram[col/level_pixels]); row--) {
			for (int i = 1; i < level_pixels - 1; i++) {
				pixels[row*width + (col+i)] = 0b10000000;
			}
		}
	}
	
	printf("\n");
}

void normalize_histogram(unsigned int *histogram, unsigned int *normalized_histogram) {
	unsigned int max_histogram_value = 0;
	int div_factor;
	unsigned int height_greatest_column = 320;

	for (int i = 0; i < 256 ; i++) {
		if (histogram[i] > max_histogram_value)
			max_histogram_value = histogram[i];
	}

	printf("mh = %d\n", max_histogram_value);
	div_factor = max_histogram_value / height_greatest_column;
	printf("dv = %d\n", div_factor);

	for (int i = 0; i < 256; i++) {
		normalized_histogram[i] = (unsigned int)(histogram[i]/div_factor);
	}
}

void create_graphic(unsigned int *histogram, char *filename) {
	image_data graphic;
	graphic.height = 400;
	graphic.width = 1030;
	unsigned char pixels[graphic.height * graphic.width];
	unsigned int normalized_histogram[256];

	for (int i = 0; i < graphic.height * graphic.width; i++)
		pixels[i] = 0b11111111;

	create_gradient(pixels);
	normalize_histogram(histogram, normalized_histogram);
	create_columns(pixels, normalized_histogram);
	graphic.pixel_data = pixels;
	store_jpeg_img(&graphic, filename);
}
