#include <stdio.h>

#include "convolution.h"

#define FREE                                                                   \
  free_filter(filter);                                                         \
  free_image(image);                                                           \
  free_image(new_image)

int process_image(args_t *args) {
  if (!args) {
    fprintf(stderr, "Error: args are NULL\n");
    return -1;
  }

  filter_t *filter = create_filter(args->filter);
  if (!filter) {
    fprintf(stderr, "Error: failed to create filter\n");
    return -1;
  }

  // Images should be placed in the “./images” folder
  char file_path[512];
  snprintf(file_path, sizeof(file_path), "./images/%s", args->input_filename);

  image_t *image = load_image(file_path);
  if (!image) {
    fprintf(stderr, "Error: failed to load image '%s' in ./images\n",
            args->input_filename);
    free_filter(filter);
    return -1;
  }

  image_t *new_image = NULL;
  switch (args->mode) {
  case MODE_SEQ:
    new_image = seq_convolution(image, filter);
    break;

  case MODE_ROW:
    new_image = parallel_row_convolution(image, filter);
    break;

  case MODE_PIXEL:
    new_image = parallel_pixel_convolution(image, filter);
    break;

  case MODE_COLUMN:
    new_image = parallel_column_convolution(image, filter);
    break;

  case MODE_BLOCK:
    new_image = parallel_block_convolution(image, filter);
    break;

  default:
    fprintf(stderr, "Error: unknown mode %d\n", args->mode);
    FREE;
    return -1;
  }

  if (!new_image) {
    fprintf(stderr, "Error: convolution failed\n");
    FREE;
    return -1;
  }

  const char *new_filename = generate_filename(file_path, filter->name);
  if (!new_filename) {
    fprintf(stderr, "Error: failed to generate output filename\n");
    FREE;
    return -1;
  }

  int ret = store_image(new_filename, new_image);
  if (ret < 0) {
    fprintf(stderr, "Error: failed to save output filename\n");
    FREE;
    return -1;
  }

  FREE;
  return 0;
}
