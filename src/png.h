#ifndef PNG_H
#define PNG_H

#include "lodepng.h"


void load_png(const char* filename, unsigned char** png, size_t* pngsize);

void decode_png(unsigned char** image, unsigned *width, unsigned *height,
		unsigned char* png, unsigned pngsize);

void encode_and_save(const char *filename, const unsigned char *image,
		unsigned width, unsigned height);

#endif
