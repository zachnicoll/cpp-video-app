#include "../headers.h"
#include "../globals.h"
#include "gui.hpp"
#include "gui-callbacks.hpp"

void reset_colour()
{
  glColor3f(1.0, 1.0, 1.0);
}

void init_gui(float window_width, float window_height)
{
  GUI *gui_controller = GUI::GetInstance();

  Rect *toolbar = new Rect(
      0.0f,
      0.0f,
      window_width,
      30.0f,
      on_toolbar_click);

  Rect *play_button = new Rect(
      window_width / 2 - 25,
      50.0f,
      window_width / 2 + 25,
      100.0f,
      on_play_button_click);

  gui_controller->AddGUIElement(toolbar);
  gui_controller->AddGUIElement(play_button);
}

void render_gui(float window_width, float window_height)
{
  GUI *gui_controller = GUI::GetInstance();

  glColor3f(1.0, 0.0, 0.0);

  gui_controller->RenderGUI();

  reset_colour();
}

void handle_gui_click(float x_pos, float y_pos)
{
  GUI *gui_controller = GUI::GetInstance();
  Rect *clicked_element = (Rect *)gui_controller->GetClickedElement(x_pos, y_pos);

  if (clicked_element != NULL)
  {
    clicked_element->OnClick();
  }
}

void gui_close()
{
  GUI *gui_controller = GUI::GetInstance();
  gui_controller->DestroyGUI();
}
