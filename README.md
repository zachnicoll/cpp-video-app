# cpp-video-app

## Dependencies
You will run into various errors while trying to build and run this project if you do not have these dependencies installed:
```shell
sudo apt-get update &&
sudo apt-get install cmake ffmpeg libavcodec-dev libavformat-dev libavdevice-dev libavfilter-dev libavutil-dev yasm pkg-config libglu1-mesa-dev freeglut3-dev mesa-common-dev libxi-dev libxcursor-dev libxinerama-dev libxrandr-dev libx11-dev doxygen
```

Make sure you have CMake installed, [follow this guide](https://cmake.org/install/) if you need help installing it, or if the above command fails to install it.

## Build (with CMake & VSCode)
This project uses CMake to build and link the source code and various libraries.

Using the [CMake Tools VSCode extension](https://code.visualstudio.com/docs/cpp/cmake-linux) makes building and debugging a lot easier, so install and set it up if you use VSCode. You can then simply click the Play button at the bottom of the IDE to build and run the executable.

Otherwise, manually build and run the executable with:

```shell
mkdir build && cd build && cmake .. && cmake --build . && ./cpp-video-app

```

## Run
Of course, after building, the app can be run simply with `./cpp-video-app`
