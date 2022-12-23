#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned char R, G, B;
} colored_image;

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
        }
        // else
        //   *im_color = load_color(filename, type, &height, &width);
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
            /*else
                save_color(filename, im_color, height, width, ascii);*/
    printf("Saved %s\n", filename);
}

int main(void)
{
    unsigned char *line = malloc(100 * sizeof(unsigned char));
    unsigned char **im_bw = NULL, **im_gray = NULL;
    colored_image **im_color = NULL;
    int height = 0, width = 0;

    while (fgets(line, 100, stdin)) {
        if (strstr(line, "LOAD")) {
            unsigned char *filename = strstr(line, "LOAD") + strlen("LOAD") + 1;
            filename[strlen(filename)-1] = '\0';
            load_cmd(filename, &im_bw, &im_gray, &im_color, &height, &width);
        }
        else {
            if (strstr(line, "EXIT"))
            {
                exit_cmd(im_bw, im_gray, im_color, height);
                break;
            } else {
                if (strstr(line, "SAVE")) {
                    save_cmd(&line, im_bw, im_gray, im_color, height, width);
                }
            }
        }

        free(line);
        line = malloc(100 * sizeof(unsigned char));
    }

    free(line);

    return 0;
}