#pragma once

class Texture : public GUIElement
{
private:
  float pos_x, pos_y, scale;
  float width, height;
  size_t u2, v2;
  double u3, v3;
  std::string filename;
  std::vector<unsigned char> image_data;
  GLuint *tex_handle;
  void (*OnClickFunc)();

public:
  Texture(GLuint *_tex_handle, float _pos_x, float _pos_y, float _scale, std::string _filename, void (*on_click)());
  virtual void Draw();
  virtual bool CheckClicked(float x_pos, float y_pos);
  virtual void OnClick()
  {
    OnClickFunc();
  };
};

class PNGDecodeException : public std::exception
{
public:
  const char *reason;

  PNGDecodeException(const char *_reason)
  {
    reason = _reason;
  }

  virtual const char *what() const throw()
  {
    char *buff = new char[1024];
    sprintf(buff, "PNGDecodeException: %s\n", reason);
    return buff;
  }
};
