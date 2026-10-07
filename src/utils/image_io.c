#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stdio.h>
#include <string.h>

#include "image_io.h"
#include "stb_image.h"
#include "stb_image_write.h"

image_t *load_image(const char *filename) {
  if (!filename) {
    fprintf(stderr, "Error: NULL filename provided\n");
    return NULL;
  }

  int width, height, channels;
  unsigned char *image_data =
      stbi_load(filename, &width, &height, &channels, 0);
  if (!image_data) {
    fprintf(stderr, "Error: Could not load image '%s': %s\n", filename,
            stbi_failure_reason());
    return NULL;
  }

  if (channels > 4 || channels < 1) {
    fprintf(stderr, "Error: unsupported number of channels (%d)\n", channels);
    stbi_image_free(image_data);
    return NULL;
  }

  image_t *image = (image_t *)malloc(sizeof(image_t));
  if (!image) {
    fprintf(stderr, "Error: memory allocation failed for image structure.\n");
    stbi_image_free(image_data);
    return NULL;
  }

  image->height = height;
  image->width = width;
  image->channels = channels;
  image->image_data = image_data;

  return image;
}

int store_image(const char *filename, image_t *image) {
  if (!filename) {
    fprintf(stderr, "Error: NULL filename provided\n");
    return -1;
  }

  if (!image || !image->image_data) {
    fprintf(stderr, "Error: NULL pointer in image\n");
    return -1;
  }

  const char *dot = strrchr(filename, '.');
  if (!dot) {
    fprintf(stderr, "Error: no file extension in '%s'\n", filename);
    return -1;
  }

  int result = 0;
  const char *ext = dot + 1;

  if (strcmp(ext, "jpg") == 0 || strcmp(ext, "jpeg") == 0) {
    result = stbi_write_jpg(filename, image->width, image->height,
                            image->channels, image->image_data, 95);
  } else if (strcmp(ext, "png") == 0) {
    result =
        stbi_write_png(filename, image->width, image->height, image->channels,
                       image->image_data, image->width * image->channels);
  } else if (strcmp(ext, "bmp") == 0) {
    result = stbi_write_bmp(filename, image->width, image->height,
                            image->channels, image->image_data);
  } else if (strcmp(ext, "tga") == 0) {
    result = stbi_write_tga(filename, image->width, image->height,
                            image->channels, image->image_data);
  } else {
    fprintf(stderr, "Error: unsupported format '%s'\n", ext);
    return -1;
  }

  if (result == 0) {
    fprintf(stderr, "Error: could not save image '%s'\n", filename);
    return -1;
  }
  return 0;
}

image_t *create_empty_image(int height, int width, int channels) {
  size_t image_size = width * height * channels;
  unsigned char *new_image_data =
      (unsigned char *)calloc(image_size, sizeof(unsigned char));
  if (!new_image_data) {
    fprintf(stderr, "Error: memory allocation failed for image data.\n");
    return NULL;
  }

  image_t *new_image = (image_t *)malloc(sizeof(image_t));
  if (!new_image) {
    fprintf(stderr, "Error: memory allocation failed for image structure.\n");
    free(new_image_data);
    return NULL;
  }

  new_image->height = height;
  new_image->width = width;
  new_image->channels = channels;
  new_image->image_data = new_image_data;
  return new_image;
}

char *generate_filename(char *out, size_t out_size, const char *filename,
                        const char *filter_name) {
  if (!out || out_size == 0) {
    fprintf(stderr, "Error: NULL output buffer in generate_filename\n");
    return NULL;
  }

  if (!filename || !filter_name) {
    fprintf(stderr, "Error: NULL pointer in generate_filename\n");
    return NULL;
  }

  const char *dot = strrchr(filename, '.');
  if (!dot) {
    fprintf(stderr, "Error: invalid filename '%s': missing file extension\n",
            filename);
    return NULL;
  }

  size_t base_len = (size_t)(dot - filename);
  size_t filter_len = strlen(filter_name);
  size_t ext_len = strlen(dot);

  if (base_len + 1 + filter_len + ext_len + 1 > out_size) {
    fprintf(stderr, "Error: filename too long\n");
    return NULL;
  }

  char *p = out;
  memcpy(p, filename, base_len);
  p += base_len;
  *p++ = '_';
  memcpy(p, filter_name, filter_len);
  p += filter_len;
  memcpy(p, dot, ext_len);
  p += ext_len;
  *p = '\0';

  return out;
}

void free_image(image_t *image) {
  if (image) {
    if (image->image_data) {
      stbi_image_free(image->image_data);
    }
    free(image);
  }
}
