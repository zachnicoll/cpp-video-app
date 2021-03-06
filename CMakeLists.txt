cmake_minimum_required(VERSION 3.14)
project(video_app C CXX)
set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(lib/glfw)

add_executable(video_app src/main.cpp)
target_link_libraries(video_app OpenGL::GL)
target_link_libraries(video_app glfw)
