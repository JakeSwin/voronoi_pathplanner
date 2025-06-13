#include "image_data.hpp"

#include <iostream>
#include <raylib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

ImageData load_image(const char *filepath) {
  int width, height, channels;
  unsigned char *image = stbi_load(filepath, &width, &height, &channels, 3);
  std::cout << "Loaded Image: " << filepath << std::endl;

  return ImageData {
    .data = image,
    .filepath = filepath,
    .width = width,
    .height = height,
    .channels = channels
  };
}

void write_image(ImageData out, const char* filepath) {
  stbi_write_png(filepath, out.width, out.height, out.channels, out.data, out.width * out.channels);
}

void delete_image(ImageData img) {
  stbi_image_free(img.data);
  std::cout << "Unloaded Image: " << img.filepath << std::endl;
}
