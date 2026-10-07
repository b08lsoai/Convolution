// clang-format off
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmocka.h>

#include "algo/convolution.h"
#include "utils/filters.h"
#include "utils/image_io.h"
#include "utils/parser_args.h"
// clang-format on

#define INPUT_IMAGE "./images/test_image.jpg"
#define INPUT_RGBA_IMAGE "./images/test_RGBA.png"
#define INPUT_GRAISCALE_GRAY_IMAGE "./images/test_Grayscale_gray.png"
#define INPUT_GRAISCALE_LA_IMAGE "./images/test_Grayscale_la.png"

// Checking that parallel modes get the result as sequential
#define TEST_CONVOL(convol_mode)                                               \
  image_t *image = ctx->input_image;                                           \
  filter_t *filter = ctx->filter;                                              \
  ctx->seq_result = seq_convolution(image, filter);                            \
  ctx->parallel_result = convol_mode(image, filter);                           \
  assert_images_equal(ctx->seq_result, ctx->parallel_result);

typedef struct {
  image_t *input_image;
  filter_t *filter;
  image_t *seq_result;
  image_t *parallel_result;
} state_t;

static void assert_images_equal(image_t *expected, image_t *actual) {
  assert_non_null(expected);
  assert_non_null(actual);

  assert_int_equal(expected->width, actual->width);
  assert_int_equal(expected->height, actual->height);
  assert_int_equal(expected->channels, actual->channels);

  int size = expected->width * expected->height * expected->channels;

  for (int i = 0; i < size; i++) {
    assert_int_equal(expected->image_data[i], actual->image_data[i]);
  }
}

static int setup(void **state) {
  state_t *ctx = malloc(sizeof(state_t));
  if (!ctx) {
    return -1;
  }

  ctx->input_image = load_image(INPUT_IMAGE);
  if (!ctx->input_image) {
    free(ctx);
    return -1;
  }
  ctx->filter = create_filter(MOTION_BLUR);
  if (!ctx->filter) {
    free_image(ctx->input_image);
    free(ctx);
    return -1;
  }
  ctx->seq_result = NULL;
  ctx->parallel_result = NULL;
  *state = ctx;
  return 0;
}

static int teardown(void **state) {
  state_t *ctx = *state;
  if (ctx) {
    free_filter(ctx->filter);
    free_image(ctx->input_image);
    free_image(ctx->seq_result);
    free_image(ctx->parallel_result);
    free(ctx);
  }
  return 0;
}

static void test_pixel_convolution(void **state) {
  state_t *ctx = *state;
  TEST_CONVOL(parallel_pixel_convolution);
}

static void test_row_convolution(void **state) {
  state_t *ctx = *state;
  TEST_CONVOL(parallel_row_convolution);
}

static void test_column_convolution(void **state) {
  state_t *ctx = *state;
  TEST_CONVOL(parallel_column_convolution);
}

static void test_block_convolution(void **state) {
  state_t *ctx = *state;
  TEST_CONVOL(parallel_block_convolution);
}

static void test_rgba_image_convolution(void **state) {
  (void)state;
  image_t *input_image = load_image(INPUT_RGBA_IMAGE);
  assert_non_null(input_image);
  assert_int_equal(input_image->channels, 4);
  filter_t *filter = create_filter(MOTION_BLUR);
  assert_non_null(filter);
  image_t *seq_res = seq_convolution(input_image, filter);
  assert_non_null(seq_res);
  image_t *pix_res = parallel_pixel_convolution(input_image, filter);
  image_t *row_res = parallel_row_convolution(input_image, filter);
  image_t *col_res = parallel_column_convolution(input_image, filter);
  image_t *blk_res = parallel_block_convolution(input_image, filter);
  assert_images_equal(seq_res, pix_res);
  assert_images_equal(seq_res, row_res);
  assert_images_equal(seq_res, col_res);
  assert_images_equal(seq_res, blk_res);
  free_image(seq_res);
  free_image(pix_res);
  free_image(row_res);
  free_image(col_res);
  free_image(blk_res);
  free_filter(filter);
  free_image(input_image);
}

static void test_gray_image_convolution(void **state) {
  (void)state;
  image_t *input_image = load_image(INPUT_GRAISCALE_GRAY_IMAGE);
  assert_non_null(input_image);
  assert_int_equal(input_image->channels, 1);
}

static void test_la_image_convolution(void **state) {
  (void)state;
  image_t *input_image = load_image(INPUT_GRAISCALE_LA_IMAGE);
  assert_non_null(input_image);
  assert_int_equal(input_image->channels, 2);
  filter_t *filter = create_filter(EMBOSS);
  assert_non_null(filter);
  image_t *seq_res = seq_convolution(input_image, filter);
  assert_non_null(seq_res);
  image_t *pix_res = parallel_pixel_convolution(input_image, filter);
  image_t *row_res = parallel_row_convolution(input_image, filter);
  image_t *col_res = parallel_column_convolution(input_image, filter);
  image_t *blk_res = parallel_block_convolution(input_image, filter);
  assert_images_equal(seq_res, pix_res);
  assert_images_equal(seq_res, row_res);
  assert_images_equal(seq_res, col_res);
  assert_images_equal(seq_res, blk_res);
  free_image(seq_res);
  free_image(pix_res);
  free_image(row_res);
  free_image(col_res);
  free_image(blk_res);
  free_filter(filter);
  free_image(input_image);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test_setup_teardown(test_pixel_convolution, setup, teardown),
      cmocka_unit_test_setup_teardown(test_row_convolution, setup, teardown),
      cmocka_unit_test_setup_teardown(test_column_convolution, setup, teardown),
      cmocka_unit_test_setup_teardown(test_block_convolution, setup, teardown),
      cmocka_unit_test(test_rgba_image_convolution),
      cmocka_unit_test(test_gray_image_convolution),
      cmocka_unit_test(test_la_image_convolution),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
