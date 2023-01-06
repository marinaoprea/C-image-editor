#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auxiliars.h"
#include "struct.h"

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

int is_space(char x)
{
	if (x == ' ' || x == '\n' || x == '\t')
		return 1;
	return 0;
}

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

int check_existence(unsigned char **im_bw, unsigned char **im_gray,
					colored_image **im_color)
{
	if (!im_bw && !im_gray && !im_color) {
		printf("No image loaded\n");
		return 0;
	}
	return 1;
}

void swap_int_values(int *x, int *y)
{
	int aux = *x;
	*x = *y;
	*y = aux;
}

unsigned char clamp(int x, int low, int high)
{
	if (x < low)
		return (unsigned char)low;
	if (x > high)
		return (unsigned char)high;
	return (unsigned char)x;
}
