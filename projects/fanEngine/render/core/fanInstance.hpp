#pragma once

#include "glfw/glfw3.h"
#include <string>
#include <vector>

namespace fan
{
    //==================================================================================================================================================================================================
    // The vulkan instance of the application
    //==================================================================================================================================================================================================
    struct Instance
    {
        void Create( void* _debugCallbackUserData );
        void Destroy();

        VkInstance mInstance = VK_NULL_HANDLE;

        std::vector<const char*> mEnabledValidationLayers;
        std::vector<const char*> mEnabledExtensions;
        VkDebugReportCallbackEXT mDebugReportCallback;

    private:
        void FindDesiredValidationLayers( const std::vector<const char*> _desiredLayers );
        void FindDesiredExtensions( const std::vector<const char*> _desiredExtensions );
        bool SetupDebugCallback( void* _userData );

        static bool IsExtensionAvailable( const std::vector<VkExtensionProperties>& _availableExtensions,
                                          const std::string _requiredExtension );
        static bool IsLayerAvailable( const std::vector<VkLayerProperties>& _availableLayers,
                                      const std::string _requiredLayer );
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
                VkDebugReportFlagsEXT _flags,
                VkDebugReportObjectTypeEXT _objType,
                uint64_t _obj,
                size_t _location,
                int32_t _code,
                const char* _layerPrefix,
                const char* msg,
                void* _userData );
    };
}