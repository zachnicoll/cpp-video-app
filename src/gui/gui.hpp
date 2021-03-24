#pragma once
#include <vector>

/**
 * Reset 2D rendering colour to white.
 */
void reset_colour();

/**
 * Draw a rectangle, from top left corner to bottom right corner.
 */
void draw_rect(Rect *rect);

/**
 * Singleton class for storing and rendering all GUI elements.
 */
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

  /**
   * Returns singleton instance of the GUI object, creates a new
   * object if one does not already exist.
   * @returns pointer to shared GUI object instance
   */
  static GUI *GetInstance()
  {
    if (singleton_ == nullptr)
    {
      singleton_ = new GUI();
    }
    return singleton_;
  }

  /**
   * Add element to stored vector of elements.
   */
  void AddGUIElement(Rect *element)
  {
    gui_elements.push_back(element);
  }

  /**
   * Find which GUI element the mouse (x, y) position falls within.
   * @returns pointer to GUI element that was clicked
   */
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

  /**
   * Render all stored GUI elements.
   */
  void RenderGUI()
  {
    for (Rect *elem : gui_elements)
    {
      draw_rect(elem);
    }
  }

  /**
   * Free all GUI elements and erase all GUI* from the stored vector.
   */
  void DestroyGUI()
  {
    for (Rect *elem : gui_elements)
    {
      delete elem;
    }

    gui_elements.erase(gui_elements.begin(), gui_elements.end());
  }
};

// Shared singleton instance of the GUI class
GUI *GUI::singleton_ = nullptr;
