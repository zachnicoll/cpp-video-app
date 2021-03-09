#include "headers.h"

/**
 * Reshape viewport on window resize 
 */
void reshape(GLFWwindow *window, int w, int h)
{
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

/**
 * Initialise some recommended parameters for OpenGL
 */
void init_params()
{
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Method for downscaling image
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Method for upscaling image
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

/**
 * Render a 2D texture to the screen with given width and height, with bottom left corner at
 * xpos and ypos. An inverse scale can be given to scale the texture, where a larger number
 * will decrease the texture's size. E.g. inv_scale = 2 will halve the textures size.
 */
void render_tex(GLuint *texture_handle, int f_w, int f_h, int xpos, int ypos, int inv_scale)
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

bool yuv_to_rgba(VideoReader *videoReader, uint8_t *data_out)
{
  SwsContext *temp_sws_ctx = sws_getContext(
      videoReader->width, videoReader->height, videoReader->av_codec_ctx->pix_fmt, // Input formats
      videoReader->width, videoReader->height, AV_PIX_FMT_RGBA,                    // Output formats
      SWS_BILINEAR,
      NULL, NULL, NULL);

  if (!temp_sws_ctx)
  {
    printf("Couldn't get SwsContext!\n");
    return false;
  }

  uint8_t *dest[4] = {data_out, 0, 0, 0};
  int dest_linesize[4] = {videoReader->width * 4, 0, 0, 0};
  sws_scale(temp_sws_ctx, videoReader->av_frame->data, videoReader->av_frame->linesize, 0, videoReader->height, dest, dest_linesize);

  sws_freeContext(temp_sws_ctx);

  return true;
}
