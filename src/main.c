#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>

#include "lodepng.h"

#define PNG_SIG_CAP 8
const uint8_t PNG_SIG[PNG_SIG_CAP] = {137, 80, 78, 71, 13, 10, 26, 10};

#define BIT_DEPTH 8

const LodePNGColorType LCT = LCT_GREY;

void reverse_bytes(void *buf_void, size_t buf_cap) {
	uint8_t *buf = buf_void;
	for (size_t i = 0; i < buf_cap / 2; i++) {
		uint8_t t = buf[buf_cap - i - 1];
		buf[buf_cap - i - 1] = buf[i];
		buf[i] = t;
	}
}

void print_bytes(const char *image, unsigned width, unsigned height) {
	for (unsigned i = 0; i < height; i++) {
		for (unsigned j = 0; j < width; j++) {
			printf("%u ", image[i*width+j]);
		}
		puts("");
	}
}

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

int main(int argc, char **argv) {
	assert(*argv != NULL);

	char *program = *argv++;

	if (*argv == NULL) {
		fprintf(stderr, "Usage: %s <input.png>\n", program);
		fprintf(stderr, "ERROR: no input file is provided\n");
		exit(1);
	}

	char *input_filepath = *argv++;
	printf("Inspected file is %s\n", input_filepath);

	unsigned char* raw_png = 0;
	size_t pngsize;
	load_png(input_filepath, &raw_png, &pngsize);

	unsigned char* image = 0;
	unsigned width, height;
	decode_png(&image, &width, &height, raw_png, pngsize);

	memset(image, 255, width*height);

	encode_and_save("output.png", image, width, height);

	free(raw_png);
	free(image);

	return 0;
}
