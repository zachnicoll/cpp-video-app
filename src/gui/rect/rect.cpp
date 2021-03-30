#include "../gui.hpp"

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