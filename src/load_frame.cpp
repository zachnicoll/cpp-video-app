#include <stdint.h>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

bool handle_error(int err)
{
  char *errbuff = new char[1024];
  av_strerror(err, errbuff, 1024);
  printf("av_strerror: %s\n", errbuff);
  return false;
}

bool yuv_to_rgba(AVFrame* frame, AVCodecContext* codec_ctx, uint8_t* data_out) {
  SwsContext* sws_scaler_ctx = sws_getContext(
    frame->width, frame->height, codec_ctx->pix_fmt, // Input formats
    frame->width, frame->height, AV_PIX_FMT_RGBA, // Output formats
    SWS_BILINEAR,
    NULL, NULL, NULL
  );

  if (!sws_scaler_ctx) {
    printf("Couldn't get SwsContext!\n");
    return false;
  }

  uint8_t* dest[4] = {data_out, 0, 0, 0};
  int dest_linesize[4] = {frame->width * 4, 0, 0, 0};
  sws_scale(sws_scaler_ctx, frame->data, frame->linesize, 0, frame->height, dest, dest_linesize);

  sws_freeContext(sws_scaler_ctx);
  return true;
}

bool load_frame(const char *filename, int *width_out, int *height_out, uint8_t **data_out)
{
  int err = 0;

  AVFormatContext *av_format_ctx = avformat_alloc_context();
  if (!av_format_ctx)
  {
    printf("Could not allocate AVFormatContext!\n");
    return false;
  }

  // Need to enable all muxers, demuxers, and protocols
  av_register_all();
  avcodec_register_all();

  // Open video file stream and header metadata into av_format_ctx 
  if ((err = avformat_open_input(&av_format_ctx, filename, NULL, NULL)) != 0)
  {
    printf("Couldn't open video file!\n");
    return handle_error(err);
  }

  int video_stream_index = -1;
  AVCodecParameters *codec_params;
  AVCodec *av_codec;

  // Find video stream that has valid decoder
  for (int i = 0; i < av_format_ctx->nb_streams; i++)
  {
    codec_params = av_format_ctx->streams[i]->codecpar; // Codec parameters associated with stream
    av_codec = avcodec_find_decoder(codec_params->codec_id);

    if (!av_codec)
    {
      // Keep looking for a stream that has a decoder
      continue;
    }

    // If stream type is a video, we've found the right stream
    if (codec_params->codec_type == AVMEDIA_TYPE_VIDEO)
    {
      video_stream_index = i;
      break;
    }
  }

  if (video_stream_index == -1)
  {
    printf("Could not find valid video stream!\n");
    return false;
  }

  // Set up a codec context for the decoder
  AVCodecContext *av_codec_ctx = avcodec_alloc_context3(av_codec);
  if (!av_codec_ctx)
  {
    printf("Coudn't create AVCodecContext!\n");
    return false;
  }

  if ((err = avcodec_parameters_to_context(av_codec_ctx, codec_params)) < 0)
  {
    printf("Coudn't init AVCodecContext!\n");
    return handle_error(err);
  }

  if ((err = avcodec_open2(av_codec_ctx, av_codec, NULL)) < 0)
  {
    printf("Couldn't open codec\n");
    return handle_error(err);
  }

  AVFrame *av_frame = av_frame_alloc();
  AVPacket *av_packet = av_packet_alloc();

  if (!av_frame || !av_packet)
  {
    printf("Couldn't allocate AVFrame or AVPacket!\n");
    return false;
  }

  // Read first frame from the packet stream
  while (av_read_frame(av_format_ctx, av_packet) >= 0)
  {
    if (av_packet->stream_index == video_stream_index)
    {
      if ((err = avcodec_send_packet(av_codec_ctx, av_packet)) < 0)
      {
        printf("Failed to decode packet!\n");
        return handle_error(err);
      }

      err = avcodec_receive_frame(av_codec_ctx, av_frame);
      if (err == AVERROR(EAGAIN) || err == AVERROR_EOF)
      {
        continue;
      }
      else if (err < 0)
      {
        printf("Failed to decode packet!\n");
        return handle_error(err);
      }

      // Break after reading first packet
      break;
    }
  }

  uint8_t* data = new uint8_t[av_frame->width * av_frame->height * 4];

  if (!yuv_to_rgba(av_frame, av_codec_ctx, data)) {
    return false;
  }

  *width_out = av_frame->width;
  *height_out = av_frame->height;
  *data_out = data;

  // Free up avformat data
  avformat_close_input(&av_format_ctx);
  avformat_free_context(av_format_ctx);

  // Free up packet and frame data
  av_frame_free(&av_frame);
  av_packet_free(&av_packet);

  // Free up avcodec data
  avcodec_free_context(&av_codec_ctx);

  return true;
}
