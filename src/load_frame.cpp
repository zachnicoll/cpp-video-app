#include <stdint.h>
extern "C" {
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
}

bool handle_error(int err) {
    char* errbuff = new char[1024];
    av_strerror(err, errbuff, 1024 );
    printf("av_strerror: %s\n", errbuff);
}

bool load_frame(const char* filename, int* width, int* height, uint8_t** data) {
  int err = 0;

  AVFormatContext* av_format_ctx = avformat_alloc_context();
  if (!av_format_ctx) {
    printf("Could not allocate AVFormatContext!\n");
    return false;
  }

  // Need to enable all muxers, demuxers, and protocols
  av_register_all();
  avcodec_register_all();

  if ((err = avformat_open_input(&av_format_ctx, filename, NULL, NULL)) != 0) {
    printf("Couldn't open video file!\n");
    handle_error(err);
    return false;
  }


  return true;
}
