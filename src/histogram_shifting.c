#include "histogram_shifting.h"
#include "lodepng.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>


void get_histogram(const unsigned char *image, unsigned width, unsigned height,
		int starti, int endi, int startj, int endj,
		long *p, long *z, unsigned **counts) {

	/* count bytes */

	*counts = (unsigned*)malloc(256 * sizeof(unsigned));
	memset(*counts, 0, 256 * sizeof(unsigned));

	for (int i = starti; i < endi; i++) {
		for (int j = startj; j < endj; j++) {
			(*counts)[image[i*(width)+j]]++;
		}
	}

	/* find p */
	*p = -1, *z = -1;
	long pvalue = -1, zvalue = 256;
	for (int i = 0; i < 256; i++) {
		if (pvalue < (*counts)[i]) {
			*p = i;
			pvalue = (*counts)[i];
		}
		if (zvalue > (*counts)[i]) {
			*z = i;
			zvalue = (*counts)[i];
		}
	}
}

void shift(unsigned char *image, const unsigned width, const unsigned height,
		int starti, int endi, int startj, int endj,
		const long p, const long z) {
	assert(p != z);

	if (z > p) {
		for (int i = starti; i < endi; i++) {
			for (int j = startj; j < endj; j++) {
				if (image[i*width+j] > p && image[i*width+j] < z) {
					image[i*width+j] += 1;
				}
			}

		}
	} else {
		for (int i = starti; i < endi; i++) {
			for (int j = startj; j < endj; j++) {
				if (image[i*width+j] > z && image[i*width+j] < p) {
					image[i*width+j] -= 1;
				}
			}

		}
	}
}

#define MESS_BUF_CAP 256
unsigned char message_buff[MESS_BUF_CAP];

void hide_message(const char *filepath, unsigned char *image,
		const unsigned width, const unsigned height) {

	FILE *message = fopen(filepath, "rb");

    unsigned char message_byte;
    size_t n = fread(&message_byte, 1, 1, message);
    if (n != 1) {
		return;
    }

    unsigned *counts;
    long p, z;

    long starti[4] = {0, 0, height/2, height/2};
    long endi[4] = {height/2, height/2, height, height};
    long startj[4] = {0, width/2, 0, width/2};
    long endj[4] = {width/2, width, width/2, width};

    size_t sum = 0;     // 一共保存保存的字节数
    unsigned char byte = 0x80;

    for (int k = 0; k < 4; k++) {       // 4 parts of the image
        // 对每一个部分进行获取直方图和直方图位移操作
        get_histogram(image, width, height, starti[k], endi[k], startj[k], endj[k], &p, &z, &counts);
        shift(image, width, height, starti[k], endi[k], startj[k], endj[k], p, z);

        size_t hide_count = 0;          // 每一部分保存的比特数
        size_t hide_cap = counts[p];    // 每一个部分最大保存的比特数容量
        for (long i = starti[k]; i < endi[k]; i++) {     // every part of the image, for every chunk
            for (long j = startj[k]; j < endj[k]; j++) {
                if (image[i*width+j] != p) continue;    // 找到灰度值为 p 的像素点

                /* 判断跳出循环条件 */
                if (byte == 0) {
                    n = fread(&message_byte, 1, 1, message);
                    if (n != 1) {
                        goto NEXT_PART;   // 条件 1: 文件读取到末尾, 跳出循环
                    }
                    byte = 0x80;
                    sum++;
                }
                if (hide_count >= hide_cap) {
                    goto NEXT_PART;
                }
                /* 判断跳出循环条件 */

                if ((message_byte & byte) == byte) {
                    if (z > p) {
                        image[i*width+j] += 1;
                    } else {
                        image[i*width+j] -= 1;
                    }
                }
                hide_count++;
                byte >>= 1;

            }
        }
        NEXT_PART:{
            printf("Part %d of 4: \n", k+1);
            printf("CAP: %lu\n", hide_count);
            printf("P: %lu\n", p);
            printf("Z: %lu\n", z);
        }
    }

	fclose(message);
}

void get_message(unsigned char *image, const unsigned width, const unsigned height, char *filename) {

    long starti[4] = {0, 0, height/2, height/2};
    long endi[4] = {height/2, height/2, height, height};
    long startj[4] = {0, width/2, 0, width/2};
    long endj[4] = {width/2, width, width/2, width};

	FILE *output = fopen(filename, "wb");
    unsigned char byte = 0;
    int bitcount = 0;
    unsigned long cap;        // 统计当前部分保存的比特数
    long p, z;

    for (int k = 0; k < 4; k++) {   // 4 个部分
        printf("Please input the value of byte cap, p, z: ");
        scanf("%lu%ld%ld", &cap, &p, &z);
        size_t n = 0;   // 统计已获得的比特数

        assert(p != z);
        if (cap == 0) {        // 如果此部分隐藏的信息 bit 数为 0
            goto RECOVER;
        }
        for (long i = starti[k]; i < endi[k]; i++) {
            for (long j = startj[k]; j < endj[k]; j++) {
                if (image[i*width+j] == p) {
                    byte <<= 1;
                } else if (image[i*width+j] == p+1 || image[i*width+j] == p-1){
                    if (z > p) {
                        if (image[i*width+j] == p+1) {
                            byte <<= 1;
                            byte += 1;
                        }
                    } else {
                        if (image[i*width+j] == p-1) {
                            byte <<= 1;
                            byte += 1;
                        }
                    }
                } else {
                    continue;
                }
                n += 1;
                if (++bitcount == 8) {
                    fwrite(&byte, 1, 1, output);
                    byte = 0;
                    bitcount = 0;
                    if (n >= cap) {
                        goto RECOVER;
                    }
                }

            }
        }
        RECOVER:{}
        /* recover the image */
        if (z > p) {
            for (long i = starti[k]; i < endi[k]; i++) {
                for (long j = startj[k]; j < endj[k]; j++) {
                    if (image[i*width+j] > p && image[i*width+j] <= z) {
                        image[i*width+j] -= 1;
                    }
                }
            }
        } else {
            for (long i = starti[k]; i < endi[k]; i++) {
                for (long j = startj[k]; j < endj[k]; j++) {
                    if (image[i*width+j] >= z && image[i*width+j] < p) {
                        image[i*width+j] += 1;
                    }
                }
            }
        }
        AFTER_RECOVER: {}
    }


	fclose(output);
}

