#include <stdlib.h>
#include <time.h>
#include "rdh.h"
#include "png.h"

/*
Generate key png file that stream encrypt process needed.
width, height: The width and height of the image to be encrypted.
The key png file is saved to current directory.
 */
void generate_key_image(const unsigned width, const unsigned height) {
	unsigned char *image = (unsigned char *)malloc(width*height);

	time_t t;
	srand((unsigned) time(&t));
	
	for (int i = 0; i < width*height; i++) {
		image[i] = rand() * 255;
	}
	encode_and_save("key.png", image, width, height);

}

/*
Load key from file. 
filename: key file name.
image: buffer that saved key file.
width, height: get width and height of the key.
 */
void get_key_image(const char *filename, unsigned char **image,
		unsigned *width, unsigned *height) {

	unsigned char *png;
	size_t pngsize;

	load_png(filename, &png, &pngsize);
	decode_png(image, width, height, png, pngsize);
}

/*
stream encryption. 
 */
void stream_encrypt(const unsigned char *key, unsigned char *image, const long bufsize) {
	for (long i = 0; i < bufsize; i++) {
		image[i] ^= key[i];
	}
}

void watermark(unsigned char *image, unsigned L) {

}
