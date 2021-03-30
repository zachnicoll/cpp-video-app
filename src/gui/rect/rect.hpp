#pragma once

class Rect : public GUIElement
{
private:
  float x1, y1, x2, y2;
  void (*OnClickFunc)();

public:
  Rect(float _x1, float _y1, float _x2, float _y2, void (*on_click)());
  virtual void Draw();
  virtual bool CheckClicked(float x_pos, float y_pos);
  virtual void OnClick()
  {
    OnClickFunc();
  };
};
