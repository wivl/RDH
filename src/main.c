#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
//#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>

#include "png.h"
#include "rdh.h"


void usage(char *filename) {
    printf("usage: %s [options] [arguments]\n", filename);
    printf("options:\n");
    printf("-e\t\t\tencrypt and hide message and watermark to a png file\n");
    printf("-d\t\t\tdecrypt and get message and watermark of a png file\n");
    printf("-g\t\t\tgenerate key png file for a certain image\n");
    printf("-h\t\t\tprint this help message\n");
    printf("-i [image filepath]\tspecify the image to be processed\n");
    printf("-k [key filepath]\tspecify the image to be processed\n");
    printf("-w [watermark filepath]\t specify watermark file\n");
    printf("-m [message filepath]\t specify message file\n");
}

extern int optind,opterr,optopt;
extern char *optarg;
int main(int argc, char **argv) {
	assert(*argv != NULL);

    char *program = argv[0];

	if (argc == 1) {
        usage(program);
		exit(1);
	}


    char *input_filepath = NULL;
    char *keyfile_path = NULL;
    char *watermark_input = "watermark.txt";
    char *message_input = "message.txt";

    int opt, flags;
    char *optstring = "edghi:k:w:m:";

    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
            case 'e':
                flags = 1;
                break;
            case 'd':
                flags = 2;
                break;
            case 'g':
                flags = 3;
                break;
            case 'i':
                input_filepath = optarg;
                break;
            case 'k':
                keyfile_path = optarg;
                break;
            case 'w':
                watermark_input = optarg;
                break;
            case 'm':
                message_input = optarg;
                break;
            case 'h':
                usage(program);
                exit(0);
            case '?':
                fprintf(stderr, "Unknown flag: -%c\n", optopt);
                usage(program);
                exit(1);
            default:
                break;
        }
    }
    bool error = false;

    unsigned char* raw_png = 0;
    size_t pngsize;
    unsigned char* image = 0;
    unsigned width, height;
    unsigned char *keyimage = 0;


    switch (flags) {
        case 1:
            if (input_filepath == NULL) {
                fprintf(stderr, "ERROR: Missing image file path.\n");
                error = true;
            }
            if (keyfile_path == NULL) {
                fprintf(stderr, "ERROR: Missing key image file path.\n");
                error = true;
            }
            if (error) {
                fprintf(stderr, "The process stopped due to the error(s) above.\n");
                exit(1);
            }
            load_png(input_filepath, &raw_png, &pngsize);
            decode_png(&image, &width, &height, raw_png, pngsize);
            get_key_image(keyfile_path, &keyimage, &width, &height);
            watermark_process(image, width, height, keyimage, watermark_input, message_input);
            encode_and_save("processed.png", image, width, height);
            printf("The processed image is saved as 'processed.png'.\n");
            printf("Process done!\n");
            break;
        case 2:
            if (input_filepath == NULL) {
                fprintf(stderr, "ERROR: Missing image file path.\n");
                error = true;
            }
            if (keyfile_path == NULL) {
                fprintf(stderr, "ERROR: Missing key image file path.\n");
                error = true;
            }
            if (error) {
                fprintf(stderr, "The process stopped due to the error(s) above.\n");
                exit(1);
            }
            load_png(input_filepath, &raw_png, &pngsize);
            decode_png(&image, &width, &height, raw_png, pngsize);
            get_key_image(keyfile_path, &keyimage, &width, &height);
            recover_process(image, width, height, keyimage, watermark_input, message_input);
            encode_and_save("recover.png", image, width, height);
            printf("The recovered image is saved as 'recover.png'.\n");
            printf("Process done!\n");
            break;
        case 3:
            if (input_filepath == NULL) {
                fprintf(stderr, "ERROR: Missing image file path.\n");
                error = true;
            }
            if (error) {
                fprintf(stderr, "The process stopped due to the error(s) above.\n");
                exit(1);
            }
            load_png(input_filepath, &raw_png, &pngsize);
            decode_png(&image, &width, &height, raw_png, pngsize);
            generate_key_image(width, height);
            printf("The key png file is saved as 'key.png'\n");
            printf("Process done!\n");
            break;
        default:
            assert(0 && "unreachable");
    }




	free(raw_png);
	free(image);


	return 0;
}
