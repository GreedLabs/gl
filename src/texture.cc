#include "texture.hh"

#include <epoxy/gl.h>
#include <png.h>
#include <string.h>

unsigned Texture::count = 0;

Texture::Texture(unsigned id, unsigned unit): GlObject(id), unit(unit)
{}

TexturePtr Texture::fromFile(const char *filename,
                             int min_mag_filter, int wrap_mode) {
  png_image img;
  memset(&img, 0, sizeof(img));
  img.version = PNG_IMAGE_VERSION;

  png_image_begin_read_from_file(&img, filename);

  img.format = PNG_FORMAT_RGBA;

  char *buffer = new char[PNG_IMAGE_SIZE(img)];

  png_image_finish_read(&img, nullptr, (void *)buffer, 0, nullptr);

  unsigned tex_id;

  glGenTextures(1, &tex_id);

  glBindTexture(GL_TEXTURE_2D, tex_id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_mag_filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, min_mag_filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode);
  glTexImage2D(GL_TEXTURE_2D,
      0, GL_RGBA, img.width, img.height,
      0, GL_RGBA, GL_UNSIGNED_BYTE, (void *)buffer);

  glBindTexture(GL_TEXTURE_2D, 0);

  png_image_free(&img);

  delete[] buffer;

  return TexturePtr(new Texture(tex_id, Texture::count++));
}
