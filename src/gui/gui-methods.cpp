#include "../headers.h"
#include "gui.hpp"

void Rect::Draw()
{
  float height = this->y1 - this->y2;

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
