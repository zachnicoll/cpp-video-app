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
  // SwsContext* sws_scaler_ctx;
};

bool load_frame(const char *filename, int *width, int *height, uint8_t **data);
void reshape(GLFWwindow *window, int w, int h);
void render_tex(GLuint *texture_handle, int f_w, int f_h, int xpos, int ypos, int inv_scale);
void init_params();
bool handle_error(int err);

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
