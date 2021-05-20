#include "render/core/fanInstance.hpp"

#include <iostream>
#include "core/fanDebug.hpp"
#include "render/fanRenderGlobal.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Instance::Create( void* _debugCallbackUserData )
    {
        fanAssert( mInstance == VK_NULL_HANDLE );

        // Get desired extensions
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwInit();
        int res = glfwVulkanSupported() == GLFW_TRUE;
        (void)res;
        glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount );
        std::vector<const char*> desiredExtensions = RenderGlobal::sDesiredInstanceExtensions;

        for( unsigned glfwExtensionIndex = 0; glfwExtensionIndex < glfwExtensionCount; glfwExtensionIndex++ )
        {
            desiredExtensions.push_back( glfwExtensions[glfwExtensionIndex] );
        }

        FindDesiredExtensions( desiredExtensions );
        FindDesiredValidationLayers( RenderGlobal::sDesiredValidationLayers );

        VkApplicationInfo appInfo;
        appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pNext              = VK_NULL_HANDLE;
        appInfo.pApplicationName   = "FEngine";
        appInfo.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
        appInfo.pEngineName        = "FEngine";
        appInfo.engineVersion      = VK_MAKE_VERSION( 1, 0, 0 );

        VkInstanceCreateInfo instanceCreateInfo;
        instanceCreateInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pNext                   = VK_NULL_HANDLE;
        instanceCreateInfo.flags                   = 0;
        instanceCreateInfo.pApplicationInfo        = &appInfo;
        instanceCreateInfo.enabledLayerCount       = static_cast< uint32_t >( mEnabledValidationLayers.size() );
        instanceCreateInfo.ppEnabledLayerNames     = mEnabledValidationLayers.data();
        instanceCreateInfo.enabledExtensionCount   = static_cast< uint32_t >( mEnabledExtensions.size() );
        instanceCreateInfo.ppEnabledExtensionNames = mEnabledExtensions.size() > 0 ?
                mEnabledExtensions.data() :
                nullptr;

        if( vkCreateInstance( &instanceCreateInfo, nullptr, &mInstance ) != VK_SUCCESS ||
            mInstance == VK_NULL_HANDLE )
        {
            Debug::Error( "ouch, this is going to be messy" );
        }
        SetupDebugCallback( _debugCallbackUserData );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Instance::Destroy()
    {
        // destroy debug report callback
        PFN_vkDestroyDebugReportCallbackEXT func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(
                mInstance,
                "vkDestroyDebugReportCallbackEXT" );
        if( func != nullptr )
        {
            func( mInstance, mDebugReportCallback, nullptr );
        }

        vkDestroyInstance( mInstance, nullptr );
        mInstance = VK_NULL_HANDLE;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Instance::IsExtensionAvailable( const std::vector<VkExtensionProperties>& _availableExtensions,
                                         const std::string _requiredExtension )
    {
        for( int i = 0; i < (int)_availableExtensions.size(); i++ )
        {
            if( _requiredExtension.compare( _availableExtensions[i].extensionName ) == 0 )
            {
                return true;
            }
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Instance::IsLayerAvailable( const std::vector<VkLayerProperties>& _availableLayers,
                                     const std::string _requiredLayer )
    {
        for( int i = 0; i < (int)_availableLayers.size(); i++ )
        {
            if( _requiredLayer.compare( _availableLayers[i].layerName ) == 0 )
            {
                return true;
            }
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Instance::FindDesiredValidationLayers( const std::vector<const char*> _desiredLayers )
    {
        if( _desiredLayers.empty() ){ return; }

        // Get available layers
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties( &layerCount, nullptr );
        std::vector<VkLayerProperties> availableLayers( layerCount );
        vkEnumerateInstanceLayerProperties( &layerCount, availableLayers.data() );

        mEnabledValidationLayers.clear();
        mEnabledValidationLayers.reserve( _desiredLayers.size() );
        for( int layerIndex = 0; layerIndex < (int)_desiredLayers.size(); layerIndex++ )
        {
            if( IsLayerAvailable( availableLayers, _desiredLayers[layerIndex] ) )
            {
                mEnabledValidationLayers.push_back( _desiredLayers[layerIndex] );
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Instance::FindDesiredExtensions( const std::vector<const char*> _desiredExtensions )
    {
        // Get available extensions
        uint32_t extensionsCount;
        vkEnumerateInstanceExtensionProperties( nullptr, &extensionsCount, nullptr );
        std::vector<VkExtensionProperties> availableExtensions( extensionsCount );
        vkEnumerateInstanceExtensionProperties( nullptr, &extensionsCount, availableExtensions.data() );

        mEnabledExtensions.clear();
        mEnabledExtensions.reserve( _desiredExtensions.size() );
        for( int extensionIndex = 0; extensionIndex < (int)_desiredExtensions.size(); extensionIndex++ )
        {
            if( IsExtensionAvailable( availableExtensions, _desiredExtensions[extensionIndex] ) )
            {
                mEnabledExtensions.push_back( _desiredExtensions[extensionIndex] );
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Instance::SetupDebugCallback( void* _userData )
    {
        VkDebugReportCallbackCreateInfoEXT createInfo = {};
        createInfo.sType       = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        createInfo.flags       = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
        createInfo.pfnCallback = DebugCallback;
        createInfo.pUserData   = _userData;

        auto func = (PFN_vkCreateDebugReportCallbackEXT)
                vkGetInstanceProcAddr( mInstance, "vkCreateDebugReportCallbackEXT" );
        if( func != nullptr && func( mInstance, &createInfo, nullptr, &mDebugReportCallback ) == VK_SUCCESS )
        {
            Debug::Log() << Debug::Type::Render << std::hex << "VkDebugCallback       " << mDebugReportCallback << std::dec << Debug::Endl();
            return true;
        }

        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    VKAPI_ATTR VkBool32 VKAPI_CALL Instance::DebugCallback( VkDebugReportFlagsEXT /*_flags*/,
                                                            VkDebugReportObjectTypeEXT /*_objType*/,
                                                            uint64_t _obj,
                                                            size_t /*_location*/,
                                                            int32_t /*_code*/,
                                                            const char* /*_layerPrefix*/,
                                                            const char* _msg,
                                                            void* _userData )
    {
        Debug::Error() << Debug::Type::Render << "Vulkan  Error:  " << _msg << Debug::Endl();
        std::stringstream ss;
        ss << "######## VkError";

        std::map<uint64_t, std::string>& debugNames = *static_cast<std::map<uint64_t,
                                                                            std::string>*>( _userData );
        auto it = debugNames.find( _obj );
        if( it != debugNames.end() )
        {
            ss << " [  " << it->second << "  ] ";
        }

        ss << _msg;

        std::cout << ss.str() << std::endl;
        return VK_FALSE;
    }
}