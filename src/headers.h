#pragma once

#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <exception>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#define MAX_QUEUE_LENGTH 50

struct FrameNode
{
  uint8_t *frame_data;
  FrameNode *next_frame;
};

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
  FrameNode *frame_queue;
  int frame_queue_length;
};

struct Rect
{
  float x1, y1, x2, y2;
};

/**
 * Exception called when video_reader_init() fails in some way. Reason
 * for exception can be given in constructor.
 */
class VideoReaderException : public std::exception
{
public:
  const char *reason;

  VideoReaderException(const char *_reason)
  {
    reason = _reason;
  }

  virtual const char *what() const throw()
  {
    char *buff = new char[1024];
    sprintf(buff, "VideoReaderException: %s\n", reason);
    return buff;
  }
};

// TODO: Replace all occurences of this by throwing VideoReaderException
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
void render_tex(GLuint *texture_handle, float f_w, float f_h, float xpos, float ypos, float inv_scale);

/**
 * Initialise some recommended parameters for OpenGL
 */
void init_params();

/**
 * Converts YUV data in video_reader->av_frame->data to RGBA
 * for displaying on an OpenGL texture
 * @returns true if successful
 */
bool yuv_to_rgba(VideoReader *video_reader, uint8_t *data_out);

/**
 * Initialises all contexts for the VideoReader
 * @throws exceptions based on which part of the function fails
 */
void video_reader_open(VideoReader *video_reader, const char *filename);

/**
 * Reads and decodes the next frame in the video file into data_out
 * @returns error code, 0 means success, -1 or other averror code on failure
 **/
int video_reader_next(VideoReader *video_reader);

/**
 * Frees all contexts in the VideoReader
 */
void video_reader_close(VideoReader *video_reader);

/**
 * Allocates a VideoReader struct and fills it with NULL values
 * @returns an allocated VideoReader struct
 */
VideoReader *video_reader_init();

/**
 * Initialise a GLFW window with width, height, and title
 * @returns pointer to window object
 */
GLFWwindow *init_window(int w, int h, const char *title);

// TODO: add docs for these functions

/**
 * Push a new frame data buffer to the supplied video_reader's frame_queue.
 */
void frame_queue_push(VideoReader *video_reader, uint8_t *new_frame);

/**
 * Consume a frame from the supplied video_reader's frame_queue and store
 * it in node_out. The new head of the frame_queue becomes the next FrameNode
 * in the queue. 
 */
void frame_queue_consume(VideoReader *video_reader, FrameNode **node_out);

/**
 * De-allocates the frame data associated with a FrameNode.
 */
void frame_queue_cleanup_node(FrameNode *node);

/**
 * Recursively de-allocates all frame data in an entire frame queue.
 */
void frame_queue_cleanup(FrameNode *node);

/**
 * Function for loading and queueing frames from a VideoReader on a separate
 * thread. Although vid_reader is a void*, it is cast to a VideoReader inside
 * the function.
 */
void *load_frames_thread(void *vid_reader);

/**
 * Wrapper for calling GUI.RenderGUI().
 */
void render_gui(float window_width, float window_height);

/**
 * Allocate and add all GUI elements to the singleton GUI instance.
 */
void init_gui(float window_width, float window_height);

/**
 * Wrapper for calling GUI.GetClickedElement() and running that elements OnClick() function.
 */
Rect* handle_gui_click(float x_pos, float y_pos);

/**
 * Wrapper for calling GUI.DestroyGUI().
 */
void gui_close();
