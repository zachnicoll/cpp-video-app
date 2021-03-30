#include "../gui.hpp"

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
