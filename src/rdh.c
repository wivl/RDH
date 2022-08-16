#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "rdh.h"
#include "png.h"

/*
   Generate key png file that stream encrypt process needed.
   width, height: The width and height of the image to be encrypted.
   The key png file is saved to current directory.
   */
void generate_key_image(const unsigned width, const unsigned height) { unsigned char *image = (unsigned char *)malloc(width*height);

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


void watermark(unsigned char *image, unsigned width, unsigned height,
		char *filename) {

	assert(width == height && width % L == 0 && height % L == 0);

	FILE *message = fopen(filename, "rb");

	unsigned char message_byte;
	int n = fread(&message_byte, 1, 1, message);
	if (n != 1) {
		fprintf(stderr, "ERROR: the file %s contains no message\n", filename);
		exit(1);
	}

	unsigned char byte = 0x80;
	int ptr = 0;
	int bitcount = 0;
	bool quit = false;
	bool empty = true;
	int starti, startj, endi, endj;
	int chunk_count = 0;

	for (int h = 0; h < height/L; h++) {				
		for (int w = 0; w < width/L; w++) {				// devide the buffer into L x L chunks

			int mode;
			switch (chunk_count) {
				case 0:
					byte = 0x80;
					break;
				case 1:
					byte = 0x20;
					break;
				case 2:
					byte = 0x08;
					break;
				case 3:
					byte = 0x02;
					break;
				default:
					assert(0 && "unreachable");
			}
			

			mode = (message_byte & byte) == byte ? 1 : 0;
			byte >>= 1;
			mode <<= 1;
			mode += (message_byte & byte) == byte ? 1 : 0;
			printf("mode: %d\n", mode);

			switch (mode) {
				case 0:
					starti = h*L, endi = h*L+L/2;
					startj = w*L, endj = w*L+L/2;
					break;
				case 1:
					starti = h*L, endi = h*L+L/2;
					startj = w*L+L/2, endj = (w+1)*L;
					break;
				case 2:
					starti = h*L+L/2, endi = (h+1)*L;
					startj = w*L, endj = w*L+L/2;
					break;
				case 3:
					starti = h*L+L/2, endi = (h+1)*L;
					startj = w*L+L/2, endj = (w+1)*L;
					break;
				default:
					assert(0 && "unreachable");
			}
			printf("start: (%d, %d), end: (%d, %d)\n", starti, startj, endi, endj);
			unsigned char mask = 0x18;
			for (int i = starti; i < endi; i++) {		// for every chunk, height
				for (int j = startj; j < endj; j++) {	// for every chunk, width
					image[i*(width)+j] ^= mask;
				}
			}

			if (++chunk_count == 4) {
				n = fread(&message_byte, 1, 1, message);
				if (n != 1) {
					goto OUT;
				}
				chunk_count = 0;

			}


		}
	}
OUT: {

	 }

}
