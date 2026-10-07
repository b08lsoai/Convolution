#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils/filters.h"
#include "utils/image_io.h"

#define BOUND(value, min, max)                                                 \
  ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

static inline void process_pixel(int x, int y, int width, int height,
                                 int channels, unsigned char *image_data,
                                 unsigned char *new_image_data,
                                 double *filter_arr, int filter_size,
                                 int offset, double factor, double bias) {
  double accum[4] = {0.0, 0.0, 0.0, 0.0};

  for (int filter_y = 0; filter_y < filter_size; ++filter_y) {
    for (int filter_x = 0; filter_x < filter_size; ++filter_x) {
      int image_x = (x - offset + filter_x + width) % width;
      int image_y = (y - offset + filter_y + height) % height;
      int idx = (image_y * width + image_x) * channels;
      double filter_val = filter_arr[filter_y * filter_size + filter_x];

      for (int c = 0; c < channels; ++c) {
        accum[c] += image_data[idx + c] * filter_val;
      }
    }
  }

  int dst_idx = (y * width + x) * channels;
  for (int c = 0; c < channels; ++c) {
    double res = factor * accum[c] + bias;
    new_image_data[dst_idx + c] = (unsigned char)BOUND(res, 0, 255);
  }
}

image_t *seq_convolution(image_t *image, filter_t *filter) {
  if (image == NULL || filter == NULL) {
    fprintf(stderr, "Error: NULL pointer in convolution.\n");
    return NULL;
  }

  int height = image->height;
  int width = image->width;
  int channels = image->channels;
  unsigned char *image_data = image->image_data;

  double *filter_arr = filter->filter_arr;
  int filter_size = filter->size;

  image_t *new_image = create_empty_image(height, width, channels);
  if (!new_image) {
    fprintf(stderr, "Error: Cannot create new image.\n");
    return NULL;
  }
  unsigned char *new_image_data = new_image->image_data;

  int offset = filter_size / 2;

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      process_pixel(x, y, width, height, channels, image_data, new_image_data,
                    filter_arr, filter_size, offset, filter->factor,
                    filter->bias);
    }
  }

  return new_image;
}

image_t *parallel_pixel_convolution(image_t *image, filter_t *filter) {
  if (image == NULL || filter == NULL) {
    fprintf(stderr, "Error: NULL pointer in convolution.\n");
    return NULL;
  }

  int height = image->height;
  int width = image->width;
  int channels = image->channels;
  unsigned char *image_data = image->image_data;

  double *filter_arr = filter->filter_arr;
  int filter_size = filter->size;

  image_t *new_image = create_empty_image(height, width, channels);
  if (!new_image) {
    fprintf(stderr, "Error: Cannot create new image.\n");
    return NULL;
  }
  unsigned char *new_image_data = new_image->image_data;

  int offset = filter_size / 2;

#pragma omp parallel for collapse(2)
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      process_pixel(x, y, width, height, channels, image_data, new_image_data,
                    filter_arr, filter_size, offset, filter->factor,
                    filter->bias);
    }
  }

  return new_image;
}

image_t *parallel_row_convolution(image_t *image, filter_t *filter) {
  if (image == NULL || filter == NULL) {
    fprintf(stderr, "Error: NULL pointer in convolution.\n");

    return NULL;
  }

  int height = image->height;
  int width = image->width;
  int channels = image->channels;
  unsigned char *image_data = image->image_data;

  double *filter_arr = filter->filter_arr;
  int filter_size = filter->size;

  image_t *new_image = create_empty_image(height, width, channels);
  if (!new_image) {
    fprintf(stderr, "Error: Cannot create new image.\n");
    return NULL;
  }
  unsigned char *new_image_data = new_image->image_data;

  int offset = filter_size / 2;

#pragma omp parallel for
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      process_pixel(x, y, width, height, channels, image_data, new_image_data,
                    filter_arr, filter_size, offset, filter->factor,
                    filter->bias);
    }
  }

  return new_image;
}

image_t *parallel_column_convolution(image_t *image, filter_t *filter) {
  if (image == NULL || filter == NULL) {
    fprintf(stderr, "Error: NULL pointer in convolution.\n");
    return NULL;
  }

  int height = image->height;
  int width = image->width;
  int channels = image->channels;
  unsigned char *image_data = image->image_data;

  double *filter_arr = filter->filter_arr;
  int filter_size = filter->size;

  image_t *new_image = create_empty_image(height, width, channels);
  if (!new_image) {
    fprintf(stderr, "Error: Cannot create new image.\n");
    return NULL;
  }
  unsigned char *new_image_data = new_image->image_data;

  int offset = filter_size / 2;

#pragma omp parallel for
  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      process_pixel(x, y, width, height, channels, image_data, new_image_data,
                    filter_arr, filter_size, offset, filter->factor,
                    filter->bias);
    }
  }

  return new_image;
}

image_t *parallel_block_convolution(image_t *image, filter_t *filter) {
  if (image == NULL || filter == NULL) {
    fprintf(stderr, "Error: NULL pointer in convolution.\n");
    return NULL;
  }

  // The block size is now fixed at 64
  int block_size = 64;

  int height = image->height;
  int width = image->width;
  int channels = image->channels;
  unsigned char *image_data = image->image_data;

  double *filter_arr = filter->filter_arr;
  int filter_size = filter->size;

  image_t *new_image = create_empty_image(height, width, channels);
  if (!new_image) {
    fprintf(stderr, "Error: Cannot create new image.\n");
    return NULL;
  }
  unsigned char *new_image_data = new_image->image_data;

  int offset = filter_size / 2;

  int block_width = block_size;
  int block_height = block_size;

  int blocks_x = (width + block_width - 1) / block_width;
  int blocks_y = (height + block_height - 1) / block_height;

#pragma omp parallel for collapse(2)
  for (int by = 0; by < blocks_y; by++) {
    for (int bx = 0; bx < blocks_x; bx++) {
      int start_y = by * block_height;
      int start_x = bx * block_width;
      int end_y =
          (start_y + block_height > height) ? height : start_y + block_height;
      int end_x =
          (start_x + block_width > width) ? width : start_x + block_width;

      for (int y = start_y; y < end_y; y++) {
        for (int x = start_x; x < end_x; x++) {
          process_pixel(x, y, width, height, channels, image_data,
                        new_image_data, filter_arr, filter_size, offset,
                        filter->factor, filter->bias);
        }
      }
    }
  }

  return new_image;
}
