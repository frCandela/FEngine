#pragma once

#pragma warning( push, 0)
#pragma warning( disable, 4099)

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "bullet/btBulletDynamicsCommon.h"

#include "nlohmann_json.h"

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

#include "fanGlobalUsing.h"

#include "core/math/fanMathUtils.h"
#include "core/math/fanVector2.h"
#include "core/fanHash.h"
#include "vulkan/util/vkColor.h"

#pragma warning( pop )