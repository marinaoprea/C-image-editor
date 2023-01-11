// Copyright Marina Oprea 313CAb 2022-2023

#ifndef __STRUCT__
#define __STRUCT__

// header for typedefinitions

typedef struct {
	unsigned char R, G, B;
} colored_image;

// field divide avoids working with a kernel of doubles
// we calculate on ints and then divide the result by field divide
typedef struct {
	char *name;
	int **kernel;
	int divide;
} filter;

#endif
