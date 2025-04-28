#pragma once

#include <memory>
#include <raylib.h>

class ImageLoader {
public:
  static std::shared_ptr<ImageLoader> Create(const char *filepath) {
    return std::shared_ptr<ImageLoader>(new ImageLoader(filepath));
  }
  ~ImageLoader();

  // Accessors
  int Width() const noexcept { return width_; }
  int Height() const noexcept { return height_; }
  const unsigned char *Data() const noexcept { return data_; }
  Texture2D GetTexture() const noexcept { return texture; }

  // Prohibit copying
  ImageLoader(const ImageLoader &) = delete;
  ImageLoader &operator=(const ImageLoader &) = delete;

private:
  ImageLoader(const char *filepath);

  int width_ = 0;
  int height_ = 0;
  unsigned char *data_ = nullptr;
  Texture2D texture;
};
