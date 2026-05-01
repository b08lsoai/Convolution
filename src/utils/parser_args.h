#pragma once

#include <stdbool.h>

typedef enum {
  INVALID = -1,
  BLUR,
  MOTION_BLUR,
  GAUSSIAN_BLUR,
  FIND_EDGES,
  EMBOSS,
  SHARPEN,
} filter_name;

typedef enum {
  MODE_INVALID = -1,
  MODE_SEQ,
  MODE_PIXEL,
  MODE_ROW,
  MODE_COLUMN,
  MODE_BLOCK,
} conv_mode;

typedef struct {
  char *input_filename;
  conv_mode mode;
  filter_name filter;
  bool help_flag;
} args_t;

int parse_args(int argc, char *argv[], args_t *args);
