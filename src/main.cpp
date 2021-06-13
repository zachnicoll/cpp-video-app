#include "headers.h"
#include "globals.h"

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (action == GLFW_PRESS) {
    switch (key)
    {
    case GLFW_KEY_SPACE:
      play_video = !play_video;
      break;
    case GLFW_KEY_RIGHT:
      seek_to_time(global_video_reader_handle, play_time + 5.0);
    case GLFW_KEY_LEFT:
      seek_to_time(global_video_reader_handle, play_time - 5.0);
    default:
      break;
    }
  }    
}

void mouse_click_callback(GLFWwindow *window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
  {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    std::cout << "Cursor Position at " << xpos << "," << ypos << std::endl;

    handle_gui_click(xpos, ypos);
  }
}

void render_loop(GLFWwindow *window, int window_width, int window_height, VideoReader *video_reader)
{
  GLuint tex_handle;            // Texture handle
  FrameNode *frame_node = NULL; // Current frame node ptr
  float inv_scale = 1.5;        // Scale video width and height

  glGenTextures(1, &tex_handle);
  glBindTexture(GL_TEXTURE_2D, tex_handle); // Bind texture handle to GL_TEXTURE_2D, this is the texture GL will use to draw 2D now

  init_params();                                      // Initialise OpenGL params
  init_gui(window_width, window_height, &tex_handle); // Allocate and create all GUI objects 

  // Load first frame, after there are frames to load
  while (video_reader->frame_queue_length < 2)
  {
  }
  frame_queue_consume(video_reader, &frame_node);

  glfwSetTime(0.0);
  double last_time = glfwGetTime();

  while (!glfwWindowShouldClose(window))
  {
    // Time since glfwSetTime()
    double current_time = glfwGetTime();

    if (play_video)
    {
      // Add difference in loop execution time to play time
      play_time += current_time - last_time;

      // Only consume the current frame if it should be presented now
      if (play_time >= video_reader->frame_queue->presentation_time)
      {
        frame_queue_consume(video_reader, &frame_node); // Load frame into ptr
      }
    }

    // Keep track of this loop's time for next loop
    last_time = current_time;

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

    float video_tex_width = (float)video_reader->width * 0.5;
    float video_tex_height = (float)video_reader->height * 0.5;

    // Render current video frame
    render_tex(
        &tex_handle,
        video_tex_width,
        video_tex_height,
        window_width / 2 - video_tex_width / 2,
        125);

    // Render all GUI elements
    render_gui((float)fb_w, (float)fb_h);

    // Swap front and back render buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

int main(int argc, const char **argv)
{
  /* SETUP */
  if (!glfwInit())
  {
    printf("Couldn't initialise GLFW!");
    return 1;
  }

  const int window_width = 1280;
  const int window_height = 720;
  const char *filename = "/home/zach/Downloads/vid.mp4"; // Change this to load a different file
  play_video = false;

  // Allocate and initialise window object
  GLFWwindow *window = init_window(window_width, window_height, "CPP | OpenGL | ffmpeg");

  glfwSetKeyCallback(window, key_callback);                 // Handle key events
  glfwSetMouseButtonCallback(window, mouse_click_callback); // Handle mouse click events

  VideoReader *video_reader = video_reader_init();
  global_video_reader_handle = video_reader;

  try
  {
    video_reader_open(video_reader, filename);
  }
  catch (std::exception &e)
  {
    printf("An exception occured opening %s:\n%s", filename, e.what());
    return false;
  }

  // Start loading frames from the video on another thread
  pthread_t frame_loading_thread;
  int rc;
  rc = pthread_create(&frame_loading_thread, NULL, load_frames_thread, video_reader);

  if (rc)
  {
    printf("Failed to create frame-loading thread, aborting!");
    return (-1);
  }
  /*                      */

  /* RENDER GUI AND VIDEO */
  render_loop(window, window_width, window_height, video_reader);
  /*                      */

  /* CLEANUP */
  gui_close();
  video_reader_close(video_reader);
  /*                      */

  return 0;
}
