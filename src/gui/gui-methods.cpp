#include "../headers.h"
#include "gui.hpp"

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

void GUI::AddGUIElement(Rect *element)
{
  gui_elements.push_back(element);
}

Rect *GUI::GetClickedElement(float x_pos, float y_pos)
{
  for (Rect *element : gui_elements)
  {
    // Check if mouse position is inside rectangle bounds
    if (
        x_pos > element->x1 &&
        x_pos < element->x2 &&
        y_pos > element->y1 &&
        y_pos < element->y2)
    {
      return element;
    }
  }

  // Nothing was clicked
  return NULL;
}

void GUI::RenderGUI()
{
  for (Rect *elem : gui_elements)
  {
    draw_rect(elem);
  }
}

void GUI::DestroyGUI()
{
  for (Rect *elem : gui_elements)
  {
    delete elem;
  }

  gui_elements.erase(gui_elements.begin(), gui_elements.end());
}
