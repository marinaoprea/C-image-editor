// Copyright Marina Oprea 313CAb 2022-2023

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "op_on_color.h"
#include "struct.h"
#include "auxiliars.h"
#include "commands.h"

// function allocates color matrix
// and returns pointer to its address
colored_image **alloc_matrix_color(int height, int width)
{
	colored_image **a;
	a = malloc(height * sizeof(colored_image *));
	if (!a) {
		fprintf(stderr, "malloc() failed\n");
		return NULL;
	}

	for (int i = 0; i < height; i++) {
		a[i] = malloc(width * sizeof(colored_image));
		if (!a[i]) {
			fprintf(stderr, "malloc() failed\n");
			i--;
			while (i >= 0) {
				free(a[i]);
				i--;
			}
			free(a);
			return NULL;
		}
	}
	return a;
}

// function loads color matrix from file
colored_image **load_color(char *filename, int type, int *height, int *width)
{
	colored_image **image;

	if (type % 10 == 0) {
		FILE *in = fopen(filename, "rt");
		if (!in)
			return NULL;
		unsigned char c;
		int auxr, auxg, auxb;
		fscanf(in, "P%c%d%d%d\n", &c, width, height, &auxr);
		image = alloc_matrix_color(*height, *width);
		if (!image)
			return NULL;
		for (int i = 0; i < *height; i++)
			for (int j = 0; j < *width; j++) {
				fscanf(in, "%d%d%d", &auxr, &auxg, &auxb);
				image[i][j].R = (unsigned char)auxr;
				image[i][j].G = (unsigned char)auxg;
				image[i][j].B = (unsigned char)auxb;
			}
		fclose(in);
	} else {
		FILE *in = fopen(filename, "r");
		if (!in)
			return NULL;
		int aux;
		int aux2;
		fscanf(in, "P%d%d%d%d", &aux2, width, height, &aux);
		image = alloc_matrix_color(*height, *width);
		if (!image)
			return NULL;
		fseek(in, 1, SEEK_CUR);
		for (int i = 0; i < *height; i++)
			for (int j = 0; j < *width; j++) {
				fread(&image[i][j].R, sizeof(unsigned char), 1, in);
				fread(&image[i][j].G, sizeof(unsigned char), 1, in);
				fread(&image[i][j].B, sizeof(unsigned char), 1, in);
			}
		fclose(in);
	}

	return image;
}

// function frees allocated memory for color matrix
void free_matrix_color(colored_image **a, int m)
{
	for (int i = 0; i < m; i++)
		free(a[i]);
	free(a);
}

// function saves color matrix to file given by name
void save_color(char *filename, colored_image **im, int height, int width,
				int ascii)
{
	if (ascii == 1) {
		FILE *out = fopen(filename, "wt");
		fprintf(out, "P3\n%d %d\n", width, height);
		fprintf(out, "%d\n", MAX_VAL);
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++)
				fprintf(out, "%hhu %hhu %hhu ", im[i][j].R, im[i][j].G,
						im[i][j].B);
			fprintf(out, "\n");
		}
		fclose(out);
	} else {
		FILE *out = fopen(filename, "w");
		fprintf(out, "P6\n%d %d\n%d\n", width, height, MAX_VAL);
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++) {
				fwrite(&im[i][j].R, sizeof(unsigned char), 1, out);
				fwrite(&im[i][j].G, sizeof(unsigned char), 1, out);
				fwrite(&im[i][j].B, sizeof(unsigned char), 1, out);
			}
		fclose(out);
	}
}

// function that returns pixel value corresponding to image kernel application
// on (x, y)-upper-lefted corner
colored_image apply_pixel(filter *f, int type, colored_image **im, int x, int y)
{
	colored_image ans;
	double val1 = 0., val2 = 0., val3 = 0.;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++) {
			val1 += (double)((int)im[x + i][y + j].R * f[type].kernel[i][j]);
			val2 += (double)((int)im[x + i][y + j].G * f[type].kernel[i][j]);
			val3 += (double)((int)im[x + i][y + j].B * f[type].kernel[i][j]);
		}
	val1 = round(val1 / (double)f[type].divide);
	val2 = round(val2 / (double)f[type].divide);
	val3 = round(val3 / (double)f[type].divide);
	ans.R = clamp(val1, 0, MAX_VAL);
	ans.G = clamp(val2, 0, MAX_VAL);
	ans.B = clamp(val3, 0, MAX_VAL);

	return ans;
}

// function crops color matrix
// allocates memory for the cropped version, deallocates the old one
// and return address of new matrix
// function updates current selection to the full cropped image
colored_image **crop_color(colored_image **im, int *height, int *width,
						   int *x1, int *y1, int *x2, int *y2)
{
	colored_image **new_im = alloc_matrix_color(*y2 - *y1, *x2 - *x1);
	for (int i = *y1; i < *y2; i++)
		for (int j = *x1; j < *x2; j++)
			new_im[i - *y1][j - *x1] = im[i][j];
	free_matrix_color(im, *height);
	*height = *y2 - *y1;
	*width = *x2 - *x1;
	select_all(x1, y1, x2, y2, *height, *width, 0);

	return new_im;
}

// function rotates the full matrix and returns a pointer to its new memory
// address; it also deallocates memory of the old matrix
colored_image **rotate_full_color(colored_image **im, int *height, int *width)
{
	colored_image **im_new = alloc_matrix_color(*width, *height);
	for (int i = 0; i < *height; i++)
		for (int j = 0; j < *width; j++)
			im_new[j][*height - i - 1] = im[i][j];

	free_matrix_color(im, *height);
	swap_int_values(height, width);
	return im_new;
}

// function that calls full rotation function in case of full selection
// or rotates a selection of the image
void rotate_color(colored_image ***im, int *x1, int *y1, int *x2, int *y2,
				  int *height, int *width)
{
	if (*x1 == 0 && *x2 == *width && *y1 == 0 && *y2 == *height) {
		*im = rotate_full_color(*im, height, width);
		swap_int_values(x2, y2);
		return;
	}
	int dim = *x2 - *x1;
	colored_image **aux = alloc_matrix_color(dim, dim);
	for (int i = *y1; i < *y2; i++)
		for (int j = *x1; j < *x2; j++)
			aux[j - *x1][dim - (i - *y1) - 1] = (*im)[i][j];
	for (int i = *y1; i < *y2; i++)
		for (int j = *x1; j < *x2; j++)
			(*im)[i][j] = aux[i - *y1][j - *x1];
	free_matrix_color(aux, dim);
}

