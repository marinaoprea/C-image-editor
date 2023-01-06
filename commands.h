#ifndef __COMMANDS__
#define __COMMANDS__

#include "struct.h"

// header contains functions that deal with the given commands

void load_cmd(char *filename, unsigned char ***im_bw, unsigned char ***im_gray,
			  colored_image * **im_color, int *height, int *width);

void exit_cmd(unsigned char **im_bw, unsigned char **im_gray,
			  colored_image **im_color, int height, filter *f);

void save_cmd(char **line, unsigned char **im_bw, unsigned char **im_gray,
			  colored_image **im_color, int height, int width);

void select_all(int *x1, int *y1, int *x2, int *y2, int height, int width,
				int output);

void select_cmd(char *line, int *x1, int *y1, int *x2, int *y2, int height,
				int width);

void apply_cmd(char *line, unsigned char **im_bw, unsigned char **im_gray,
			   colored_image **im_color, int x1, int y1, int x2, int y2,
			   int height, int width, filter *f);

void crop_cmd(unsigned char ***im_bw, unsigned char ***im_gray,
			  colored_image ***im_color, int *height, int *width, int *x1,
			  int *y1, int *x2, int *y2);

void histogram_cmd(char *line, unsigned char **im_bw, unsigned char **im_gray,
				   colored_image **im_color, int height, int width);

void equalize_cmd(unsigned char **im_bw, unsigned char **im_gray,
				  colored_image **im_color, int height, int width);

void rotate_cmd(char *line, unsigned char ***im_bw, unsigned char ***im_gray,
				colored_image ***im_color, int *x1, int *y1, int *x2, int *y2,
				int *height, int *width);

#endif
