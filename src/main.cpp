#include "headers.h"

/* GLOBALS */
bool play_video = false;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    play_video = !play_video;
}

void mouse_click_callback(GLFWwindow *window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
  {
    double xpos, ypos;
    //getting cursor position
    glfwGetCursorPos(window, &xpos, &ypos);
    std::cout << "Cursor Position at " << xpos << "," << ypos << std::endl;
  }
}

int main(int argc, const char **argv)
{
  /* SETUP START */
  if (!glfwInit())
  {
    printf("Couldn't initialise GLFW!");
    return 1;
  }

  // Allocate and initialise window object
  GLFWwindow *window = init_window(1280, 720, "CPP | OpenGL | ffmpeg");

  // Handle key events
  glfwSetKeyCallback(window, key_callback);

  glfwSetMouseButtonCallback(window, mouse_click_callback);

  FrameNode *frame_node = NULL;
  const char *filename = "/home/zach/Desktop/vixd.mp4"; // Change this to load a different file

  VideoReader *video_reader = video_reader_init();

  try {
    video_reader_open(video_reader, filename);
  } catch (std::exception& e) {
    printf("An exception occured opening %s:\n%s", filename, e.what());
    return false;
  }

  // Create texture handle
  GLuint tex_handle;
  glGenTextures(1, &tex_handle);

  // Bind texture handle to GL_TEXTURE_2D,
  // this is the texture GL will use to draw 2D now
  glBindTexture(GL_TEXTURE_2D, tex_handle);

  init_params();

  // Start loading frames from the video on another thread
  pthread_t frame_loading_thread;
  int rc;
  rc = pthread_create(&frame_loading_thread, NULL, load_frames_thread, video_reader);

  if (rc)
  {
    printf("Failed to create frame-loading thread, aborting!");
    return (-1);
  }

  /* SETUP END */

  int err = 0;
  float inv_scale = 1.5;

  while (!glfwWindowShouldClose(window))
  {
    if (play_video)
    {
      /**
     * If you don't do this, the previous frame's data will not be deallocated
     * and you will lock up your PC
     */
      if (frame_node != NULL)
      {
        frame_queue_cleanup_node(frame_node);
      }

      frame_queue_consume(video_reader, &frame_node);
    }

    if (frame_node != NULL)
    {
      // Create texture from pixel data
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, video_reader->width, video_reader->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame_node->frame_data);
    }

    // Clear screen
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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
        fb_w / 2 - video_reader->width / (2 * inv_scale),
        fb_h / 2 - video_reader->height / (2 * inv_scale),
        inv_scale);

    /* Drawing a line, use for gui later
    glPointSize(10);
    glLineWidth(2.5);
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(10.0, 10.0, 0.0);
    glVertex3f(50.0, 20.0, 0.0);
    glEnd();
    glColor3f(1.0, 1.0, 1.0);
    */

    // Swap front and back render buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  video_reader_close(video_reader);

  return 0;
}
