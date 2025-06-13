#pragma once

#include <raylib.h>

struct ImageData {
  unsigned char* data;
  const char *filepath;
  int width;
  int height;
  int channels;
};

ImageData load_image(const char* filepath);

void write_image(ImageData out, const char* filepath);

void delete_image(ImageData img);
