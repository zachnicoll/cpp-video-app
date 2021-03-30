#include "video_reader.hpp"

void throw_av_error(const char *pretext, int err)
{
  char *errbuff = new char[1024];
  char *msgbuff = new char[2048];
  av_strerror(err, errbuff, 1024);
  sprintf(msgbuff, "%s %s", pretext, errbuff);
  VideoReaderException ex(msgbuff);
  throw ex;
}

VideoReader *video_reader_init()
{
  VideoReader *video_reader = new VideoReader;
  video_reader->av_codec = NULL;
  video_reader->av_codec_ctx = NULL;
  video_reader->av_codec_params = NULL;
  video_reader->av_format_ctx = NULL;
  video_reader->av_frame = NULL;
  video_reader->av_packet = NULL;
  video_reader->sws_scaler_ctx = NULL;
  video_reader->width = 0;
  video_reader->height = 0;
  video_reader->video_stream_index = 0;

  video_reader->frame_queue = new FrameNode;
  video_reader->frame_queue->frame_data = NULL;
  video_reader->frame_queue->next_frame = NULL;

  video_reader->frame_queue_length = 0;

  return video_reader;
}

void video_reader_open(VideoReader *video_reader, const char *filename)
{
  video_reader->av_format_ctx = avformat_alloc_context();
  if (!video_reader->av_format_ctx)
  {
    VideoReaderException ex("Could not allocate AVFormatContext!");
    throw ex;
  }

  // Need to enable all muxers, demuxers, and protocols
  av_register_all();
  avcodec_register_all();

  // Open video file stream and header metadata into av_format_ctx
  if (avformat_open_input(&video_reader->av_format_ctx, filename, NULL, NULL) != 0)
  {
    VideoReaderException ex("Couldn't open video file!");
    throw ex;
  }

  video_reader->video_stream_index = -1;

  // Find video stream that has valid decoder
  for (int i = 0; i < video_reader->av_format_ctx->nb_streams; i++)
  {
    video_reader->av_codec_params = video_reader->av_format_ctx->streams[i]->codecpar; // Codec parameters associated with stream
    video_reader->av_codec = avcodec_find_decoder(video_reader->av_codec_params->codec_id);

    if (!video_reader->av_codec)
    {
      // Keep looking for a stream that has a decoder
      continue;
    }

    // If stream type is a video, we've found the right stream
    if (video_reader->av_codec_params->codec_type == AVMEDIA_TYPE_VIDEO)
    {
      video_reader->video_stream_index = i;
      break;
    }
  }

  if (video_reader->video_stream_index == -1)
  {
    VideoReaderException ex("Could not find valid video stream!");
    throw ex;
  }

  // Set up a codec context for the decoder
  video_reader->av_codec_ctx = avcodec_alloc_context3(video_reader->av_codec);
  if (!video_reader->av_codec_ctx)
  {
    VideoReaderException ex("Coudn't create AVCodecContext!");
    throw ex;
  }

  if (avcodec_parameters_to_context(video_reader->av_codec_ctx, video_reader->av_codec_params) < 0)
  {
    VideoReaderException ex("Coudn't init AVCodecContext!");
    throw ex;
  }

  if (avcodec_open2(video_reader->av_codec_ctx, video_reader->av_codec, NULL) < 0)
  {
    VideoReaderException ex("Couldn't open codec");
    throw ex;
  }

  video_reader->av_frame = av_frame_alloc();
  video_reader->av_packet = av_packet_alloc();

  if (!video_reader->av_frame || !video_reader->av_packet)
  {
    VideoReaderException ex("Couldn't allocate AVFrame or AVPacket!");
    throw ex;
  }

  video_reader->width = video_reader->av_codec_ctx->width;
  video_reader->height = video_reader->av_codec_ctx->height;
}

void video_reader_next(VideoReader *video_reader)
{
  if (video_reader->frame_queue_length >= MAX_QUEUE_LENGTH)
  {
    return;
  }

  int err = 0;

  // Read the next frame from the stream
  if (av_read_frame(video_reader->av_format_ctx, video_reader->av_packet) >= 0)
  {
    if (video_reader->av_packet->stream_index == video_reader->video_stream_index)
    {
      // Supply decoder with raw packet data
      if ((err = avcodec_send_packet(video_reader->av_codec_ctx, video_reader->av_packet)) < 0)
      {
        throw_av_error("Failed to decode packet!", err);
      }

      // Get decoded frame from the decoder
      err = avcodec_receive_frame(video_reader->av_codec_ctx, video_reader->av_frame);
      if (err == AVERROR(EAGAIN) || err == AVERROR_EOF)
      {
        return;
      }
      else if (err < 0)
      {
        throw_av_error("Failed to get decoded frame from the decoder!\n", err);
      }
    }

    // We can allocate SwsContext now that we know the pix_fmt of the frame
    if (!video_reader->sws_scaler_ctx)
    {
      video_reader->sws_scaler_ctx = sws_getContext(
          video_reader->width, video_reader->height, video_reader->av_codec_ctx->pix_fmt, // Input formats
          video_reader->width, video_reader->height, AV_PIX_FMT_RGBA,                     // Output formats
          SWS_BILINEAR,
          NULL, NULL, NULL);
    }

    uint8_t *data = new uint8_t[video_reader->width * video_reader->height * 4];

    // Convert YUV frame colour data to RGBA
    if (!yuv_to_rgba(video_reader, data))
    {
      VideoReaderException ex("Failed to convert YUV frame data to RGBA!");
      throw ex;
    }

    frame_queue_push(video_reader, data);
  }
  else
  {
    VideoReaderException ex("Failed to read frame!");
    throw ex;
  }
}

void video_reader_close(VideoReader *video_reader)
{
  // Free up avformat data
  avformat_close_input(&video_reader->av_format_ctx);
  avformat_free_context(video_reader->av_format_ctx);

  // Free up packet and frame data
  av_frame_free(&video_reader->av_frame);
  av_packet_free(&video_reader->av_packet);

  // Free up avcodec data
  avcodec_free_context(&video_reader->av_codec_ctx);

  if (video_reader->sws_scaler_ctx)
  {
    sws_freeContext(video_reader->sws_scaler_ctx);
  }

  frame_queue_cleanup(video_reader->frame_queue);
}
