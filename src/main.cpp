#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <exception>
#include <string>
#include <unistd.h>
#include <vector>
#include <sstream>
#include "video-reader/video_reader.hpp"

#define NUMPROFILES 2

typedef struct
{
  int width;
  int height;
  int64_t bit_rate;
} Profile;

float convert_to_kpbs(int64_t bit_rate)
{
  return (float)bit_rate / 1000.0;
}

int main(int argc, const char **argv)
{
  // TODO: Get these from argv
  const char *input_file = "/home/zach/Downloads/vid.mp4"; // Change this to load a different file
  const char *output_file = "/home/zach/Videos/output.mp4";            // Change this to output different filename

  VideoReader *video_reader = video_reader_init();

  try
  {
    video_reader_open(video_reader, input_file);
  }
  catch (std::exception &e)
  {
    printf("An exception occured opening %s:\n%s", input_file, e.what());
    return false;
  }

  int base_width = video_reader->width;
  int base_height = video_reader->height;
  int64_t base_bit_rate = video_reader->av_codec_ctx->bit_rate;
  int bit_rate_tolerance = video_reader->av_codec_ctx->bit_rate_tolerance;

  video_reader_close(video_reader);

  printf("----Base Video Information----\n\n");
  printf("Video Dimensions: %d x %d\n", base_width, base_height);
  printf("Average Video Bit Rate: %f kbps\n", convert_to_kpbs(base_bit_rate));
  printf("Video Bit Rate Tolerance: %f kbps\n", convert_to_kpbs(bit_rate_tolerance));

  // Perform some arbitrary operation to create desired profiles
  Profile profile_1;
  profile_1.width = base_width * 0.8;
  profile_1.height = base_height * 0.8;
  profile_1.bit_rate = base_bit_rate * 0.5;

  Profile profile_2;
  profile_2.width = base_width * 0.5;
  profile_2.height = base_height * 0.5;
  profile_2.bit_rate = base_bit_rate * 0.75;

  Profile profiles[NUMPROFILES];
  profiles[0] = profile_1;
  profiles[1] = profile_2;

  printf("\n----Available Profiles----\n\n");

  for (int i = 0; i < NUMPROFILES; i++)
  {
    printf("[%d]: %d x %d, %d kbps\n", i, profiles[i].width, profiles[i].height, (int)convert_to_kpbs(profiles[i].bit_rate));
  }

  printf("Choose profile: ");
  int selected_profile;
  std::cin >> selected_profile;

  if (selected_profile < 0 || selected_profile >= NUMPROFILES)
  {
    printf("\nInvalid profile choice, aborting!\n");
    return -1;
  }

  printf("Executing ffmpeg with selected profile...\n\n");

  // Convert bit rate to kbps
  int kbps_bit_rate = (int)convert_to_kpbs(profiles[selected_profile].bit_rate);

  std::ostringstream arg_oss;

  // Input file
  arg_oss << "ffmpeg " << "-i " << input_file << " ";

  // Bit rate
  arg_oss << "-b:v " << kbps_bit_rate << "k "
                   << "-bufsize " << kbps_bit_rate << "k" << " ";

  // Scale (width & height)
  arg_oss << "-vf scale=" << profiles[selected_profile].width << ":" << profiles[selected_profile].height << " ";
  
  // Output file
  arg_oss << output_file;

  /**
   * Should execute ffmpeg with following opts:
   * ffmpeg -i input.avi -b:v 64k -bufsize 64k -vf scale=320:240 output.avi
   */
  system(arg_oss.str().c_str());

  return 0;
}
