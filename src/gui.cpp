#include "headers.h"

struct Rect
{
  float x1, y1, x2, y2;
};

void reset_colour()
{
  glColor3f(1.0, 1.0, 1.0);
}

void draw_rect(Rect rect)
{
  float height = rect.y1 - rect.y2;

  glBegin(GL_QUADS);

  // top left
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(rect.x1, rect.y1, 0.0f);

  // bottom left
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(rect.x1, rect.y2, 0.0f);

  // bottom right
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(rect.x2, rect.y2, 0.0f);

  // top right
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(rect.x2, rect.y1, 0.0f);

  glEnd();
}

void render_gui(float window_width, float window_height)
{
  glColor3f(1.0, 0.0, 0.0);

  draw_rect(Rect {
    x1: 0,
    y1: 0,
    x2: window_width,
    y2: 100,
  });

  reset_colour();
}
