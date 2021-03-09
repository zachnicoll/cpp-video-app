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

  int window_width = 1280;
  int window_height = 720;

  window = glfwCreateWindow(window_width, window_height, "CPP | OpenGL | ffmpeg", NULL, NULL);
  glfwSetFramebufferSizeCallback(window, reshape); // Handle window resize
  glfwMakeContextCurrent(window);

  uint8_t *frame_data = NULL;
  const char *filename = "/home/zach/Desktop/vid3.mp4"; // Change this to load a different file

  VideoReader *video_reader = video_reader_init();

  if (!video_reader_open(video_reader, filename))
  {
    printf("Failed to initialise VideoReader!");
    return false;
  }

  // Create texture handle
  GLuint tex_handle;
  glGenTextures(1, &tex_handle);

  // Bind texture handle to GL_TEXTURE_2D,
  // this is the texture GL will use to draw 2D now
  glBindTexture(GL_TEXTURE_2D, tex_handle);

  init_params();

  int err = 0;
  float inv_scale = 1.5;

  while (!glfwWindowShouldClose(window))
  {
    /**
     * If you don't do this, the previous frame's data will not be deallocated
     * and you will lock up your PC
     */
    if (frame_data) {
      free(frame_data);
    }

    err = video_reader_next(video_reader, &frame_data);

    if (err == AVERROR_EOF || (err != AVERROR(EAGAIN) && err < 0))
    {
      break;
    }

    // Create texture from pixel data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, video_reader->width, video_reader->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame_data);

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
    render_tex(
        &tex_handle,
        video_reader->width,
        video_reader->height,
        // Position the texture in the middle of the window
        fb_w / 2 - video_reader->width / (2*inv_scale),
        fb_h / 2 - video_reader->height / (2*inv_scale),
        inv_scale);

    // Swap front and back render buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  video_reader_close(video_reader);

  return 0;
}
