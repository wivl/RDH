#ifndef RDH_H
#define RDH_H

#define L 32

void generate_key_image(unsigned width, unsigned height);

void get_key_image(const char *filename, unsigned char **image,
		unsigned *width, unsigned *height);

void stream_encrypt(const unsigned char *key, unsigned char *image, long bufsize);

void watermark(unsigned char *image, unsigned width, unsigned height,
		char *filename);

void get_watermark(unsigned char *image, unsigned width, unsigned height,
                   char *filename);

void watermark_process(unsigned char *image, unsigned width, unsigned height,
                       unsigned char *key, char *watermarkfilename, char *messagefilename);

void recover_process(unsigned char *image, unsigned width, unsigned height,
                     unsigned char *key, char *watermarkfilename, char *messagefilename);
#endif
