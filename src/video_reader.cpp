#include "headers.h"

VideoReader * video_reader_init()
{
  VideoReader* video_reader = new VideoReader;
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

bool video_reader_open(VideoReader *video_reader, const char *filename)
{
  int err = 0;

  video_reader->av_format_ctx = avformat_alloc_context();
  if (!video_reader->av_format_ctx)
  {
    printf("Could not allocate AVFormatContext!\n");
    return false;
  }

  // Need to enable all muxers, demuxers, and protocols
  av_register_all();
  avcodec_register_all();

  // Open video file stream and header metadata into av_format_ctx
  if ((err = avformat_open_input(&video_reader->av_format_ctx, filename, NULL, NULL)) != 0)
  {
    printf("Couldn't open video file!\n");
    return handle_error(err);
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
    printf("Could not find valid video stream!\n");
    return false;
  }

  // Set up a codec context for the decoder
  video_reader->av_codec_ctx = avcodec_alloc_context3(video_reader->av_codec);
  if (!video_reader->av_codec_ctx)
  {
    printf("Coudn't create AVCodecContext!\n");
    return false;
  }

  if ((err = avcodec_parameters_to_context(video_reader->av_codec_ctx, video_reader->av_codec_params)) < 0)
  {
    printf("Coudn't init AVCodecContext!\n");
    return handle_error(err);
  }

  if ((err = avcodec_open2(video_reader->av_codec_ctx, video_reader->av_codec, NULL)) < 0)
  {
    printf("Couldn't open codec\n");
    return handle_error(err);
  }

  video_reader->av_frame = av_frame_alloc();
  video_reader->av_packet = av_packet_alloc();

  if (!video_reader->av_frame || !video_reader->av_packet)
  {
    printf("Couldn't allocate AVFrame or AVPacket!\n");
    return false;
  }

  video_reader->width = video_reader->av_codec_ctx->width;
  video_reader->height = video_reader->av_codec_ctx->height;

  return true;
}

int video_reader_next(VideoReader *video_reader)
{
  if (video_reader->frame_queue_length >= MAX_QUEUE_LENGTH) {
    return 0;
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
        printf("Failed to decode packet!\n");
        handle_error(err);
        return -1;
      }

      // Get decoded frame from the decoder
      err = avcodec_receive_frame(video_reader->av_codec_ctx, video_reader->av_frame);
      if (err == AVERROR(EAGAIN) || err == AVERROR_EOF)
      {
        return err;
      }
      else if (err < 0)
      {
        printf("Failed to decode packet!\n");
        handle_error(err);
        return -1;
      }
    }

    // We can allocate SwsContext now that we know the pix_fmt of the frame
    if (!video_reader->sws_scaler_ctx)
    {
      video_reader->sws_scaler_ctx = sws_getContext(
          video_reader->width, video_reader->height, video_reader->av_codec_ctx->pix_fmt, // Input formats
          video_reader->width, video_reader->height, AV_PIX_FMT_RGBA,                    // Output formats
          SWS_BILINEAR,
          NULL, NULL, NULL);
    }

    uint8_t *data = new uint8_t[video_reader->width * video_reader->height * 4];

    // Convert YUV frame colour data to RGBA
    if (!yuv_to_rgba(video_reader, data))
    {
      return -1;
    }

    frame_queue_push(video_reader, data);

    return 0;
  }
  else
  {
    return -1;
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
