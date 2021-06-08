# Project Settings
cmake_minimum_required(VERSION 3.14)
project(cpp-video-app C CXX)
set(CMAKE_CXX_STANDARD 14)

# Find and add all ffmpeg libs to project
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
    libavdevice
    libavfilter
    libavformat
    libavcodec
    # libswresample
    libswscale
    libavutil
)

list(APPEND SOURCES
  "src/main.cpp"
  "src/video-reader/video_reader.cpp"
)

# Add main.cpp to the executable
add_executable(cpp-video-app ${SOURCES})

# Link OpenGL libs and GLFW
target_link_libraries(cpp-video-app PkgConfig::LIBAV ${EXTRA_LIBS})
