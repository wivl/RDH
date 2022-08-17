// Histogram shifting module
#ifndef HIS_SHI
#define HIS_SHI

void get_histogram(const unsigned char *image, unsigned width, unsigned height,
		int starti, int endi, int startj, int endj,
		long *p, long *z, unsigned **counts);
void shift(unsigned char *image, unsigned width, unsigned height,
		int starti, int endi, int startj, int endj,
		long p, long z);
void hide_message(const char *filepath, unsigned char *image,
		unsigned width, unsigned height);

void get_message(unsigned char *image, unsigned width, unsigned height, char *filename);

#endif
