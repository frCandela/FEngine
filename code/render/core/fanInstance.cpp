#include "render/core/fanInstance.hpp"

#include <iostream>
#include "core/fanDebug.hpp"
#include "render/fanRenderGlobal.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void Instance::Create()
	{
		assert( instance == VK_NULL_HANDLE );

		// Get desired extensions
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwInit();
		int res = glfwVulkanSupported() == GLFW_TRUE;
		( void ) res;
		glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount );
		std::vector< const char* > desiredExtensions = RenderGlobal::s_desiredInstanceExtensions;

		for ( unsigned glfwExtensionIndex = 0; glfwExtensionIndex < glfwExtensionCount; glfwExtensionIndex++ )
		{
			desiredExtensions.push_back( glfwExtensions[ glfwExtensionIndex ] );
		}

		FindDesiredExtensions( desiredExtensions );	
		FindDesiredValidationLayers( RenderGlobal::s_desiredValidationLayers );

		VkApplicationInfo appInfo;
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "FEngine";
		appInfo.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
		appInfo.pEngineName = "FEngine";
		appInfo.engineVersion = VK_MAKE_VERSION( 1, 0, 0 );

		VkInstanceCreateInfo instanceCreateInfo;
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pNext = nullptr;
		instanceCreateInfo.flags = 0;
		instanceCreateInfo.pApplicationInfo = &appInfo;
		instanceCreateInfo.enabledLayerCount = static_cast< uint32_t >( enabledValidationLayers.size() );
		instanceCreateInfo.ppEnabledLayerNames = enabledValidationLayers.data();
		instanceCreateInfo.enabledExtensionCount = static_cast< uint32_t >( enabledExtensions.size() );
		instanceCreateInfo.ppEnabledExtensionNames = enabledExtensions.size() > 0 ? enabledExtensions.data() : nullptr;

		if ( vkCreateInstance( &instanceCreateInfo, nullptr, &instance ) != VK_SUCCESS || instance == VK_NULL_HANDLE )
		{
			Debug::Error( "ouch, this is going to be messy" );
		}
		SetupDebugCallback();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Instance::Destroy()
	{
		// destroy debug report callback
		PFN_vkDestroyDebugReportCallbackEXT func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr( instance, "vkDestroyDebugReportCallbackEXT" );
		if( func != nullptr )
		{
			func( instance, debugReportCallback, nullptr );
		}

		vkDestroyInstance( instance, nullptr );
		instance = VK_NULL_HANDLE;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Instance::IsExtensionAvailable( const std::vector< VkExtensionProperties >& _availableExtensions, const std::string _requiredExtension )
	{
		for ( int availableExtensionIndex = 0; availableExtensionIndex < _availableExtensions.size(); availableExtensionIndex++ )
		{
			if ( _requiredExtension.compare( _availableExtensions[ availableExtensionIndex ].extensionName ) == 0 )
			{
				return true;
			}
		}
		return false;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Instance::IsLayerAvailable( const std::vector<VkLayerProperties>& _availableLayers, const std::string _requiredLayer )
	{
		for ( int availableLayerIndex = 0; availableLayerIndex < _availableLayers.size(); availableLayerIndex++ )
		{
			if ( _requiredLayer.compare( _availableLayers[ availableLayerIndex ].layerName ) == 0 )
			{
				return true;
			}
		}
		return false;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Instance::FindDesiredValidationLayers( const std::vector < const char*> _desiredLayers )
	{
		if( _desiredLayers.empty() ) { return; }

		// Get available layers
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties( &layerCount, nullptr );
		std::vector<VkLayerProperties> availableLayers( layerCount );
		vkEnumerateInstanceLayerProperties( &layerCount, availableLayers.data() );

		enabledValidationLayers.clear();
		enabledValidationLayers.reserve( _desiredLayers.size() );
		for ( int layerIndex = 0; layerIndex < _desiredLayers.size(); layerIndex++ )
		{
			if ( IsLayerAvailable( availableLayers, _desiredLayers[ layerIndex ] ) )
			{
				enabledValidationLayers.push_back( _desiredLayers[ layerIndex ] );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Instance::FindDesiredExtensions( const std::vector < const char*> _desiredExtensions )
	{
		// Get available extensions
		uint32_t extensionsCount;
		vkEnumerateInstanceExtensionProperties( nullptr, &extensionsCount, nullptr );
		std::vector< VkExtensionProperties > availableExtensions( extensionsCount );
		vkEnumerateInstanceExtensionProperties( nullptr, &extensionsCount, availableExtensions.data() );

		enabledExtensions.clear();
		enabledExtensions.reserve( _desiredExtensions.size() );
		for ( int extensionIndex = 0; extensionIndex < _desiredExtensions.size(); extensionIndex++ )
		{
			if ( IsExtensionAvailable( availableExtensions, _desiredExtensions[ extensionIndex ] ) )
			{
				enabledExtensions.push_back( _desiredExtensions[ extensionIndex ] );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Instance::SetupDebugCallback()
	{
		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		createInfo.pfnCallback = DebugCallback;

		auto func = ( PFN_vkCreateDebugReportCallbackEXT ) vkGetInstanceProcAddr( instance, "vkCreateDebugReportCallbackEXT" );
		if ( func != nullptr && func( instance, &createInfo, nullptr, &debugReportCallback ) == VK_SUCCESS )
		{
			Debug::Get() << Debug::Severity::log << std::hex << "VkDebugCallback       " << debugReportCallback << std::dec << Debug::Endl();
			return true;
		}

		return false;
	}

	//================================================================================================================================
	//================================================================================================================================
	VKAPI_ATTR VkBool32 VKAPI_CALL Instance::DebugCallback( VkDebugReportFlagsEXT /*_flags*/, VkDebugReportObjectTypeEXT /*_objType*/, uint64_t /*_obj*/, size_t /*_location*/, int32_t /*_code*/, const char* /*_layerPrefix*/, const char* _msg, void* /*_userData*/ )
	{
		//Debug::Get() << Debug::Severity::error << "Vulkan  Error:  " << _msg << Debug::Endl();
		std::cout << "Vulkan  Error:  " << _msg << std::endl;
		return VK_FALSE;
	}
}