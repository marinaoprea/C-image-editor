// Copyright Marina Oprea 313CAb 2022-2023

#ifndef __OP_ON_COLOR__
#define __OP_ON_COLOR__

#include "struct.h"
#include "auxiliars.h"

// header functions implement color image operations

colored_image * *alloc_matrix_color(int height, int width);

colored_image **load_color(char *filename, int type, int *height, int *width);

void free_matrix_color(colored_image **a, int m);

void save_color(char *filename, colored_image **im, int height, int width,
				int ascii);

colored_image apply_pixel(filter *f, int type, colored_image **im, int x,
						  int y);

colored_image **crop_color(colored_image **im, int *height, int *width,
						   int *x1, int *y1, int *x2, int *y2);

colored_image **rotate_full_color(colored_image **im, int *height, int *width);

void rotate_color(colored_image ***im, int *x1, int *y1, int *x2, int *y2,
				  int *height, int *width);

#endif
