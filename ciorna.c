#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
	unsigned char R, G, B;
} colored_image;

typedef struct {
	char *name;
	int **kernel;
	int divide;
} filter;

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

int check_existence(unsigned char **im_bw, unsigned char **im_gray, colored_image **im_color)
{
	if (!im_bw && !im_gray && !im_color) {
		printf("No image loaded\n");
		return 0;
	}
	return 1;
}

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
		//unsigned char *aux2 = malloc(2 * sizeof(unsigned char));
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

void free_matrix_color(colored_image **a, int m)
{
	for (int i = 0; i < m; i++)
		free(a[i]);
	free(a);
}

void load_cmd(char *filename, unsigned char ***im_bw, unsigned char ***im_gray, colored_image ***im_color, int *height, int *width)
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

void exit_cmd(unsigned char **im_bw, unsigned char **im_gray, colored_image **im_color, int height, filter *f)
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

void save_gray(char *filename, unsigned char **im, int height, int width, int ascii)
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

void save_bw(char *filename, unsigned char **im, int height, int width, int ascii)
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

void save_color(char *filename, colored_image **im, int height, int width, int ascii)
{
	if (ascii == 1) {
		FILE *out = fopen(filename, "wt");
		fprintf(out, "P3\n%d %d\n", width, height);
		fprintf(out, "255\n");
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++)
				fprintf(out, "%hhu %hhu %hhu ", im[i][j].R, im[i][j].G, im[i][j].B);
			fprintf(out, "\n");
		}
		fclose(out);
	} else {
		FILE *out = fopen(filename, "w");
		fprintf(out, "P6\n%d %d\n255\n", width, height);
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++) {
				fwrite(&im[i][j].R, sizeof(unsigned char), 1, out);
				fwrite(&im[i][j].G, sizeof(unsigned char), 1, out);
				fwrite(&im[i][j].B, sizeof(unsigned char), 1, out);
			}
		fclose(out);
	}
}

void save_cmd(char **line, unsigned char **im_bw, unsigned char **im_gray, colored_image **im_color, int height, int width)
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

void select_all(int *x1, int *y1, int *x2, int *y2, int height, int width, int output)
{
	*x1 = 0;
	*y1 = 0;
	*x2 = width;
	*y2 = height;
	if (output)
		printf("Selected ALL\n");
}

void interschimba(int *x, int *y)
{
	int aux = *x;
	*x = *y;
	*y = aux;
}

void select_cmd(char *line, int *x1, int *y1, int *x2, int *y2, int height, int width)
{
	unsigned char *aux2 = malloc(10 * sizeof(unsigned char));
	int x1nou, y1nou, x2nou, y2nou;
	int rc = sscanf(line, "%s%d%d%d%d", aux2, &x1nou, &y1nou, &x2nou, &y2nou);
	free(aux2);
	if (rc != 5) {
		printf("Invalid command\n");
		return;
	}
	if (x1nou < 0 || x1nou >  width || y1nou < 0 || y1nou > height || x2nou < 0 || x2nou > width || y2nou < 0 || y2nou > height || x1nou == x2nou || y1nou == y2nou) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (x1nou > x2nou)
		interschimba(&x1nou, &x2nou);
	if (y1nou > y2nou)
		interschimba(&y1nou, &y2nou);
	*x1 = x1nou;
	*y1 = y1nou;
	*x2 = x2nou;
	*y2 = y2nou;
	printf("Selected %d %d %d %d\n", x1nou, y1nou, x2nou, y2nou);
}

unsigned char clamp(int x, int low, int high)
{
	if (x < low)
		return (unsigned char)low;
	if (x > high)
		return (unsigned char)high;
	return (unsigned char)x;
}

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
	ans.R = clamp(val1, 0, 255);
	ans.G = clamp(val2, 0, 255);
	ans.B = clamp(val3, 0, 255);

	return ans;
}

void apply_cmd(char *line, unsigned char **im_bw, unsigned char **im_gray, colored_image **im_color, int x1, int y1, int x2, int y2, int height, int width, filter *f)
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
			new_im[i - y1][j - x1] = apply_pixel(f, type, im_color, i - 1, j - 1);

	for (int i = y1; i < y2; i++) // center of the block (i,j)
		for (int j = x1; j < x2; j++)
			im_color[i][j] = new_im[i - y1][j - x1];

	free_matrix_color(new_im, y2 - y1);

	printf("APPLY %s done\n", f[type].name);
}

unsigned char **crop_gray(unsigned char **im, int *height, int *width, int *x1, int *y1, int *x2, int *y2)
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

colored_image **crop_color(colored_image **im, int *height, int *width, int *x1, int *y1, int *x2, int *y2)
{
	colored_image **new_im = alloc_matrix_color(*y2 - *y1, *x2 - *x1);
	for (int i = *y1; i < *y2; i++)
		for (int j = *x1; j < *x2; j++)
			new_im[i - *y1][j - *x1] = im[i][j];
	free_matrix_color(im, *height);
	*height = *y2 - *y1;
	*width = *x2 - *x1;
	*x1 = 0;
	*y1 = 0;
	*x2 = *width;
	*y2 = *height;

	return new_im;
}

void crop_cmd(unsigned char ***im_bw, unsigned char ***im_gray, colored_image ***im_color, int *height, int *width, int *x1, int *y1, int *x2, int *y2)
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

long long *histogram(unsigned char **im, int height, int width, int stars, int bins)
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

void histogram_cmd(char *line, unsigned char **im_bw, unsigned char **im_gray, colored_image **im_color, int height, int width)
{
	if (!check_existence(im_bw, im_gray, im_color))
		return;
	unsigned char *aux = malloc(10 * sizeof(unsigned char));
	int stars, bins, aux2;
	int rc = sscanf(line, "%s%d%d%d", aux, &stars, &bins, &aux2);
	free(aux);
	if (rc != 3 || bins <= 0 || bins > 256) {
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

void equalize_cmd(unsigned char **im_bw, unsigned char **im_gray, colored_image **im_color, int height, int width)
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
	int bins = 256;
	long long *hist = histogram(im, height, width, -1, bins);
	long long *sum_part = malloc(bins * sizeof(long long));
	sum_part[0] = hist[0];
	for (int i = 1; i < bins; i++)
		sum_part[i] = sum_part[i - 1] + hist[i];
	int area = height * width;
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			double pixel = 1.0 * sum_part[im[i][j]] / area * 255.0;
			im[i][j] = clamp(round(pixel), 0, 255);
		}
	free(sum_part);
	free(hist);
	im = NULL;
	printf("Equalize done\n");
}

unsigned char **rotate_full_bw(unsigned char **im, int *height, int *width)
{
	unsigned char **im_new = alloc_matrix(*width, *height);
	for (int i = 0; i < *height; i++)
		for (int j = 0; j < *width; j++)
			im_new[j][*height - i - 1] = im[i][j];

	free_matrix_bw(im, *height);
	interschimba(height, width);
	return im_new;
}

colored_image **rotate_full_color(colored_image **im, int *height, int *width)
{
	colored_image **im_new = alloc_matrix_color(*width, *height);
	for (int i = 0; i < *height; i++)
		for (int j = 0; j < *width; j++)
			im_new[j][*height - i - 1] = im[i][j];

	free_matrix_color(im, *height);
	interschimba(height, width);
	return im_new;
}

void rotate_bw(unsigned char ***im, int *x1, int *y1, int *x2, int *y2, int *height, int *width)
{
	if (*x1 == 0 && *x2 == *width && *y1 == 0 && *y2 == *height) {
		*im = rotate_full_bw(*im, height, width);
		interschimba(x2, y2);
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

void rotate_color(colored_image ***im, int *x1, int *y1, int *x2, int *y2, int *height, int *width)
{
	if (*x1 == 0 && *x2 == *width && *y1 == 0 && *y2 == *height) {
		*im = rotate_full_color(*im, height, width);
		interschimba(x2, y2);
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

void rotate_cmd(char *line, unsigned char ***im_bw, unsigned char ***im_gray, colored_image ***im_color, int *x1, int *y1, int *x2, int *y2, int *height, int *width)
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
	if ((*x1 != 0 || *x2 != *width || *y1 != 0 || *y2 != *height) && (*x2 - *x1 != *y2 - *y1)) {
		printf("The selection must be square\n");
		return;
	}

	if (degrees == 360 || degrees == -360 || degrees == 0) {
		printf("Rotated %d\n", degrees);
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

int main(void)
{
	char *line = malloc(100 * sizeof(unsigned char));
	unsigned char **im_bw = NULL, **im_gray = NULL;
	colored_image **im_color = NULL;
	int height = 0, width = 0;
	int x1, x2, y1, y2;
	filter *my_filters = define_filters();

	while (fgets(line, 100, stdin)) {
		if (strstr(line, "LOAD")) {
			char *filename = strstr(line, "LOAD") + strlen("LOAD") + 1;
			filename[strlen(filename) - 1] = '\0';
			load_cmd(filename, &im_bw, &im_gray, &im_color, &height, &width);
			select_all(&x1, &y1, &x2, &y2, height, width, 0);
			continue;
		}
		if (strstr(line, "EXIT")) {
			exit_cmd(im_bw, im_gray, im_color, height, my_filters);
			break;
		}
		if (strstr(line, "SAVE")) {
			save_cmd(&line, im_bw, im_gray, im_color, height, width);
			continue;
		}
		if (strstr(line, "SELECT ALL")) {
			if (check_existence(im_bw, im_gray, im_color))
				select_all(&x1, &y1, &x2, &y2, height, width, 1);
			continue;
		}
		if (strstr(line, "SELECT")) {
			if (check_existence(im_bw, im_gray, im_color))
				select_cmd(line, &x1, &y1, &x2, &y2, height, width);
			continue;
		}
		if (strstr(line, "APPLY")) {
			apply_cmd(line, im_bw, im_gray, im_color, x1, y1, x2, y2, height, width, my_filters);
			continue;
		}
		if (strstr(line, "CROP")) {
			crop_cmd(&im_bw, &im_gray, &im_color, &height, &width, &x1, &y1, &x2, &y2);
			continue;
		}
		if (strstr(line, "HISTOGRAM")) {
			histogram_cmd(line, im_bw, im_gray, im_color, height, width);
			continue;
		}
		if (strstr(line, "EQUALIZE")) {
			equalize_cmd(im_bw, im_gray, im_color, height, width);
			continue;
		}

		if (strstr(line, "ROTATE")) {
			rotate_cmd(line, &im_bw, &im_gray, &im_color, &x1, &y1, &x2, &y2, &height, &width);
			continue;
		}
		printf("Invalid command\n");
		/*free(line);
		line = malloc(100 * sizeof(unsigned char));*/
	}

	free(line);

	return 0;
}
