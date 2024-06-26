// Copyright Marina Oprea 313CAb 2022-2023

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "commands.h"
#include "struct.h"
#include "auxiliars.h"
#include "op_on_gray.h"
#include "op_on_color.h"

// function that deals with image loading
// function deallocates memory of previously loaded image
// function primarily prints error messages and then calls the specific
// functions for the image type
void load_cmd(char *filename, unsigned char ***im_bw, unsigned char ***im_gray,
			  colored_image ***im_color, int *height, int *width)
{
	int type = image_type(filename);

	if (type == -1) {
		if (*im_bw) {
			free_matrix_bw(*im_bw, *height);
			*im_bw = NULL;
		}
		if (*im_gray) {
			free_matrix_bw(*im_gray, *height);
			*im_gray = NULL;
		}
		if (*im_color) {
			free_matrix_color(*im_color, *height);
			*im_color = NULL;
		}
		return;
	}

	if (*im_bw) {
		free_matrix_bw(*im_bw, *height);
		*im_bw = NULL;
	} else {
		if (*im_gray) {
			free_matrix_bw(*im_gray, *height);
			*im_gray = NULL;
		} else {
			if (*im_color) {
				free_matrix_color(*im_color, *height);
				*im_color = NULL;
			}
		}
	}

	if (type < 10) {
		*im_bw = load_bw(filename, type, height, width);
		if (!(*im_bw)) {
			printf("Failed to load %s\n", filename);
			return;
		}
	} else {
		if (type < 20) {
			*im_gray = load_bw(filename, type, height, width);
			if (!(*im_gray)) {
				printf("Failed to load %s\n", filename);
				return;
			}
		} else {
			*im_color = load_color(filename, type, height, width);
			if (!(*im_color)) {
				printf("Failed to load %s\n", filename);
				return;
			}
		}
	}
	printf("Loaded %s\n", filename);
}

// function that deallocates everything as final step of running the programm
// filter structure is taken care of regardless of image existence
void exit_cmd(unsigned char **im_bw, unsigned char **im_gray,
			  colored_image **im_color, int height, filter *f)
{
	for (int i = 0; i < 4; i++) {
		free(f[i].name);
		for (int j = 0; j < 3; j++)
			free(f[i].kernel[j]);
		free(f[i].kernel);
	}
	free(f);

	if (!check_existence(im_bw, im_gray, im_color))
		return;

	if (im_bw)
		free_matrix_bw(im_bw, height);
	if (im_gray)
		free_matrix_bw(im_gray, height);
	if (im_color)
		free_matrix_color(im_color, height);
}

// function that saves an image to the file given as parameter by its name
// (int)ascii = 1 whether we save the output in ascii mode and (int)ascii = 0
// otherwise (binary mode)
void save_cmd(char **line, unsigned char **im_bw, unsigned char **im_gray,
			  colored_image **im_color, int height, int width)
{
	if (!check_existence(im_bw, im_gray, im_color))
		return;

	char *filename;

	int ascii = 0;
	if (strstr(*line, "ascii")) {
		ascii = 1;
		filename = strtok(*line, " ");
		filename = strtok(NULL, " ");
	} else {
		(*line)[strlen(*line) - 1] = '\0';
		filename = strstr(*line, "SAVE") + strlen("SAVE") + 1;
		while (is_space(filename[0]))
			filename++; // avoid whitespaces at the beginning of filename
		while (is_space(filename[strlen(filename) - 1]))
			filename[strlen(filename) - 1] = '\0'; // same for the end
	}

	if (im_bw)
		save_bw(filename, im_bw, height, width, ascii);
	else
		if (im_gray)
			save_gray(filename, im_gray, height, width, ascii);
			else
				save_color(filename, im_color, height, width, ascii);
	printf("Saved %s\n", filename);
}

// function that updates current selection to that of the full image
// selection coordinates are given by address in order to maintain changes
// after function call ended
// (int) output = 1 for printing succeess message, 0 otherwise
// function extends full selection beyond "SELECT ALL" command
void select_all(int *x1, int *y1, int *x2, int *y2, int height, int width,
				int output)
{
	*x1 = 0;
	*y1 = 0;
	*x2 = width;
	*y2 = height;
	if (output)
		printf("Selected ALL\n");
}

// function that updates current selection to the coordinates given as command
// parameters, passed by input line
// selection coordinates are given by addres in order to maintain changes
// note that selection is considered (x, y), x1 <= x < x2, y1 <= y < y2
// note that we assure that (x1, y1) represents upper left corner and
// (x2, y2) represents down right corner
// thus, x1 < x2, y1 < y2 is assured at the end of function call regardless
// of input order
// rc stands for return code
void select_cmd(char *line, int *x1, int *y1, int *x2, int *y2, int height,
				int width)
{
	unsigned char *aux2 = malloc(COMM_LENG * sizeof(unsigned char));
	int x1nou, y1nou, x2nou, y2nou;
	int rc = sscanf(line, "%s%d%d%d%d", aux2, &x1nou, &y1nou, &x2nou, &y2nou);
	free(aux2);
	if (rc != 5) { // wrong number of parameters given as input
		printf("Invalid command\n");
		return;
	}
	if (x1nou < 0 || x1nou >  width || y1nou < 0 || y1nou > height ||
		x2nou < 0 || x2nou > width || y2nou < 0 || y2nou > height ||
		x1nou == x2nou || y1nou == y2nou) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (x1nou > x2nou)
		swap_int_values(&x1nou, &x2nou);
	if (y1nou > y2nou)
		swap_int_values(&y1nou, &y2nou);
	*x1 = x1nou;
	*y1 = y1nou;
	*x2 = x2nou;
	*y2 = y2nou;
	printf("Selected %d %d %d %d\n", x1nou, y1nou, x2nou, y2nou);
}

// function that calls pixel application of filter given as parameter in input
// command
// function allocates a new matrix for the selected surface to avoid
// kernel applications interfering with each other
// (i, j) represents the center of the current block, but we give (i - 1, j - 1)
// as upper left coordinates of the block for pixel apply function
// pixels on the image borders don't have enough neighbours so they remain
// unaffected; thus we change the local selection accordingly
// global selection is not affected as x1, y1, x2, y2 parameters are given by
// value
void apply_cmd(char *line, unsigned char **im_bw, unsigned char **im_gray,
			   colored_image **im_color, int x1, int y1, int x2, int y2,
			   int height, int width, filter *f)
{
	line[strlen(line) - 1] = '\0';
	char *filter = strstr(line, "APPLY") + strlen("APPLY") + 1;

	if (!check_existence(im_bw, im_gray, im_color))
		return;

	if (*filter == '\0') {
		printf("Invalid command\n");
		return;
	}

	if (im_bw || im_gray) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}

	int type = -1;
	for (int i = 0; i < 4; i++)
		if (strcmp(filter, f[i].name) == 0)
			type = i;
	if (type == -1) {
		printf("APPLY parameter invalid\n");
		return;
	}

	if (x1 == 0)
		x1++;
	if (y1 == 0)
		y1++;
	if (x2 == width)
		x2--;
	if (y2 == height)
		y2--;

	colored_image **new_im = alloc_matrix_color(y2 - y1, x2 - x1);

	for (int i = y1; i < y2; i++) // center of the block (i,j)
		for (int j = x1; j < x2; j++)
			new_im[i - y1][j - x1] = apply_pixel(f, type, im_color, i - 1,
												 j - 1);

	for (int i = y1; i < y2; i++) // center of the block (i,j)
		for (int j = x1; j < x2; j++)
			im_color[i][j] = new_im[i - y1][j - x1];

	free_matrix_color(new_im, y2 - y1);

	printf("APPLY %s done\n", f[type].name);
}

// function crops image according to current selection
// function calls specific functions for image type
// the memory adress of the image is changed, being reallocated
// thus the usage of triple pointer, for changes on the matrix address
// to be visible after end of function call
void crop_cmd(unsigned char ***im_bw, unsigned char ***im_gray,
			  colored_image ***im_color, int *height, int *width, int *x1,
			  int *y1, int *x2, int *y2)
{
	if (!check_existence(*im_bw, *im_gray, *im_color))
		return;

	if (*im_bw)
		*im_bw = crop_gray(*im_bw, height, width, x1, y1, x2, y2);
	else
		if (*im_gray)
			*im_gray = crop_gray(*im_gray, height, width, x1, y1, x2, y2);
		else
			*im_color = crop_color(*im_color, height, width, x1, y1, x2, y2);
	printf("Image cropped\n");
}

// function that prints error messages, calls for histogram calculation
// and the prints it accordingly
// rc = return code
void histogram_cmd(char *line, unsigned char **im_bw, unsigned char **im_gray,
				   colored_image **im_color, int height, int width)
{
	if (!check_existence(im_bw, im_gray, im_color))
		return;
	unsigned char *aux = malloc(COMM_LENG * sizeof(unsigned char));
	int stars, bins, aux2;
	int rc = sscanf(line, "%s%d%d%d", aux, &stars, &bins, &aux2);
	free(aux);
	if (rc != 3 || bins <= 0 || bins > MAX_BINS) {
		printf("Invalid command\n");
		return;
	}
	int y = bins;
	while (y > 1)
		y /= 2;
	if (y != 1) {
		printf("Invalid command\n");
		return;
	}
	if (im_color) {
		printf("Black and white image needed\n");
		return;
	}
	long long *hist;
	if (im_bw)
		hist = histogram(im_bw, height, width, stars, bins);
	else
		hist = histogram(im_gray, height, width, stars, bins);

	for (int i = 0; i < bins; i++) {
		printf("%lld\t|\t", hist[i]);
		for (long long j = 0; j < hist[i]; j++)
			printf("*");
		printf("\n");
	}

	free(hist);
}

// function that equalizes grayscale and black&white images
// function works on the image in-place, thus matrix is passed by
// double pointer
// function works with the histogram based on 256 bins, one for each
// possible pixel value
// we use round function to approximate to nearest int value
void equalize_cmd(unsigned char **im_bw, unsigned char **im_gray,
				  colored_image **im_color, int height, int width)
{
	if (!check_existence(im_bw, im_gray, im_color))
		return;
	if (im_color) {
		printf("Black and white image needed\n");
		return;
	}
	unsigned char **im;
	if (im_bw)
		im = im_bw;
	else
		im = im_gray;
	int bins = MAX_BINS;
	long long *hist = histogram(im, height, width, -1, bins);
	long long *sum_part = malloc(bins * sizeof(long long));
	sum_part[0] = hist[0];
	for (int i = 1; i < bins; i++)
		sum_part[i] = sum_part[i - 1] + hist[i];
	int area = height * width;
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			double pixel = 1.0 * sum_part[im[i][j]] / area * MAX_VAL;
			im[i][j] = clamp(round(pixel), 0, MAX_VAL);
		}
	free(sum_part);
	free(hist);
	im = NULL;
	printf("Equalize done\n");
}

// function that deals with image/selection rotation
// we rotate with 90 degrees to the right repeatedly in order to obtain
// the desired result
// negative degrees, meaning rotation to the left, are complementary converted
// in order to apply right-sensed rotation
// by full rotation, the image's matrix is reallocated, thus we pass image
// addresses as triple pointers, in order to maintain the result visible
// selection rotation is permitted only on square selections
void rotate_cmd(char *line, unsigned char ***im_bw, unsigned char ***im_gray,
				colored_image ***im_color, int *x1, int *y1, int *x2, int *y2,
				int *height, int *width)
{
	if (!check_existence(*im_bw, *im_gray, *im_color))
		return;

	char *aux = malloc(10 * sizeof(char));
	int aux2, degrees;
	int rc = sscanf(line, "%s%d%d", aux, &degrees, &aux2);
	free(aux);
	if (rc != 2) {
		printf("Invalid command\n");
		return;
	}
	if (degrees % 90 || degrees < -360 || degrees > 360) {
		printf("Unsupported rotation angle\n");
		return;
	}
	if ((*x1 != 0 || *x2 != *width || *y1 != 0 || *y2 != *height) &&
		(*x2 - *x1 != *y2 - *y1)) {
		printf("The selection must be square\n");
		return;
	}

	if (degrees == 360 || degrees == -360 || degrees == 0) {
		printf("Rotated %d\n", degrees); // image remains the same
		return;
	}

	printf("Rotated %d\n", degrees);

	if (degrees < 0)
		degrees += 360;

	if (*im_bw)
		for (int i = 0; i < degrees / 90; i++)
			rotate_bw(im_bw, x1, y1, x2, y2, height, width);
	else
		if (*im_gray)
			for (int i = 0; i < degrees / 90; i++)
				rotate_bw(im_gray, x1, y1, x2, y2, height, width);
		else
			for (int i = 0; i < degrees / 90; i++)
				rotate_color(im_color, x1, y1, x2, y2, height, width);
}

