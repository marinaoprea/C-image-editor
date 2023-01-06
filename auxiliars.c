#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auxiliars.h"
#include "struct.h"

// function applied to coordinates (x, y) of an image kernel
// requires 0 <= x, y <= 2
// function returns 1 whether (x, y) in the corner of the kernel and
// 0 otherwise
int iscorner(int x, int y)
{
	if (x == 0 && y == 0)
		return 1;
	if (x == 0 && y == 2)
		return 1;
	if (x == 2 && y == 0)
		return 1;
	if (x == 2 && y == 2)
		return 1;
	return 0;
}

// function returns 1 whether character x is a space( , \t, \n) and 0
// otherwise
int is_space(char x)
{
	if (x == ' ' || x == '\n' || x == '\t')
		return 1;
	return 0;
}

// function that allocs memory for an array of 4 specific filters and returns
// a pointer to the memory address of the array
// both fields name and kernel are dinamically allocated and should be freed
// after usage
filter *define_filters(void)
{
	filter *f = malloc(4 * sizeof(filter));
	for (int i = 0; i < 4; i++) {
		f[i].name = malloc(15 * sizeof(unsigned char));
		f[i].kernel = malloc(3 * sizeof(int *));
		for (int j = 0; j < 3; j++)
			f[i].kernel[j] = malloc(3 * sizeof(int));
	}

	strcpy(f[0].name, "EDGE");
	strcpy(f[1].name, "SHARPEN");
	strcpy(f[2].name, "BLUR");
	strcpy(f[3].name, "GAUSSIAN_BLUR");

	f[0].divide = 1;
	f[1].divide = 1;
	f[2].divide = 9;
	f[3].divide = 16;

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++) {
			f[2].kernel[i][j] = 1;
			f[0].kernel[i][j] = -1;
			if (iscorner(i, j)) {
				f[1].kernel[i][j] = 0;
				f[3].kernel[i][j] = 1;
			} else {
				f[1].kernel[i][j] = -1;
				f[3].kernel[i][j] = 2;
			}
		}
	f[1].kernel[1][1] = 5;
	f[3].kernel[1][1] = 4;
	f[0].kernel[1][1] = 8;

	return f;
}

// function that returns the type of a ppm or pgm image based on the magic word
// existing in the file given as parameter by its name
// type is encoded through a 2 digit number as follows:
// first digit = 0 for black and white images
// first digit = 1 for grayscale images
// first digit = 2 for color images
// last digit = 0 for ascii input
// last digit = 1 for binary input
// type = -1 if the file is nonexistent or could not be opened
int image_type(char *filename)
{
	int type, ans;
	FILE *in = fopen(filename, "r");

	if (!in) {
		printf("Failed to load %s\n", filename);
		return -1;
	}

	fscanf(in, "P%d", &type);
	switch (type) {
	case 1:
		ans = 0;
		break;
	case 4:
		ans =  1;
		break;
	case 2:
		ans =  10;
		break;
	case 5:
		ans =  11;
		break;
	case 3:
		ans =  20;
		break;
	case 6:
		ans =  21;
		break;
	default:
		break;
	}
	fclose(in);

	return ans;
}

// function that checks if there is any image loaded and prints the
// error message otherwise
// furthermore, function returns 1 if an image exists and 0 otherwise
int check_existence(unsigned char **im_bw, unsigned char **im_gray,
					colored_image **im_color)
{
	if (!im_bw && !im_gray && !im_color) {
		printf("No image loaded\n");
		return 0;
	}
	return 1;
}

// function that swaps the int values indicated by the addresses given
// as parameters
// function does not swap the memory addreses
void swap_int_values(int *x, int *y)
{
	int aux = *x;
	*x = *y;
	*y = aux;
}

// function that avoids overflow when working with unsigned chars
// function returns minimum type value in case of underflow and
// maximum type value in case of overflow
unsigned char clamp(int x, int low, int high)
{
	if (x < low)
		return (unsigned char)low;
	if (x > high)
		return (unsigned char)high;
	return (unsigned char)x;
}
