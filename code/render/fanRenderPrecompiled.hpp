#pragma once

#include <vector>
#include <set>
#include <filesystem>

#define GLFW_INCLUDE_VULKAN
#include "glfw/glfw3.h"
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS

#pragma warning( push )
#pragma warning( disable : 4244 )
#include "imgui/imgui.h"
#pragma warning( pop )

#include "core/fanDebug.hpp"