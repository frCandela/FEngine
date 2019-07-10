#pragma once

#pragma warning( push, 0)
#pragma warning( disable, 4099)

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "bullet/btBulletDynamicsCommon.h"

#define GLFW_INCLUDE_VULKAN
#include "glfw/glfw3.h"
#include "imgui/imgui.h"

#include <vector>
#include <map>
#include <set>
#include <array>
#include <queue>
#include <iostream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <algorithm>
#include <fstream>
#include <type_traits>
#include <cassert>
#include <filesystem>

#include "util/fanVector2.h"
#include "util/fanUtil.h"

#pragma warning( pop )