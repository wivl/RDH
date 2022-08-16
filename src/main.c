#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>


#include "histogram_shifting.h"
#include "png.h"
#include "rdh.h"


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

	unsigned char* image = 0, *image_backup = 0;
	unsigned width, height;
	decode_png(&image, &width, &height, raw_png, pngsize);
	decode_png(&image_backup, &width, &height, raw_png, pngsize);


	/* process image */
    /* test stream encryption */
	
	unsigned char *keyimage = 0;

//	generate_key_image(width, height);
	get_key_image("key.png", &keyimage, &width, &height);
//	stream_encrypt(keyimage, image, width*height);
//
//	encode_and_save("encrypted.png", image, width, height);
//
//	stream_encrypt(keyimage, image, width*height);
//
//	encode_and_save("decrypted.png", image, width, height);

    /* test histogram shifting */

//	 long p, z;
//	 unsigned *counts;
//
//	 get_histogram(image, width, height, &p, &z, &counts);
//
//	 for (int i = 0; i < 256; i++) {
//	 	printf("%u ", counts[i]);
//	 }
//	 puts("");
//
//	printf("p: %ld (%u)\n", p, counts[p]);
//	printf("z: %ld (%u)\n", z, counts[z]);
//
//	shift(image, width, height, p, z);
//
//	char *message_file = "message.txt";
//
//	size_t cap;
//
//	hide_message(message_file, image, width, height, p, z, counts, &cap);
//
//
//	encode_and_save("output.png", image, width, height);
//
//	get_message(image, width, height, p, z, "output.txt", cap);
//
//	encode_and_save("recover.png", image, width, height);
//
//	if(memcmp(image, image_backup, width*height) == 0) {
//		printf("Success!\n");
//	} else {
//		fprintf(stderr, "ERROR: the process has problems\n");
//	}

//	/* test watermark */
//	watermark(image, width, height, "watermark.txt");
//	encode_and_save("watermarked.png", image, width, height);
	watermark_process(image, keyimage, width, height, "watermark.txt", "message.txt");
	encode_and_save("demo.png", image, width, height);
	stream_encrypt(keyimage, image, width*height);
	encode_and_save("demo1.png", image, width, height);
	/* process image end */


	free(raw_png);
	free(image);


	return 0;
}
