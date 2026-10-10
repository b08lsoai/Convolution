#ifndef TESTS_UTILS_H
#define TESTS_UTILS_H

#define IMG_DIR "./images/"

#define IMG_RGB IMG_DIR "test_image.jpg"
#define IMG_RGBA IMG_DIR "test_RGBA.png"
#define IMG_GRAY IMG_DIR "test_Grayscale_gray.png"
#define IMG_LA IMG_DIR "test_Grayscale_la.png"

#define assert_images_equal(e, a) assert_images_equal_msg((e), (a), #a)

#endif