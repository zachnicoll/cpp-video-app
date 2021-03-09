#pragma once

#include <GLFW/glfw3.h>
#include <stdint.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

struct VideoReader
{
  int width;
  int height;
  int video_stream_index;
  AVFormatContext *av_format_ctx;
  AVCodecParameters *av_codec_params;
  AVCodec *av_codec;
  AVCodecContext *av_codec_ctx;
  AVFrame *av_frame;
  AVPacket *av_packet;
  SwsContext *sws_scaler_ctx;
};

bool handle_error(int err);

/**
 * Reshape viewport on window resize 
 */
void reshape(GLFWwindow *window, int w, int h);

/**
 * Render a 2D texture to the screen with given width and height, with bottom left corner at
 * xpos and ypos. An inverse scale can be given to scale the texture, where a larger number
 * will decrease the texture's size. E.g. inv_scale = 2 will halve the textures size.
 */
void render_tex(GLuint *texture_handle, int f_w, int f_h, float xpos, float ypos, float inv_scale);

/**
 * Initialise some recommended parameters for OpenGL
 */
void init_params();

/**
 * Converts YUV data in videoReader->av_frame->data to RGBA
 * for displaying on an OpenGL texture
 * @returns true if successful
 */
bool yuv_to_rgba(VideoReader *videoReader, uint8_t *data_out);

/**
 * Initialises all contexts for the VideoReader
 * @returns true if intialisation succeeds
 */
bool video_reader_open(VideoReader *videoReader, const char *filename);

/**
 * Reads and decodes the next frame in the video file into data_out
 * @returns error code, 0 means success, -1 or other averror code on failure
 **/
int video_reader_next(VideoReader *videoReader, uint8_t **data_out);

/**
 * Frees all contexts in the VideoReader
 */
void video_reader_close(VideoReader *videoReader);

/**
 * Allocates a VideoReader struct and fills it with NULL values
 * @returns an allocated VideoReader struct
 */
VideoReader *video_reader_init();
