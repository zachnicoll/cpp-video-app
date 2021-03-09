#include "headers.h"

bool video_reader_open(VideoReader *videoReader, const char *filename)
{
  int err = 0;

  videoReader->av_format_ctx = avformat_alloc_context();
  if (!videoReader->av_format_ctx)
  {
    printf("Could not allocate AVFormatContext!\n");
    return false;
  }

  // Need to enable all muxers, demuxers, and protocols
  av_register_all();
  avcodec_register_all();

  // Open video file stream and header metadata into av_format_ctx
  if ((err = avformat_open_input(&videoReader->av_format_ctx, filename, NULL, NULL)) != 0)
  {
    printf("Couldn't open video file!\n");
    return handle_error(err);
  }

  videoReader->video_stream_index = -1;

  // Find video stream that has valid decoder
  for (int i = 0; i < videoReader->av_format_ctx->nb_streams; i++)
  {
    videoReader->av_codec_params = videoReader->av_format_ctx->streams[i]->codecpar; // Codec parameters associated with stream
    videoReader->av_codec = avcodec_find_decoder(videoReader->av_codec_params->codec_id);

    if (!videoReader->av_codec)
    {
      // Keep looking for a stream that has a decoder
      continue;
    }

    // If stream type is a video, we've found the right stream
    if (videoReader->av_codec_params->codec_type == AVMEDIA_TYPE_VIDEO)
    {
      videoReader->video_stream_index = i;
      break;
    }
  }

  if (videoReader->video_stream_index == -1)
  {
    printf("Could not find valid video stream!\n");
    return false;
  }

  // Set up a codec context for the decoder
  videoReader->av_codec_ctx = avcodec_alloc_context3(videoReader->av_codec);
  if (!videoReader->av_codec_ctx)
  {
    printf("Coudn't create AVCodecContext!\n");
    return false;
  }

  if ((err = avcodec_parameters_to_context(videoReader->av_codec_ctx, videoReader->av_codec_params)) < 0)
  {
    printf("Coudn't init AVCodecContext!\n");
    return handle_error(err);
  }

  if ((err = avcodec_open2(videoReader->av_codec_ctx, videoReader->av_codec, NULL)) < 0)
  {
    printf("Couldn't open codec\n");
    return handle_error(err);
  }

  videoReader->av_frame = av_frame_alloc();
  videoReader->av_packet = av_packet_alloc();

  if (!videoReader->av_frame || !videoReader->av_packet)
  {
    printf("Couldn't allocate AVFrame or AVPacket!\n");
    return false;
  }

  return true;
}

int video_reader_next(VideoReader *videoReader, uint8_t **data_out)
{
  int err = 0;

  // Read the next frame from the stream
  if (av_read_frame(videoReader->av_format_ctx, videoReader->av_packet) >= 0)
  {
    if (videoReader->av_packet->stream_index == videoReader->video_stream_index)
    {
      // Supply decoder with raw packet data
      if ((err = avcodec_send_packet(videoReader->av_codec_ctx, videoReader->av_packet)) < 0)
      {
        printf("Failed to decode packet!\n");
        handle_error(err);
        return -1;
      }

      // Get decoded frame from the decoder
      err = avcodec_receive_frame(videoReader->av_codec_ctx, videoReader->av_frame);
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

    videoReader->width = videoReader->av_frame->width;
    videoReader->height = videoReader->av_frame->height;

    uint8_t *data = new uint8_t[videoReader->width * videoReader->height * 4];

    // Convert YUV frame colour data to RGBA
    if (!yuv_to_rgba(videoReader, data))
    {
      return -1;
    }

    *data_out = data;

    return 0;
  }
  else
  {
    return -1;
  }
}

void video_reader_close(VideoReader *videoReader)
{
  // Free up avformat data
  avformat_close_input(&videoReader->av_format_ctx);
  avformat_free_context(videoReader->av_format_ctx);

  // Free up packet and frame data
  av_frame_free(&videoReader->av_frame);
  av_packet_free(&videoReader->av_packet);

  // Free up avcodec data
  avcodec_free_context(&videoReader->av_codec_ctx);

  // if (videoReader->sws_scaler_ctx) {
  //   sws_freeContext(videoReader->sws_scaler_ctx);
  // }
}
