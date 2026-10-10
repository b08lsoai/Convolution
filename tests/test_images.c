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
#include "test_utils.h"
// clang-format on

static void test_rgba_image_convolution(void **state) {
  (void)state;

  image_t *input_image = load_image(IMG_RGBA);
  assert_non_null(input_image);
  assert_int_equal(input_image->channels, 4);

  filter_t *filter = create_filter(MOTION_BLUR);
  assert_non_null(filter);

  image_t *seq_res = seq_convolution(input_image, filter);
  assert_non_null(seq_res);
  assert_int_equal(seq_res->channels, 4);

  free_image(seq_res);
  free_filter(filter);
  free_image(input_image);
}

static void test_gray_image_convolution(void **state) {
  (void)state;

  image_t *input_image = load_image(IMG_GRAY);
  assert_non_null(input_image);
  assert_int_equal(input_image->channels, 1);

  filter_t *filter = create_filter(MOTION_BLUR);
  assert_non_null(filter);

  image_t *seq_res = seq_convolution(input_image, filter);
  assert_non_null(seq_res);
  assert_int_equal(seq_res->channels, 1);

  free_image(seq_res);
  free_filter(filter);
  free_image(input_image);
}

static void test_la_image_convolution(void **state) {
  (void)state;

  image_t *input_image = load_image(IMG_LA);
  assert_non_null(input_image);
  assert_int_equal(input_image->channels, 2);

  filter_t *filter = create_filter(MOTION_BLUR);
  assert_non_null(filter);

  image_t *seq_res = seq_convolution(input_image, filter);
  assert_non_null(seq_res);
  assert_int_equal(seq_res->channels, 2);

  free_image(seq_res);
  free_filter(filter);
  free_image(input_image);
}

static void test_rgb_image_convolution(void **state) {
  (void)state;

  image_t *input_image = load_image(IMG_RGB);
  assert_non_null(input_image);
  assert_int_equal(input_image->channels, 3);

  filter_t *filter = create_filter(MOTION_BLUR);
  assert_non_null(filter);

  image_t *seq_res = seq_convolution(input_image, filter);
  assert_non_null(seq_res);
  assert_int_equal(seq_res->channels, 3);

  free_image(seq_res);
  free_filter(filter);
  free_image(input_image);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_rgba_image_convolution),
      cmocka_unit_test(test_rgb_image_convolution),
      cmocka_unit_test(test_gray_image_convolution),
      cmocka_unit_test(test_la_image_convolution),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
