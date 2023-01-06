#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"
#include "auxiliars.h"
#include "commands.h"

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
			while (is_space(filename[strlen(filename) - 1]) == 1)
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
			apply_cmd(line, im_bw, im_gray, im_color, x1, y1, x2, y2, height,
					  width, my_filters);
			continue;
		}
		if (strstr(line, "CROP")) {
			crop_cmd(&im_bw, &im_gray, &im_color, &height, &width, &x1, &y1,
					 &x2, &y2);
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
			rotate_cmd(line, &im_bw, &im_gray, &im_color, &x1, &y1, &x2, &y2,
					   &height, &width);
			continue;
		}
		printf("Invalid command\n");
		/*free(line);
		line = malloc(100 * sizeof(unsigned char));*/
	}

	free(line);

	return 0;
}
