#pragma once

#pragma warning( push, 0)
#pragma warning( disable, 4099)

#define GLFW_INCLUDE_VULKAN
#include "glfw/glfw3.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "util/fanSignal.h"
#include "util/fanImguiUtil.h"

#include <vector>
#include <array>

#include <iostream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <algorithm>
#include <fstream>

#pragma warning( pop )