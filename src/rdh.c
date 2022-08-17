#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "rdh.h"
#include "png.h"
#include "histogram_shifting.h"

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
			// printf("mode: %d\n", mode);

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
			// printf("start: (%d, %d), end: (%d, %d)\n", starti, startj, endi, endj);
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
OUT: {}
}

void get_watermark(unsigned char *image, unsigned width, unsigned height,
                   char *filename) {

    FILE *outputfile = fopen(filename, "wb");

    unsigned char mask = 0x18;
    unsigned char byte = 0;
    int bitcount = 0;


    for (int h = 0; h < height/L; h++) {
        for (int w = 0; w < width / L; w++) {   // 每个 L x L 大小的区块
            long starti[4] = {h*L, h*L, h*L+L/2, h*L+L/2};
            long endi[4] = {h*L+L/2, h*L+L/2, (h+1)*L, (h+1)*L};
            long startj[4] = {w*L, w*L+L/2, w*L, w*L+L/2};
            long endj[4] = {w*L+L/2, (w+1)*L, w*L+L/2, (w+1)*L};

            double F[4] = {0, 0, 0, 0};

            for (int k = 0; k < 4; k++) {   // 每个子区块
                for (long i = starti[k]; i < endi[k]; i++) {		// for every chunk, height
                    for (long j = startj[k]; j < endj[k]; j++) {	// for every chunk, width
                        image[i*(width)+j] ^= mask;
                    }
                }
                // 计算波动值
                for (long i = starti[k]+1; i < endi[k]-1; i++) {		// for every chunk, height
                    for (long j = startj[k]+1; j < endj[k]-1; j++) {	// for every chunk, width
                        double temp = fabs((double)image[i*(width)+j] - (double)(image[(i-1)*(width)+j] + (double)image[i*(width)+j-1] + (double)image[(i+1)*(width)+j] + image[i*(width)+j+1]) / 4.0);
                        F[k] += temp;
                    }
                }
            }
            double alpha = F[0];
            int index = 0;
            for (int i = 1; i < 4; i++) {
                if (alpha > F[i]) {
                    index = i;
                    alpha = F[i];
                }
            }
            switch (index) {
                case 0:             // 00
                    byte <<= 2;
                    break;
                case 1:             // 01
                    byte <<= 2;
                    byte += 1;
                    break;
                case 2:             // 10
                    byte <<= 1;
                    byte += 1;
                    byte <<= 1;
                    break;
                case 3:             // 11
                    byte <<= 1;
                    byte += 1;
                    byte <<= 1;
                    byte += 1;
                    break;
                default:
                    assert(0 && "unreachable");
            }
            bitcount += 2;
            if (bitcount == 8) {
                printf("%c\n", byte);
                fwrite(&byte, 1, 1, outputfile);
                byte = 0;
                bitcount = 0;
            }
            // recover
            for (int k = 0; k < 4; k++) {
                if (k == index) continue;
                for (long i = starti[k]; i < endi[k]; i++) {		// for every chunk, height
                    for (long j = startj[k]; j < endj[k]; j++) {	// for every chunk, width
                        image[i*(width)+j] ^= mask;
                    }
                }
            }
        }
    }

    fclose(outputfile);
}

void watermark_process(unsigned char *image, unsigned width, unsigned height,
                       unsigned char *key, char *watermarkfilename, char *messagefilename) {
    stream_encrypt(key, image, width*height);
    watermark(image, width, height, watermarkfilename);

    hide_message(messagefilename, image, width, height);

}

void recover_process(unsigned char *image, unsigned width, unsigned height,
                     unsigned char *key, char *watermarkfilename, char *messagefilename) {

    get_message(image, width, height, messagefilename);
    stream_encrypt(key, image, width*height);

    get_watermark(image, width, height, watermarkfilename);

}
