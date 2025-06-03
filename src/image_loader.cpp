#include "image_loader.hpp"

#include <raylib.h>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

ImageLoader::ImageLoader(const char *filepath) {
  int channels_in_file;
  data_ = stbi_load(filepath, &width_, &height_, &channels_in_file, 3);
  texture = LoadTexture(filepath);

  if (!data_) {
    throw std::runtime_error("Failed to load image");
  }
}

ImageLoader::~ImageLoader() {
  if (data_) {
    stbi_image_free(data_);
    UnloadTexture(texture);
  }
}
