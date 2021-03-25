#include "../headers.h"
#include "../globals.h"
#include "gui-callbacks.hpp"

void on_toolbar_click()
{
  std::cout << "TOOLBAR CLICK" << std::endl;
}

void on_play_button_click()
{
  std::cout << "PLAY BUTTON CLICK" << std::endl;
  play_video = !play_video;
}

void on_tex_click()
{
  std::cout << "TEXTURE CLICK" << std::endl;
}
