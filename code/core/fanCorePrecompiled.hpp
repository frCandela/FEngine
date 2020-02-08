#pragma once

#include <functional>
#include <map>
#include <cassert>
#include <vector>
#include <chrono>
#include <cstdint>
#include <type_traits>
#include <sstream>
#include <array>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <set> 

#pragma warning( push )
#pragma warning( disable : 4201 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4100 )

#include "bullet/btBulletDynamicsCommon.h"
#define GLFW_INCLUDE_VULKAN
#include "glfw/glfw3.h"
#include "nlohmann_json.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include <glm/gtx/hash.hpp>
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#include "imgui/imgui.h"

#pragma warning( pop )

using Json = nlohmann::json;

#include "core/fanDebug.hpp"
#include "core/fanColor.hpp"
#include "core/math/fanVector2.hpp"
#include "core/math/fanMathUtils.hpp"