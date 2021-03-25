#pragma once
#include <vector>
#include <string>

/**
 * Reset 2D rendering colour to white.
 */
void reset_colour();

class GUIElement
{
protected:
  void (*OnClickFunc)();

public:
  virtual void Draw() = 0;
  virtual bool CheckClicked(float x_pos, float y_pos) = 0;
  virtual void OnClick() = 0;
};

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

class Texture : public GUIElement
{
private:
  float pos_x, pos_y;
  float width, height;
  std::string filename;
  uint8_t *image_data;
  GLuint *tex_handle;
  void (*OnClickFunc)();

public:
  Texture(GLuint *_tex_handle, float _pos_x, float _pos_y, float _width, float _height, std::string _filename, void (*on_click)());
  virtual void Draw();
  virtual bool CheckClicked(float x_pos, float y_pos);
  virtual void OnClick()
  {
    OnClickFunc();
  };
};

/**
 * Singleton class for storing and rendering all GUI elements.
 */
class GUI
{
private:
  std::vector<GUIElement *> gui_elements;

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
  static GUI *GetInstance();

  /**
   * Add element to stored vector of elements.
   */
  void AddGUIElement(GUIElement *element);

  /**
   * Find which GUI element the mouse (x, y) position falls within.
   * @returns pointer to GUI element that was clicked
   */
  GUIElement *GetClickedElement(float x_pos, float y_pos);

  /**
   * Render all stored GUI elements.
   */
  void RenderGUI();

  /**
   * Free all GUI elements and erase all GUI* from the stored vector.
   */
  void DestroyGUI();
};
