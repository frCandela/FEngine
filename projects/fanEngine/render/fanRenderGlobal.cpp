#include "render/fanRenderGlobal.hpp"
#include "glfw/glfw3.h"

namespace fan
{
    const std::set<std::string>    RenderGlobal::sMeshExtensions   = { "gltf" };
    const std::set<std::string>    RenderGlobal::sImagesExtensions = { "png" };
    const std::set<std::string>    RenderGlobal::sSceneExtensions  = { "scene" };
    const std::set<std::string>    RenderGlobal::sPrefabExtensions = { "prefab" };
    const std::set<std::string>    RenderGlobal::sFontsExtensions  = { "ttf" };
    const std::set<std::string>    RenderGlobal::sAnimationsExtensions  = { "gltf" };

    const std::vector<const char*> RenderGlobal::sDesiredValidationLayers = {
#ifndef NDEBUG
            "VK_LAYER_LUNARG_standard_validation",
            "VK_LAYER_LUNARG_assistant_layer",
            "VK_LAYER_LUNARG_core_validation",
            "VK_LAYER_KHRONOS_validation",
            "VK_LAYER_LUNARG_monitor",
            "VK_LAYER_LUNARG_object_tracker",
            "VK_LAYER_LUNARG_screenshot",
            "VK_LAYER_LUNARG_standard_validation",
            "VK_LAYER_LUNARG_parameter_validation"
#endif
    };

    const std::vector<const char*> RenderGlobal::sDesiredInstanceExtensions =
                                           {
                                                   VK_EXT_DEBUG_REPORT_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME
                                           };

    const std::vector<const char*> RenderGlobal::sDesiredDeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
}