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

#define MES_BUF_CAP 256
unsigned char message_buffer[MES_BUF_CAP];

void watermark(unsigned char *image, unsigned width, unsigned height,
		char *filename) {

	assert(width % L == 0 && height % L == 0);

	FILE *message = fopen(filename, "rb");
	memset(message_buffer, 0, MES_BUF_CAP);

	unsigned char byte = 0x80;
	int ptr = 0;
	int bitcount = 0;
	bool quit = false;
	bool empty = true;
	int n;
	int starti, startj, endi, endj;
	for (int h = 0; h < height/L; h++) {				
		for (int w = 0; w < width/L; w++) {				// devide the buffer into L x L chunks
            if (empty) {
                n = fread(message_buffer, 1, MES_BUF_CAP, message);
                if (n <= 0) {
                    goto OUT;
                } else {
                    empty = false;
                    ptr = 0;
                }
            }
			printf("%c\n", message_buffer[ptr]);
			unsigned char mode = (message_buffer[ptr] & byte) == byte ? 1 : 0;
			mode <<= 1;
			byte >>= 1;
			mode += (message_buffer[ptr] & byte) == byte ? 1 : 0;
			if (byte == 0x01) {
				byte = 0x80;
                if (++ptr == MES_BUF_CAP) {
                    empty = true;
                }
			}
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
			unsigned char mask = 0x18;
			for (int i = starti; i < endi; i++) {		// for every chunk, height
				for (int j = startj; j < endj; j++) {	// for every chunk, width
					image[i*(L/2)+j] ^= mask;
				}
			}


		}
	}
OUT: {

	 }

}
