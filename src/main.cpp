#include <stdio.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <stdint.h>

int main (int argc, const char** argv) {
  GLFWwindow* window;

  if (!glfwInit()) {
    printf("Couldn't initialise GLFW!");
    return 1;
  }

  window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
  glfwMakeContextCurrent(window);

  uint8_t* data = new uint8_t[100 * 100 * 3];
  for (int y = 0; y < 100; ++y) {
    for (int x = 0; x < 100; ++x) {
      data[y * 100 * 3 + x * 3] = 0xff;
      data[y * 100 * 3 + x * 3 + 1] = 0x00;
      data[y * 100 * 3 + x * 3 + 2] = 0x00;
    }
  }

  while (!glfwWindowShouldClose(window)) {
    // Rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawPixels(100, 100, GL_RGB, GL_UNSIGNED_BYTE, data);

    glfwSwapBuffers(window);
    glfwWaitEvents();
  }

  return 0;
}
