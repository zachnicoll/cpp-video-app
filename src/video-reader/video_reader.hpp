#include <stdint.h>
#include <stdio.h>
#include <iostream>

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
  double presentation_time;
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
void video_reader_next(VideoReader *video_reader);

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
 * Formats and throws a VideoReaderException, with an av_strerror embedded in
 * the exception reason, based on provided err int.
 */
void throw_av_error(const char *pretext, int err);
