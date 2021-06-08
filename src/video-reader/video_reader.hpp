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
 * Initialises all contexts for the VideoReader
 * @throws exceptions based on which part of the function fails
 */
void video_reader_open(VideoReader *video_reader, const char *filename);

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
 * Formats and throws a VideoReaderException, with an av_strerror embedded in
 * the exception reason, based on provided err int.
 */
void throw_av_error(const char *pretext, int err);
