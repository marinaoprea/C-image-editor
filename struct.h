#ifndef __STRUCT__
#define __STRUCT__

typedef struct {
	unsigned char R, G, B;
} colored_image;

typedef struct {
	char *name;
	int **kernel;
	int divide;
} filter;

#endif
