#include "../headers.h"
#include "gui.hpp"

void reset_colour()
{
  glColor3f(1.0, 1.0, 1.0);
}

void draw_rect(Rect *rect)
{
  float height = rect->y1 - rect->y2;

  glBegin(GL_QUADS);

  // top left
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(rect->x1, rect->y1, 0.0f);

  // bottom left
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(rect->x1, rect->y2, 0.0f);

  // bottom right
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(rect->x2, rect->y2, 0.0f);

  // top right
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(rect->x2, rect->y1, 0.0f);

  glEnd();
}

void init_gui(float window_width, float window_height)
{
  GUI* gui_controller = GUI::GetInstance();

  Rect* toolbar = new Rect{
    x1 : 0,
    y1 : 0,
    x2 : window_width,
    y2 : 30,
  };

  Rect* play_button = new Rect{
    x1 : 50,
    y1 : 35,
    x2 : 100,
    y2 : 65
  };

  for (int i = 0; i < 100; i++) {
    Rect* dummy_elem = new Rect{
      x1 : 10.0f + i,
      y1 : 10.0f,
      x2 : 50.0f + i,
      y2 : 50.0f
    };

    gui_controller->AddGUIElement(dummy_elem);
  }

  gui_controller->AddGUIElement(toolbar);
  gui_controller->AddGUIElement(play_button);
}

void render_gui(float window_width, float window_height)
{
  GUI* gui_controller = GUI::GetInstance();

  glColor3f(1.0, 0.0, 0.0);

  gui_controller->RenderGUI();

  reset_colour();
}

Rect* handle_gui_click(float x_pos, float y_pos) {
  GUI* gui_controller = GUI::GetInstance();
  return gui_controller->GetClickedElement(x_pos, y_pos);
  /**
   * TODO: Call a function assigned to the element on click
   */
}

void gui_close()
{
  GUI* gui_controller = GUI::GetInstance();
  gui_controller->DestroyGUI();
}
