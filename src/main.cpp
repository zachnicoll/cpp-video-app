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

    Rect *clicked_element = handle_gui_click(xpos, ypos);
    if (clicked_element != NULL) {
      std::cout << "Clicked element with x1,y1,x2,y2: " << clicked_element->x1 << ',' << clicked_element->x2 << ',' << clicked_element->y1 << ',' << clicked_element->y2 << std::endl;
    }
  }
}

void render_loop(GLFWwindow *window, VideoReader *video_reader)
{
  GLuint tex_handle; // Texture handle
  FrameNode *frame_node = NULL; // Current frame node ptr
  float inv_scale = 1.5; // Scale video width and height

  glGenTextures(1, &tex_handle);
  glBindTexture(GL_TEXTURE_2D, tex_handle); // Bind texture handle to GL_TEXTURE_2D, this is the texture GL will use to draw 2D now

  init_params();

  while (!glfwWindowShouldClose(window))
  {
    if (play_video)
    {
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

    // Render GUI
    render_gui((float)fb_w, (float)fb_h);

    float video_tex_width = (float)video_reader->width * 0.8;
    float video_tex_height = (float)video_reader->height * 0.8;

    // Render stuff
    render_tex(
        &tex_handle,
        video_tex_width,
        video_tex_height,
        // Position the texture in the middle of the window
        fb_w / 2 - video_tex_width / (2 * inv_scale),
        125,
        inv_scale);

    // Swap front and back render buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
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

  const int window_width = 1280;
  const int window_height = 720;
  const char *filename = "/home/zach/Desktop/vid3.mp4"; // Change this to load a different file

  // Allocate and initialise window object
  GLFWwindow *window = init_window(window_width, window_height, "CPP | OpenGL | ffmpeg");

  glfwSetKeyCallback(window, key_callback); // Handle key events
  glfwSetMouseButtonCallback(window, mouse_click_callback); // Handle mouse click events

  VideoReader *video_reader = video_reader_init();

  try
  {
    video_reader_open(video_reader, filename);
  }
  catch (std::exception &e)
  {
    printf("An exception occured opening %s:\n%s", filename, e.what());
    return false;
  }

  init_gui(window_width, window_height);

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

  render_loop(window, video_reader);

  gui_close();
  video_reader_close(video_reader);

  return 0;
}
