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

Texture::Texture(GLuint *_tex_handle, float _pos_x, float _pos_y, float _width, float _height, std::string _filename, void (*on_click)())
{
  pos_x = _pos_x;
  pos_y = _pos_y;
  width = _width;
  height = _height;
  filename = _filename;
  tex_handle = _tex_handle;
  OnClickFunc = on_click;

  std::ifstream file;
  file.open(filename);

  if (!file.is_open())
  {
    throw new std::exception();
  }

  file.close();
}

void Texture::Draw()
{
  int _width = (int)width;
  int _height = (int)height;

  uint8_t data[_width * _height * 3];

  for (int x = 0; x < _width; x++)
  {
    for (int y = 0; y < _height; y++)
    {
      data[x * _width * 3 + y * 3] = 0x00;
      data[x * _width * 3 + y * 3 + 1] = 0xff;
      data[x * _width * 3 + y * 3 + 2] = 0xff;
    }
  }

  // // TODO: change GL_RGB to GL_RGBA
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

  render_tex(
      tex_handle,
      width,
      height,
      pos_x,
      pos_y,
      1);

  // free(data);
}

bool Texture::CheckClicked(float x_pos, float y_pos)
{
  if (
      x_pos > pos_x &&
      x_pos < pos_x + width &&
      y_pos > pos_y &&
      y_pos < pos_y + height)
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
