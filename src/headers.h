#pragma once

#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <exception>
#include <string>
#include <vector>
#include "./video-reader/video_reader.hpp"

/**
 * Reshape viewport on window resize 
 */
void reshape(GLFWwindow *window, int w, int h);

/**
 * Render a 2D texture to the screen with given width and height, with bottom left corner at
 * xpos and ypos.
 */
void render_tex(GLuint *texture_handle, float f_w, float f_h, float xpos, float ypos);

/**
 * Initialise some recommended parameters for OpenGL
 */
void init_params();

/**
 * Initialise a GLFW window with width, height, and title
 * @returns pointer to window object
 */
GLFWwindow *init_window(int w, int h, const char *title);

/**
 * Function for loading and queueing frames from a VideoReader on a separate
 * thread. Although vid_reader is a void*, it is cast to a VideoReader inside
 * the function.
 */
void *load_frames_thread(void *vid_reader);

/**
 * Wrapper for calling GUI.RenderGUI().
 */
void render_gui(float window_width, float window_height);

/**
 * Allocate and add all GUI elements to the singleton GUI instance.
 */
void init_gui(float window_width, float window_height, GLuint *tex_handle);

/**
 * Wrapper for calling GUI.GetClickedElement() and running that elements OnClick() function.
 */
void handle_gui_click(float x_pos, float y_pos);

/**
 * Wrapper for calling GUI.DestroyGUI().
 */
void gui_close();
