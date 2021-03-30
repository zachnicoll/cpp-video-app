#include "../gui.hpp"

Texture::Texture(GLuint *_tex_handle, float _pos_x, float _pos_y, float _scale, std::string _filename, void (*on_click)())
{
  pos_x = _pos_x;
  pos_y = _pos_y;
  scale = _scale;
  filename = _filename;
  tex_handle = _tex_handle;
  OnClickFunc = on_click;

  unsigned w, h;

  //decode
  unsigned error = lodepng::decode(image_data, w, h, filename);
  if (error)
  {
    throw new PNGDecodeException(lodepng_error_text(error));
  }

  width = w;
  height = h;

  // Texture size must be power of two for the primitive OpenGL version this is written for. Find next power of two.
  u2 = 1;
  while (u2 < width)
    u2 *= 2;
  v2 = 1;
  while (v2 < height)
    v2 *= 2;
  // Ratio for power of two version compared to actual version, to render the non power of two image with proper size.
  u3 = width / u2;
  v3 = height / v2;

  // Make power of two version of the image.
  std::vector<unsigned char> image2(u2 * v2 * 4);
  for (size_t y = 0; y < height; y++)
    for (size_t x = 0; x < width; x++)
      for (size_t c = 0; c < 4; c++)
      {
        int index = 4 * width * y + 4 * x + c;
        image2[4 * u2 * y + 4 * x + c] = image_data[index];
      }

  image_data = image2;
}

void Texture::Draw()
{
  glTexImage2D(GL_TEXTURE_2D, 0, 4, u2, v2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image_data[0]);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, *tex_handle);
  glBegin(GL_QUADS);
  glTexCoord2d(0, 0);
  glVertex2f(pos_x, pos_y);
  glTexCoord2d(u3, 0);
  glVertex2f(pos_x + width * scale, pos_y);
  glTexCoord2d(u3, v3);
  glVertex2f(pos_x + width * scale, pos_y + height * scale);
  glTexCoord2d(0, v3);
  glVertex2f(pos_x, pos_y + height * scale);
  glEnd();
  glDisable(GL_TEXTURE_2D);
}

bool Texture::CheckClicked(float x_pos, float y_pos)
{
  if (
      x_pos > pos_x &&
      x_pos < pos_x + width * scale &&
      y_pos > pos_y &&
      y_pos < pos_y + height * scale)
  {
    return true;
  }

  return false;
}
