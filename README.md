# cpp-video-app

This repo is dedicated to messing about with GLFW and ffmpeg (and its associated libraries) inside C++ to view and manipulate video data. Potential use cases for this would be a video player, or video editing software.

Please feel free to take any of the code you find useful here - this project was started from [https://github.com/bartjoyce/video-app](bartjoyce)'s series of videos, starting [https://www.youtube.com/watch?v=A8jAKuPnFGg&ab_channel=Bartholomew](here).

## Dependencies
You will run into various errors while trying to build and run this project you do not have these dependencies installed:
```shell
sudo apt-get update &&
sudo apt-get install ffmpeg libavcodec-dev libavformat-dev libavdevice-dev libavfilter-dev libavutil-dev yasm pkg-config libglu1-mesa-dev freeglut3-dev mesa-common-dev libxi-dev libxcursor-dev libxinerama-dev libxrandr-dev libx11-dev doxygen
```

Make sure you have CMake installed, [https://cmake.org/install/](follow this guide).

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
rm -rf ./build && mkdir build && cd build && cmake .. && cpp-video-app

```
