# cpp-video-app

This repo is dedicated to messing about with `GLFW` and `ffmpeg` (and its associated libraries) inside C++ to view and manipulate video data. Potential use cases for this would be a video player or video editing software.

Please feel free to take any of the code you find useful here - this project was started from [bartjoyce](https://github.com/bartjoyce)'s series of videos, starting [here](https://www.youtube.com/watch?v=A8jAKuPnFGg&ab_channel=Bartholomew).

## Dependencies
You will run into various errors while trying to build and run this project if you do not have these dependencies installed:
```shell
sudo apt-get update &&
sudo apt-get install cmake ffmpeg libavcodec-dev libavformat-dev libavdevice-dev libavfilter-dev libavutil-dev yasm pkg-config libglu1-mesa-dev freeglut3-dev mesa-common-dev libxi-dev libxcursor-dev libxinerama-dev libxrandr-dev libx11-dev doxygen
```

Make sure you have CMake installed, [follow this guide](https://cmake.org/install/) if you need help installing it.

## Clone
This repo contains the submodule `glfw`. Be sure to clone that repo as well by running:
```shell
git clone git@github.com:zachnicoll/cpp-video-app.git --recursive
```

## Build (with CMake & VSCode)
This project uses CMake to build and link the source code and various libraries.

Using the CMake Tools VSCode extension makes building and debugging a lot easier, so install and set it up if you use VSCode. You can then simply click the Play button at the bottom of the IDE to build and run the executable.

Otherwise, manually build and run the executable with:

```shell
mkdir build && cd build && cmake .. && cmake --build . && ./cpp-video-app

```

## Wishlist
- [x] Display the first frame of a video file in a window with GLFW and OpenGL
- [x] Play a video file from start to finish (unsynchronised)
- [ ] Play a video file, synchronised to its correct frame rate
- [x] Processing and rendering of video frames occuring on different threads
- [ ] Play audio synchronised with video
- [x] Play/Pause button and functionality
- [ ] Seeking (move forward or backwards through video) GUI element and functionality
- [ ] Full mock video player GUI in OpenGL (for fun)
