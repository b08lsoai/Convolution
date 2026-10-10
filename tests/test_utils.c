#include <stdarg.h>
#include <stddef.h>

#include "test_utils.h"
#include "utils/filters.h"
#include "utils/image_io.h"

void assert_images_equal_msg(image_t *expected, image_t *actual,
                             const char *msg) {
  assert_non_null(expected);
  assert_non_null(actual);

  if (expected->width != actual->width || expected->height != actual->height ||
      expected->channels != actual->channels) {
    fail_msg("%s: size mismatch: exp %dx%dx%d, got %dx%dx%d", msg,
             expected->width, expected->height, expected->channels,
             actual->width, actual->height, actual->channels);
  }

  const size_t size =
      (size_t)expected->width * expected->height * expected->channels;
  for (size_t i = 0; i < size; ++i) {
    if (expected->image_data[i] != actual->image_data[i]) {
      fail_msg("%s: pixel mismatch at idx %zu: exp %d, got %d", msg, i,
               expected->image_data[i], actual->image_data[i]);
    }
  }
}
