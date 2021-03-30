#pragma once
#include <vector>
#include <string>
#include <GLFW/glfw3.h>
#include "./element/element.hpp"
#include "./controller/controller.hpp"
#include "./rect/rect.hpp"
#include "./texture/texture.hpp"
#include "./callbacks/callbacks.hpp"
#include "../../lib/lodepng/lodepng.h"

/**
 * Reset 2D rendering colour to white.
 */
void reset_colour();
