#pragma once

#pragma warning( push, 0)
#pragma warning( disable, 4099)

#define GLFW_INCLUDE_VULKAN
#include "glfw/glfw3.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <vector>
#include <iostream>
#include <chrono>

#pragma warning( pop )