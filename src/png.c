#include "png.h"
#include "lodepng.h"
#include <stdlib.h>
#include <stdio.h>

#define BIT_DEPTH 8

const LodePNGColorType LCT = LCT_GREY;

void load_png(const char* filename, unsigned char** png, size_t* pngsize) {
	unsigned error;

	error = lodepng_load_file(png, pngsize, filename);
	if (error) {
		fprintf(stderr, "ERROR: %u: %s\n", error, lodepng_error_text(error));
		exit(1);
	} 

}

void decode_png(unsigned char** image, unsigned *width, unsigned *height,
		unsigned char* png, unsigned pngsize) {

	unsigned error;
	error = lodepng_decode_memory(image, width, height, png, pngsize, LCT, BIT_DEPTH);
	if (error) {
		fprintf(stderr, "ERROR: %u: %s\n", error, lodepng_error_text(error));
		exit(1);
	}
}

void encode_and_save(const char *filename, const unsigned char *image,
		unsigned width, unsigned height) {

	unsigned error = lodepng_encode_file(filename, image, width, height, LCT, BIT_DEPTH);
	if (error) {
		fprintf(stderr, "ERROR: %u: %s\n", error,
				lodepng_error_text(error));
		exit(1);
	}
}
