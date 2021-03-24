#include "../headers.h"
#include "../globals.h"
#include "gui.hpp"
#include "gui-callbacks.hpp"

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
    on_click: on_toolbar_click,
  };

  Rect* play_button = new Rect{
    x1 : window_width/2 - 25,
    y1 : 50,
    x2 : window_width/2 + 25,
    y2 : 100,
    on_click: on_play_button_click,
  };

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

void handle_gui_click(float x_pos, float y_pos) {
  GUI* gui_controller = GUI::GetInstance();
  Rect* clicked_element = gui_controller->GetClickedElement(x_pos, y_pos);

  if (clicked_element != NULL)
  {
    clicked_element->on_click();
  }
}

void gui_close()
{
  GUI* gui_controller = GUI::GetInstance();
  gui_controller->DestroyGUI();
}
