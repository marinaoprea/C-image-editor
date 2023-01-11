// Copyright Marina Oprea 313CAb 2022-2023

#ifndef __AUXILIARS__
#define __AUXILIARS__

#include "auxiliars.h"
#include "struct.h"

// header that contains auxiliary functions for solving the querries

int iscorner(int x, int y);

int is_space(char x);

filter *define_filters(void);

int image_type(char *filename);

int check_existence(unsigned char **im_bw, unsigned char **im_gray,
					colored_image **im_color);

void swap_int_values(int *x, int *y);

unsigned char clamp(int x, int low, int high);

#endif
