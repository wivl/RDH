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

    unsigned char *keyimage = 0;
    get_key_image("key.png", &keyimage, &width, &height);

	/* process image */
//    watermark_process(image, width, height, keyimage, "watermark.txt", "message.txt");
//
//    encode_and_save("processed.png", image, width, height);
//
//    recover_process(image, width, height, keyimage, "watermark1.txt", "message1.txt");
//
//    encode_and_save("recover.png", image, width, height);

    watermark(image, width, height, "watermark.txt");

    encode_and_save("watermarked.png", image, width, height);

    get_watermark(image, width, height, "get watermark.txt");

    encode_and_save("recover.png", image, width, height);

//    hide_message("message.txt", image, width, height);
//    encode_and_save("demo.png", image, width, height);
//
//    if (memcmp(image_backup, image, width*height) == 0) {
//        printf("bruh\n");
//    } else {
//        printf("The hiding proecss is working!\n");
//    }
//
//    get_message(image, width, height, "extraction.txt");
//    encode_and_save("demo1.png", image, width, height);
//
//
//
    if (memcmp(image_backup, image, width*height) == 0) {
        printf("Success!\n");
    } else {
        printf("bruh\n");
    }
	/* process image end */


	free(raw_png);
	free(image);


	return 0;
}
