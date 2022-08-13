#include "histogram_shifting.h"
#include "lodepng.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>


void get_histogram(const unsigned char *image, unsigned width, unsigned height,
		long *p, long *z, unsigned **counts) {

	/* count bytes */

	*counts = (unsigned*)malloc(256 * sizeof(unsigned));
	memset(*counts, 0, 256 * sizeof(unsigned));

	for (unsigned i = 0; i < width*height; i++) {
		(*counts)[image[i]]++;
	}

	/* find p */
	*p = -1, *z = -1;
	long pvalue = -1, zvalue = 256;
	for (int i = 0; i < 256; i++) {
		if (pvalue < (*counts)[i]) {
			*p = i;
			pvalue = (*counts)[i];
		}
		if (zvalue > (*counts)[i]) {
			*z = i;
			zvalue = (*counts)[i];
		}
	}
}

void shift(unsigned char *image, const unsigned width, const unsigned height,
		const long p, const long z) {
	assert(p != z);

	if (z > p) {
		for (int i = 0; i < width * height; i++) {
			if (image[i] > p && image[i] < z) {
				image[i] += 1;
			}
		}
	} else {
		for (int i = 0; i < width * height; i++) {
			if (image[i] > z && image[i] < p) {
				image[i] -= 1;
			}
		}
	}
}

#define MESS_BUF_CAP 256
unsigned char message_buff[MESS_BUF_CAP];

void hide_message(const char *filepath, unsigned char *image,
		const unsigned width, const unsigned height,
		const long p, const long z, const unsigned *counts, unsigned long *cap) {

	FILE *message = fopen(filepath, "rb");

	size_t hide_cap = counts[p];

	int ptr = 0;

	size_t sum = 0, n;
	while((n = fread(message_buff, 1, MESS_BUF_CAP, message)) > 0) {
		sum += n;
		for (int i = 0; i < n; i++) {		// for every byte
			unsigned char byte = 0x80;
			while (byte) {
				while (image[ptr] != p && ptr < width*height) ptr++;
				if (ptr >= width*height) {
					fprintf(stderr,
							"ERROR: message is too long: longer than %lu bit",
							hide_cap);
					exit(1);
				}
				if ((message_buff[i] & byte) == byte) {	// if current bit is 1
					if (z > p) {
						image[ptr] += 1;
					} else {
						image[ptr] -= 1;
					}
				}										// 0: do nothing
				ptr += 1;
				byte >>= 1;
			}
		}
	}
	*cap = sum;

	fclose(message);
}

void get_message(const unsigned char *image, const unsigned width, const unsigned height,
		const long p, const long z, char *filename, int byte_cap) {

	assert(z != p);
	memset(message_buff, 0, sizeof message_buff);

	FILE *output = fopen(filename, "wb");

	unsigned char byte = 0;			// 以字节为单位
	int bit_count = 0;				// 当前字节读取的位数 最大 8
	int ptr = 0, buf_ptr = 0;		// image 指针 message 指针
	for (int i = 0; i < byte_cap; i++) {
		bool quit = false;
		while (!quit) {
			if (z > p)
				while (image[ptr] != p && image[ptr] != p+1 && ptr < width*height) ptr++;
			else
				while (image[ptr] != p && image[ptr] != p-1 && ptr < width*height) ptr++;
			if (ptr >= width*height) {
				fprintf(stderr, "ERROR: can not find any message in this image, \
						or the value p given is wrong\n");
				exit(1);
			}
			if (image[ptr] == p) {	// the hided bit is 0
				byte <<= 1;
			} else {
				byte <<= 1;
				byte += 1;
			}
			if (++bit_count == 8) {	// add the full byte to the buffer
				message_buff[buf_ptr++] = byte;
				byte = 0;
				bit_count = 0;
				quit = true;
			}
			if (buf_ptr >= MESS_BUF_CAP) {
				fwrite(message_buff, 1, sizeof message_buff, output);
				buf_ptr = 0;
				memset(message_buff, 0, sizeof message_buff);
			}
			ptr += 1;
		}
	}
	fwrite(message_buff, 1, buf_ptr, output);

	fclose(output);
}
