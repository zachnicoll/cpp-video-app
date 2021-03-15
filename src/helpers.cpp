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
}

void render_tex(GLuint *texture_handle, int f_w, int f_h, float xpos, float ypos, float inv_scale)
{
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, *texture_handle);
  glBegin(GL_QUADS);
  glTexCoord2d(0, 0);
  glVertex2i(xpos, ypos);
  glTexCoord2d(1, 0);
  glVertex2i(xpos + f_w / inv_scale, ypos);
  glTexCoord2d(1, 1);
  glVertex2i(xpos + f_w / inv_scale, ypos + f_h / inv_scale);
  glTexCoord2d(0, 1);
  glVertex2i(xpos, ypos + f_h / inv_scale);
  glEnd();
  glDisable(GL_TEXTURE_2D);
}

bool handle_error(int err)
{
  char *errbuff = new char[1024];
  av_strerror(err, errbuff, 1024);
  printf("av_strerror: %s\n", errbuff);
  return false;
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
  sws_scale(video_reader->sws_scaler_ctx, video_reader->av_frame->data, video_reader->av_frame->linesize, 0, video_reader->height, dest, dest_linesize);

  return true;
}

GLFWwindow* init_window(int w, int h, const char* title) {
    GLFWwindow *window;

    window = glfwCreateWindow(w, h, title, NULL, NULL);
    glfwSetFramebufferSizeCallback(window, reshape); // Handle window resize
    glfwMakeContextCurrent(window);
    return window;
}
