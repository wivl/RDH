#include "histogram_shifting.h"
#include "lodepng.h"
#include <stdlib.h>

void get_histogram(unsigned char *image, unsigned width, unsigned height,
		unsigned *p, unsigned *z, unsigned *counts) {

	/* count bytes */

	counts = malloc(width*height);
	memset(counts, 0, width*height);

	for (unsigned i = 0; i < width*height; i++) {
		if (image[i] >= 0 && image[i] <= 255) {
			counts[image[i]]++;
		}
	}


}
