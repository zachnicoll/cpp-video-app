#include "../headers.h"
#include "gui.hpp"
#include <fstream>

Rect::Rect(float _x1, float _y1, float _x2, float _y2, void (*on_click)())
{
  x1 = _x1;
  y1 = _y1;
  x2 = _x2;
  y2 = _y2;
  OnClickFunc = on_click;
}

void Rect::Draw()
{
  float height = this->y1 - this->y2;

  glColor3f(1.0, 0.0, 0.0);
  glBegin(GL_QUADS);

  // top left
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(this->x1, this->y1, 0.0f);

  // bottom left
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(this->x1, this->y2, 0.0f);

  // bottom right
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(this->x2, this->y2, 0.0f);

  // top right
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(this->x2, this->y1, 0.0f);

  glEnd();
  reset_colour();
}

bool Rect::CheckClicked(float x_pos, float y_pos)
{
  if (
      x_pos > x1 &&
      x_pos < x2 &&
      y_pos > y1 &&
      y_pos < y2)
  {
    return true;
  }

  return false;
}

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

// Shared singleton instance of the GUI class
GUI *GUI::singleton_ = nullptr;

GUI *GUI::GetInstance()
{
  if (singleton_ == nullptr)
  {
    singleton_ = new GUI();
  }
  return singleton_;
}

void GUI::AddGUIElement(GUIElement *element)
{
  gui_elements.push_back(element);
}

GUIElement *GUI::GetClickedElement(float x_pos, float y_pos)
{
  for (GUIElement *element : gui_elements)
  {
    // Check if mouse position is inside rectangle bounds
    if (element->CheckClicked(x_pos, y_pos))
    {
      return element;
    }
  }

  // Nothing was clicked
  return NULL;
}

void GUI::RenderGUI()
{
  for (GUIElement *elem : gui_elements)
  {
    elem->Draw();
  }
}

void GUI::DestroyGUI()
{
  for (GUIElement *elem : gui_elements)
  {
    delete elem;
  }

  gui_elements.erase(gui_elements.begin(), gui_elements.end());
}
