#include <stdio.h>
#include "headers.h"

int main(int argc, const char **argv)
{
  GLFWwindow *window;

  if (!glfwInit())
  {
    printf("Couldn't initialise GLFW!");
    return 1;
  }

  int window_width = 640;
  int window_height = 480;

  window = glfwCreateWindow(window_width, window_height, "CPP | OpenGL | ffmpeg", NULL, NULL);
  glfwSetFramebufferSizeCallback(window, reshape); // Handle window resize
  glfwMakeContextCurrent(window);

  int frame_width, frame_height;
  uint8_t *frame_data;
  const char *filename = "/home/zach/Desktop/vid.mp4"; // Change this to load a different file

  if (!load_frame(filename, &frame_width, &frame_height, &frame_data))
  {
    printf("Couldn't load video frame!\n");
    return 1;
  }

  // Create texture handle
  GLuint tex_handle;
  glGenTextures(1, &tex_handle);

  // Bind texture handle to GL_TEXTURE_2D,
  // this is the texture GL will use to draw 2D now
  glBindTexture(GL_TEXTURE_2D, tex_handle);

  // Create texture from pixel data
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame_width, frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame_data);

  init_params();

  while (!glfwWindowShouldClose(window))
  {
    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Get current frame buffer size (window size) to correctly project texture
    int fb_w, fb_h;
    glfwGetFramebufferSize(window, &fb_w, &fb_h);

    // Setup orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, fb_w, fb_h, 0, -1, 1);

    // Set matrix mode to model view so we can start rendering things
    glMatrixMode(GL_MODELVIEW);

    // Render stuff
    render_tex(&tex_handle, frame_width, frame_height, 0, 0, 2);

    // Swap front and back render buffers
    glfwSwapBuffers(window);
    glfwWaitEvents();
  }

  return 0;
}
