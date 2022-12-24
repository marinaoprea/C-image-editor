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

filter *define_filters()
{
    filter *f = malloc(4 * sizeof(filter));
    for (int i = 0; i < 4; i++) {
        f[i].name = malloc(15 * sizeof(unsigned char));
        f[i].kernel = malloc(3 * sizeof(int*));
        for (int j = 0; j < 4; j++)
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
            } else
            {
                f[1].kernel[i][j] = -1;
                f[3].kernel[i][j] = 2;
            }
        }
    f[1].kernel[1][1] = 5;
    f[3].kernel[1][1] = 4;
    f[0].kernel[1][1] = 8;

    return f;
}

int image_type(unsigned char *filename)
{
    int type, ans;
    FILE *in = fopen(filename, "r");

    /*int c;
    int isbinary = 0;
    while (c = fgetc(in) && c != EOF) {
        if (isspace(c) == 0 && (isascii(c) == 0 || iscntrl(c) == 0)) {
            isbinary = 1;
            break;
        }
    }
    fclose(in);*/

    //if (isbinary == 0) {
    //FILE *in = fopen(filename, "rt");
    fscanf(in, "P%d", &type);
    switch (type)
    {
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
    /*} else {
        FILE *in = fopen(filename, "rb");
        unsigned char *aux2 = malloc(2 * sizeof(unsigned char));
        fread(aux2, sizeof(unsigned char), 2, in);
        switch (aux2[1])
        {
        case '1':
            ans =  0;
        case '4':
            ans =  1;
        case '2':
            ans =  10;
        case '5':
            ans =  11;
        case '3':
            ans =  20;
        case '6':
            ans =  21;
        default:
            break;
        }
        fclose(in);
        free(aux2);
    }*/

    return ans;
}

unsigned char **alloc_matrix(int height, int width)
{
    unsigned char **a;
    a = malloc(height * sizeof(unsigned char *));
    if (a == NULL) {
        fprintf(stderr, "malloc() failed\n");
        return NULL;
    }

    for (int i = 0; i < height; i++) {
        a[i] = malloc(width * sizeof(unsigned char));
        if (a[i] == NULL) {
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
    if (a == NULL) {
        fprintf(stderr, "malloc() failed\n");
        return NULL;
    }

    for (int i = 0; i < height; i++) {
        a[i] = malloc(width * sizeof(colored_image));
        if (a[i] == NULL) {
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

unsigned char **load_bw(unsigned char *filename, int type, int *height, int *width) {
    unsigned char **image;

    if (type%10 == 0) {
        FILE *in = fopen(filename, "rt");
        if (in == NULL)
            return NULL;
        unsigned char c;
        int aux;
        fscanf(in, "P%c%d%d%d\n", &c, width, height, &aux);
        image = alloc_matrix(*height, *width);
        if (image == NULL)
            return NULL;
        for (int i = 0; i < *height; i++)
            for (int j = 0; j < *width; j++) {
                fscanf(in, "%d", &aux);
                image[i][j] = (unsigned char) aux;
            }
        fclose(in);
    } else {
        FILE *in = fopen(filename, "rb");
        if (in == NULL)
            return NULL;
        int aux;
        unsigned char *aux2 = malloc(2 * sizeof(unsigned char));
        fscanf(in, "%s%d%d%d", aux2, width, height, &aux);
        image = alloc_matrix(*height, *width);
        if (image == NULL)
            return NULL;
        for (int i = 0; i < *height; i++)
            for (int j = 0; j < *width; j++)
                fread(&(image[i][j]), sizeof(unsigned char), 1, in);
        free(aux2);
        fclose(in);
    }

    return image;
}

colored_image **load_color(unsigned char *filename, int type, int *height, int *width)
{
    colored_image **image;

    if (type%10 == 0) {
        FILE *in = fopen(filename, "rt");
        if (in == NULL)
            return NULL;
        unsigned char c;
        int auxr, auxg, auxb;
        fscanf(in, "P%c%d%d%d\n", &c, width, height, &auxr);
        image = alloc_matrix_color(*height, *width);
        if (image == NULL)
            return NULL;
        for (int i = 0; i < *height; i++)
            for (int j = 0; j < *width; j++) {
                fscanf(in, "%d%d%d", &auxr, &auxg, &auxb);
                image[i][j].R = (unsigned char) auxr;
                image[i][j].G = (unsigned char) auxg;
                image[i][j].B = (unsigned char) auxb;
            }
        fclose(in);
    } else {
        FILE *in = fopen(filename, "rb");
        if (in == NULL)
            return NULL;
        int aux;
        unsigned char *aux2 = malloc(2 * sizeof(unsigned char));
        fscanf(in, "%s%d%d%d", aux2, width, height, &aux);
        image = alloc_matrix_color(*height, *width);
        if (image == NULL)
            return NULL;
        for (int i = 0; i < *height; i++)
            for (int j = 0; j < *width; j++) {
                fread(&(image[i][j].R), sizeof(unsigned char), 1, in);
                fread(&(image[i][j].G), sizeof(unsigned char), 1, in);
                fread(&(image[i][j].B), sizeof(unsigned char), 1, in);
            }
        free(aux2);
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

void load_cmd(unsigned char *filename, unsigned char ***im_bw, unsigned char ***im_gray, colored_image ***im_color, int *height, int *width)
{
    int type = image_type(filename);

    if (*im_bw != NULL)
        free_matrix_bw(*im_bw, *height);
    else
        if (*im_gray != NULL)
            free_matrix_bw(*im_gray, *height);
        else
            if (*im_color != NULL)
                free_matrix_color(*im_color, *height);

    if (type < 10) {
        *im_bw = load_bw(filename, type, height, width);
        if (*im_bw == NULL) {
            printf("Failed to load %s\n", filename);
            return;
        }
    }
    else {
        if (type < 20) {
            *im_gray = load_bw(filename, type, height, width);
            if (*im_gray == NULL) {
                printf("Failed to load %s\n", filename);
                return;
            }
        } else {
            *im_color = load_color(filename, type, height, width);
            if (*im_color == NULL) {
                printf("Failed to load %s\n", filename);
                return;
            }
        }
    }
    printf("Loaded %s\n", filename);
}

void exit_cmd(unsigned char **im_bw, unsigned char **im_gray, colored_image **im_color, int height)
{
    if (im_bw != NULL)
        free_matrix_bw(im_bw, height);
    if (im_gray != NULL)
        free_matrix_bw(im_gray, height);
    if (im_color != NULL)
        free_matrix_color(im_color, height);
}

void save_gray(unsigned char *filename, unsigned char **im, int height, int width, int ascii)
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
                fwrite(&(im[i][j]), sizeof(unsigned char), 1, out);
        fclose(out);
    }
}

void save_color(unsigned char *filename, colored_image **im, int height, int width, int ascii)
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
                fwrite(&(im[i][j].R), sizeof(unsigned char), 1, out);
                fwrite(&(im[i][j].G), sizeof(unsigned char), 1, out);
                fwrite(&(im[i][j].B), sizeof(unsigned char), 1, out);
            }
        fclose(out);
    }
}

void save_cmd(unsigned char **line, unsigned char **im_bw, unsigned char **im_gray, colored_image **im_color, int height, int width)
{
    if (im_bw == NULL && im_gray == NULL && im_color == NULL) {
        printf("No image loaded\n");
        return;
    }

    unsigned char *filename;

    int ascii = 0;
    if (strstr(*line, "ascii")) {
        ascii = 1;
        filename = strtok(*line, " ");
        filename = strtok(NULL, " ");
    } else {
            (*line)[strlen(*line) - 1] = '\0';
            filename = strstr(*line, "SAVE") + strlen("SAVE") + 1;
    }

   /* if (im_bw)
        save_bw(filename, im_bw, height, width, ascii);
    else*/
        if (im_gray)
            save_gray(filename, im_gray, height, width, ascii);
            else
                save_color(filename, im_color, height, width, ascii);
    printf("Saved %s\n", filename);
}

int check_existence(unsigned char **im_bw, unsigned char **im_gray, colored_image **im_color)
{
    if (im_bw == NULL && im_gray == NULL && im_color == NULL) {
        printf("No image loaded\n");
        return 0;
    }
    return 1;
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
    int *aux = x;
    x = y;
    y = aux;
}

void select_cmd(unsigned char *line, int *x1, int *y1, int *x2, int *y2, int height, int width)
{
    unsigned char *aux2 = malloc(2 * sizeof(unsigned char));
    int x1nou, y1nou, x2nou, y2nou;
    int rc = sscanf(line, "%s%d%d%d%d", aux2, &x1nou, &y1nou, &x2nou, &y2nou);
    if (x1nou > x2nou)
        interschimba(&x1nou, &x2nou);
    if (y1nou > y2nou)
        interschimba(&y1nou, &y2nou);
    if (x1nou < 0 || x1nou >  width || y1nou < 0 || y2nou > height || x2nou < 0 || x2nou > width || y2nou < 0 || y2nou > height) {
        printf("Invalid coordinates\n");
        return;
    }
    *x1 = x1nou;
    *y1 = y1nou;
    *x2 = x2nou;
    *y2 = y2nou;
    printf("Selected %d %d %d %d \n", *x1, *y1, *x2, *y2);
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
    double val1 = 0, val2 = 0, val3 = 0;
    for (int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++) {
            val1 += ((int)im[x + i][y + j].R) * f[type].kernel[i][j];
            val2 += ((int)im[x + i][y + j].G) * f[type].kernel[i][j];
            val3 += ((int)im[x + i][y + j].B) * f[type].kernel[i][j];
        }
    val1 = round(1.0 * val1 / f[type].divide);
    val2 = round(1.0 * val2 / f[type].divide);
    val3 = round(1.0 * val3 / f[type].divide);
    ans.R = clamp((int)val1, 0, 255);
    ans.B = clamp((int)val2, 0, 255);
    ans.G = clamp((int)val3, 0, 255);

    return ans;
}

void apply_cmd(unsigned char *line, unsigned char **im_bw, unsigned char **im_gray, colored_image **im_color, int x1, int y1, int x2, int y2, filter *f)
{
    line[strlen(line) - 1] = '\0';
    unsigned char *filter = strstr(line, "APPLY") + strlen("APPLY") + 1;
    int type;

    if (!check_existence(im_bw, im_gray, im_color))
        return;

    if (im_bw || im_gray) {
        printf("Easy, Charlie Chaplin\n");
        return;
    }

    for (int i = 0; i < 4; i++)
        if (strcmp(filter, f[i].name) == 0)
            type = i;

    for (int i = y1; i <= y2 - 3; i++)
        for (int j = x1; j <= x2 - 3; j++) {
            im_color[i][j] = apply_pixel(f, type, im_color, i, j);
        }
}

int main(void)
{
    unsigned char *line = malloc(100 * sizeof(unsigned char));
    unsigned char **im_bw = NULL, **im_gray = NULL;
    colored_image **im_color = NULL;
    int height = 0, width = 0;
    int x1, x2, y1, y2;
    filter *my_filters = define_filters();

    while (fgets(line, 100, stdin)) {
        if (strstr(line, "LOAD")) {
            unsigned char *filename = strstr(line, "LOAD") + strlen("LOAD") + 1;
            filename[strlen(filename)-1] = '\0';
            load_cmd(filename, &im_bw, &im_gray, &im_color, &height, &width);
            select_all(&x1, &y1, &x2, &y2, height, width, 0);
        }
        else {
            if (strstr(line, "EXIT"))
            {
                exit_cmd(im_bw, im_gray, im_color, height);
                break;
            } else {
                if (strstr(line, "SAVE")) {
                    save_cmd(&line, im_bw, im_gray, im_color, height, width);
                } else {
                    if (strstr(line, "SELECT ALL")) {
                        if (check_existence(im_bw, im_gray, im_color));
                            select_all(&x1, &y1, &x2, &y2, height, width, 1);
                    } else {
                        if (strstr(line, "SELECT"))
                            if (check_existence(im_bw, im_gray, im_color))
                                select_cmd(line, &x1, &y1, &x2, &y2, height, width);
                            else;
                        else {
                            if (strstr(line, "APPLY")) {
                                apply_cmd(line, im_bw, im_gray, im_color, x1, y1, x2, y2, my_filters);
                            }
                        }
                    }
                }
            }
        }

        free(line);
        line = malloc(100 * sizeof(unsigned char));
    }

    free(line);

    return 0;
}