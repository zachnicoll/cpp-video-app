# Project Settings
cmake_minimum_required(VERSION 3.14)
project(video_app C CXX)
set(CMAKE_CXX_STANDARD 14)

# Don't build GLFW docs, tests, & exmaples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Add GLFW subdir to the build
add_subdirectory(lib/glfw)

# Need to include OpenGL as a library, different for each OS
if(APPLE)
    list(APPEND EXTRA_LIBS
      "-framework OpenGL"
    )
elseif(WIN32)
    list(APPEND EXTRA_LIBS
      "-lglu32 -lopengl32"
    )
    set(CMAKE_EXE_LINKER_FLAGS "-std=gnu99 -static -static-libgcc -static-libstdc++ -mwindows")
else()
    list(APPEND EXTRA_LIBS
      "-lGL -lGLU -lX11"
    )
endif()

# Add main.cpp to the executable
add_executable(video_app src/main.cpp)

# Link OpenGL libs and GLFW
target_link_libraries(video_app glfw ${EXTRA_LIBS})
