#pragma once

#include <stddef.h>

#define OUTPUT_FILENAME_CAP 512

typedef struct {
  unsigned char *image_data;
  int height;
  int width;
  int channels;
} image_t;

image_t *load_image(const char *filename);
image_t *create_empty_image(int height, int width, int channels);
int store_image(const char *filename, image_t *image);
void free_image(image_t *image);
char *generate_filename(char *out, size_t out_size, const char *filename,
                        const char *filter_name);
