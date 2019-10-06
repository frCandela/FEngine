#pragma once

#pragma warning( push, 0)
#pragma warning( disable, 4099)

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include <glm/gtx/hash.hpp>
#include "bullet/btBulletDynamicsCommon.h"

#include "nlohmann_json.h"
#include "bitset2/bitset2.hpp"

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

#include "fanGlobalValues.h"
#include "core/fanHash.h"
#include "renderer/util/fanColor.h"
#include "core/fanDebug.h"
#include "core/math/fanMathUtils.h"
#include "core/math/fanVector2.h"

namespace std {
	namespace fs = experimental::filesystem;
}

#pragma warning( pop )