#pragma once
#include <vector>

void reset_colour();
void draw_rect(Rect *rect);

class GUI
{
private:
  std::vector<Rect *> gui_elements;

protected:
  GUI()
  {
  }

  static GUI *singleton_;

public:
  /**
   * Singletons should not be cloneable.
   */
  GUI(GUI &other) = delete;
  /**
   * Singletons should not be assignable.
   */
  void operator=(const GUI &) = delete;

  static GUI *GetInstance()
  {
    if (singleton_ == nullptr)
    {
      singleton_ = new GUI();
    }
    return singleton_;
  }

  void AddGUIElement(Rect *element)
  {
    gui_elements.push_back(element);
  }

  Rect *GetClickedElement(float x_pos, float y_pos)
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

  void RenderGUI()
  {
    for (Rect *elem : gui_elements)
    {
      draw_rect(elem);
    }
  }

  void DestroyGUI()
  {
    for (Rect *elem : gui_elements)
    {
      delete elem;
    }

    gui_elements.erase(gui_elements.begin(), gui_elements.end());
  }
};

GUI *GUI::singleton_ = nullptr;
