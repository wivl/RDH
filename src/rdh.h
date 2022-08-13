#ifndef RDH_H
#define RDH_H

void generate_key_image(const unsigned width, const unsigned height);

void get_key_image(const char *filename, unsigned char **image,
		unsigned *width, unsigned *height);

void stream_encrypt(const unsigned char *key, unsigned char *image, const long bufsize);

#endif
