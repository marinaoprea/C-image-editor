#include <stdio.h>
#include <stdlib.h>

#include "op_on_gray.h"

unsigned char **alloc_matrix(int height, int width)
{
	unsigned char **a;
	a = malloc(height * sizeof(unsigned char *));
	if (!a) {
		fprintf(stderr, "malloc() failed\n");
		return NULL;
	}

	for (int i = 0; i < height; i++) {
		a[i] = malloc(width * sizeof(unsigned char));
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

unsigned char **load_bw(char *filename, int type, int *height, int *width)
{
	unsigned char **image;

	if (type % 10 == 0) {
		FILE *in = fopen(filename, "rt");
		if (!in)
			return NULL;
		unsigned char c;
		int aux;
		if (type >= 10)
			fscanf(in, "P%c%d%d%d\n", &c, width, height, &aux);
		else
			fscanf(in, "P%c%d%d\n", &c, width, height);
		image = alloc_matrix(*height, *width);
		if (!image)
			return NULL;
		for (int i = 0; i < *height; i++)
			for (int j = 0; j < *width; j++) {
				fscanf(in, "%d", &aux);
				image[i][j] = (unsigned char)aux;
			}
		fclose(in);
	} else {
		FILE *in = fopen(filename, "r");
		if (!in)
			return NULL;
		int aux;
		//unsigned char *aux2 = malloc(2 * sizeof(unsigned char));
		int aux2;
		if (type >= 10)
			fscanf(in, "P%d%d%d%d", &aux2, width, height, &aux);
		else
			fscanf(in, "P%d%d%d", &aux2, width, height);
		image = alloc_matrix(*height, *width);
		if (!image)
			return NULL;
		fseek(in, 1, SEEK_CUR);
		for (int i = 0; i < *height; i++)
			for (int j = 0; j < *width; j++)
				fread(&image[i][j], sizeof(unsigned char), 1, in);
		//free(aux2);
		fclose(in);
	}

	return image;
}

void free_matrix_bw(unsigned char **a, int m)
{
	for (int i = 0; i < m; i++)
		free(a[i]);
	free(a);
}

void save_gray(char *filename, unsigned char **im, int height, int width,
			   int ascii)
{
	if (ascii == 1) {
		FILE *out = fopen(filename, "wt");
		fprintf(out, "P2\n%d %d\n", width, height);
		fprintf(out, "255\n");
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++)
				fprintf(out, "%hhu ", im[i][j]);
			fprintf(out, "\n");
		}
		fclose(out);
	} else {
		FILE *out = fopen(filename, "w");
		fprintf(out, "P5\n%d %d\n255\n", width, height);
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
				fwrite(&im[i][j], sizeof(unsigned char), 1, out);
		fclose(out);
	}
}

void save_bw(char *filename, unsigned char **im, int height, int width,
			 int ascii)
{
	if (ascii == 1) {
		FILE *out = fopen(filename, "wt");
		fprintf(out, "P1\n%d %d\n", width, height);
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++)
				fprintf(out, "%hhu ", im[i][j]);
			fprintf(out, "\n");
		}
		fclose(out);
	} else {
		FILE *out = fopen(filename, "w");
		fprintf(out, "P4\n%d %d\n", width, height);
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
				fwrite(&im[i][j], sizeof(unsigned char), 1, out);
		fclose(out);
	}
}

unsigned char **crop_gray(unsigned char **im, int *height, int *width, int *x1,
						  int *y1, int *x2, int *y2)
{
	unsigned char **new_im = alloc_matrix(*y2 - *y1, *x2 - *x1);
	for (int i = *y1; i < *y2; i++)
		for (int j = *x1; j < *x2; j++)
			new_im[i - *y1][j - *x1] = im[i][j];
	free_matrix_bw(im, *height);
	*height = *y2 - *y1;
	*width = *x2 - *x1;
	*x1 = 0;
	*y1 = 0;
	*x2 = *width;
	*y2 = *height;

	return new_im;
}

long long *histogram(unsigned char **im, int height, int width, int stars,
					 int bins)
{
	long long *hist = malloc(bins * sizeof(long));
	for (int i = 0; i < bins; i++)
		hist[i] = 0;

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			int pixel = (int)im[i][j];
			hist[(int)(pixel / (256 / bins))]++;
		}

	int max_hist = 0;
	for (int i = 0; i < bins; i++)
		if (hist[i] > max_hist)
			max_hist = hist[i];

	if (stars == -1)
		stars = max_hist;
	for (int i = 0; i < bins; i++)
		hist[i] = (int)(hist[i] * stars / max_hist);

	return hist;
}

unsigned char **rotate_full_bw(unsigned char **im, int *height, int *width)
{
	unsigned char **im_new = alloc_matrix(*width, *height);
	for (int i = 0; i < *height; i++)
		for (int j = 0; j < *width; j++)
			im_new[j][*height - i - 1] = im[i][j];

	free_matrix_bw(im, *height);
	swap_int_values(height, width);
	return im_new;
}

void rotate_bw(unsigned char ***im, int *x1, int *y1, int *x2, int *y2,
			   int *height, int *width)
{
	if (*x1 == 0 && *x2 == *width && *y1 == 0 && *y2 == *height) {
		*im = rotate_full_bw(*im, height, width);
		swap_int_values(x2, y2);
		return;
	}
	int dim = *x2 - *x1;
	unsigned char **aux = alloc_matrix(dim, dim);
	for (int i = *y1; i < *y2; i++)
		for (int j = *x1; j < *x2; j++)
			aux[j - *x1][dim - (i - *y1) - 1] = (*im)[i][j];
	for (int i = *y1; i < *y2; i++)
		for (int j = *x1; j < *x2; j++)
			(*im)[i][j] = aux[i - *y1][j - *x1];
	free_matrix_bw(aux, dim);
}
