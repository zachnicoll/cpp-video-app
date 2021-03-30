#include "headers.h"

void reshape(GLFWwindow *window, int w, int h)
{
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

void init_params()
{
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Method for downscaling image
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Method for upscaling image
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_ALPHA_TEST);
}

void render_tex(GLuint *texture_handle, float f_w, float f_h, float xpos, float ypos)
{
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, *texture_handle);
  glBegin(GL_QUADS);
  glTexCoord2d(0, 0);
  glVertex2i(xpos, ypos);
  glTexCoord2d(1, 0);
  glVertex2i(xpos + f_w, ypos);
  glTexCoord2d(1, 1);
  glVertex2i(xpos + f_w, ypos + f_h);
  glTexCoord2d(0, 1);
  glVertex2i(xpos, ypos + f_h);
  glEnd();
  glDisable(GL_TEXTURE_2D);
}

bool yuv_to_rgba(VideoReader *video_reader, uint8_t *data_out)
{
  if (!video_reader->sws_scaler_ctx)
  {
    printf("Couldn't get SwsContext!\n");
    return false;
  }

  uint8_t *dest[4] = {data_out, 0, 0, 0};
  int dest_linesize[4] = {video_reader->width * 4, 0, 0, 0};
  sws_scale(video_reader->sws_scaler_ctx,
            video_reader->av_frame->data,
            video_reader->av_frame->linesize,
            0,
            video_reader->height,
            dest,
            dest_linesize);

  return true;
}

GLFWwindow *init_window(int w, int h, const char *title)
{
  GLFWwindow *window;

  window = glfwCreateWindow(w, h, title, NULL, NULL);
  glfwSetFramebufferSizeCallback(window, reshape); // Handle window resize
  glfwMakeContextCurrent(window);
  return window;
}

void *load_frames_thread(void *vid_reader)
{
  VideoReader *video_reader;
  video_reader = (VideoReader *)vid_reader;

  while (true)
  {
    try
    {
      video_reader_next(video_reader);
    }
    catch (VideoReaderException e)
    {
      printf("%s", e.what());
      printf("FAILED TO READ NEXT FRAME FROM VIDEO READER, CLOSING THREAD!\n");
      break;
    }
  }

  pthread_exit(NULL);
}
