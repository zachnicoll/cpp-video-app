#include <stdio.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <stdint.h>

/**
 * Reshape viewport on window resize 
 */
void reshape( GLFWwindow* window, int w, int h )
{
   glViewport( 0, 0, (GLsizei)w, (GLsizei)h );
}

bool load_frame(const char* filename, int* width, int* height, uint8_t** data);

int main (int argc, const char** argv) {
  GLFWwindow* window;

  if (!glfwInit()) {
    printf("Couldn't initialise GLFW!");
    return 1;
  }

  int window_width = 640;
  int window_height = 480;

  window = glfwCreateWindow(window_width, window_height, "Hello World", NULL, NULL);
  glfwSetFramebufferSizeCallback(window, reshape); // Handle window resize
  glfwMakeContextCurrent(window);

  int frame_width, frame_height;
  uint8_t* frame_data;

  if (!load_frame("/home/zach/Desktop/vid.mp4", &frame_width, &frame_height, &frame_data)) {
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
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame_width, frame_height, 0, GL_RGB, GL_UNSIGNED_BYTE, frame_data);

  // Need these params to render colour
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Method for downscaling image
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Method for upscaling image
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  while (!glfwWindowShouldClose(window)) {
    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int fb_w, fb_h;
    glfwGetFramebufferSize(window, &fb_w, &fb_h);

    // Setup orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, fb_w, 0, fb_h, -1, 1);

    // Set matrix mode to model view so we can start rendering things
    glMatrixMode(GL_MODELVIEW);

    // Render stuff
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex_handle);
    glBegin(GL_QUADS);
      glTexCoord2d(0, 0); glVertex2i(0, 0);
      glTexCoord2d(1, 0); glVertex2i(frame_width, 0);
      glTexCoord2d(1, 1); glVertex2i(frame_width, frame_height);
      glTexCoord2d(0, 1); glVertex2i(0, frame_height);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glfwSwapBuffers(window);
    glfwWaitEvents();
  }

  return 0;
}
