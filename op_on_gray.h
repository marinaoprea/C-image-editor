#ifndef __OP_ON_GRAY__
#define __OP_ON_GRAY__

#include "auxiliars.h"

unsigned char **alloc_matrix(int height, int width);

unsigned char **load_bw(char *filename, int type, int *height, int *width);

void free_matrix_bw(unsigned char **a, int m);

void save_gray(char *filename, unsigned char **im, int height, int width,
			   int ascii);

void save_bw(char *filename, unsigned char **im, int height, int width,
			 int ascii);

unsigned char **crop_gray(unsigned char **im, int *height, int *width, int *x1,
						  int *y1, int *x2, int *y2);

long long *histogram(unsigned char **im, int height, int width, int stars,
					 int bins);

unsigned char **rotate_full_bw(unsigned char **im, int *height, int *width);

void rotate_bw(unsigned char ***im, int *x1, int *y1, int *x2, int *y2,
			   int *height, int *width);

#endif
