#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#define PNG_SIG_CAP 8
const uint8_t PNG_SIG[PNG_SIG_CAP] = {137, 80, 78, 71, 13, 10, 26, 10};

void read_bytes(FILE *file, void *buf, size_t buf_cap) {
	size_t n = fread(buf, buf_cap, 1, file);
	if (n != 1) {
		if (ferror(file)) {
			fprintf(stderr, "ERROR: conld not read PNG header: %s\n",
					strerror(errno));
			exit(1);
		} else if (feof(file)) {
			fprintf(stderr, "ERROR: could not read PNG header: reached the end of file");
			exit(1);
		} else {
			assert(0 && "unreachable");
		}
	}

}

void print_buffer(uint8_t *buf, size_t buf_cap) {
	for (size_t i = 0; i < buf_cap; i++) {
		printf("%u ", buf[i]);
	}
	printf("\n");
}

void reverse_bytes(void *buf_void, size_t buf_cap) {
	uint8_t *buf = buf_void;
	for (size_t i = 0; i < buf_cap / 2; i++) {
		uint8_t t = buf[buf_cap - i - 1];
		buf[buf_cap - i - 1] = buf[i];
		buf[i] = t;
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

	FILE *input_file = fopen(input_filepath, "rb");
	if (input_file == NULL) {
		fprintf(stderr, "ERROR: could not open file %s: %s", 
				input_filepath, strerror(errno));
		exit(1);
	}

	uint8_t sig[PNG_SIG_CAP];
	read_bytes(input_file, sig, PNG_SIG_CAP);
	print_buffer(sig, PNG_SIG_CAP);
	if (memcmp(sig, PNG_SIG, PNG_SIG_CAP) != 0) {
		fprintf(stderr, "ERROR: %s does not appear to be a valid PNG file\n", input_filepath);
		exit(1);
	}

	printf("%s is a valid PNG file\n", input_filepath);

	/* chunks */

	uint32_t chunk_sz;	// 4 bytes: data field size
	read_bytes(input_file, &chunk_sz, sizeof chunk_sz);
	reverse_bytes(&chunk_sz, sizeof chunk_sz);
	printf("Chunk Size: %u\n", chunk_sz);

	uint8_t chunk_type[4];
	read_bytes(input_file, chunk_type, sizeof chunk_type);
	printf("Chunk type: %.*s\n", (int)sizeof chunk_type, chunk_type);

	/* chunks end */

	fclose(input_file);
	return 0;
}
