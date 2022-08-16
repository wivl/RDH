#ifndef RDH_H
#define RDH_H

#define L 32

void generate_key_image(const unsigned width, const unsigned height);

void get_key_image(const char *filename, unsigned char **image,
		unsigned *width, unsigned *height);

void stream_encrypt(const unsigned char *key, unsigned char *image, const long bufsize);

void watermark(unsigned char *image, unsigned width, unsigned height,
		char *filename);

void watermark_process(unsigned char *image, const unsigned char *key,
		unsigned width, unsigned height, char *watermarkfile,
		char *messagefile);
#endif
