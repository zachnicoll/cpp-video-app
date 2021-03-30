#pragma once

class GUIElement
{
protected:
  void (*OnClickFunc)();

public:
  virtual void Draw() = 0;
  virtual bool CheckClicked(float x_pos, float y_pos) = 0;
  virtual void OnClick() = 0;
};
