#pragma once

#include <GLFW/glfw3.h>
#include <stdint.h>

bool load_frame(const char *filename, int *width, int *height, uint8_t **data);
void reshape(GLFWwindow *window, int w, int h);
void render_tex(GLuint *texture_handle, int f_w, int f_h, int xpos, int ypos, int inv_scale);
void init_params();
