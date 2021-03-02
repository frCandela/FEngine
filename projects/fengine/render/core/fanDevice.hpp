#pragma once

#include "glfw/glfw3.h"
#include <vector>
#include <string>
#include <map>
#include "core/fanAssert.hpp"

namespace fan
{
	struct Instance;

	//========================================================================================================
	// Vulkan device
	//========================================================================================================
	struct Device
	{
		Device(){}
		void Create( Instance& _instance, VkSurfaceKHR _surface );
		void Destroy();

		VkPhysicalDevice	mPhysicalDevice	= VK_NULL_HANDLE;
		VkDevice			mDevice			= VK_NULL_HANDLE;
		VkCommandPool		mCommandPool	= VK_NULL_HANDLE;
		VkQueue				mGraphicsQueue	= VK_NULL_HANDLE;

		VkPhysicalDeviceProperties			mDeviceProperties;
		VkPhysicalDeviceMemoryProperties	mMemoryProperties;
		std::map< uint64_t, std::string >	mDebugNames;
		

		uint32_t		FindMemoryType( uint32_t _typeFilter, VkMemoryPropertyFlags _properties );
		VkFormat		FindDepthFormat();
		VkCommandBuffer BeginSingleTimeCommands();
		void			EndSingleTimeCommands( VkCommandBuffer _commandBuffer );
		void				AddDebugName( const uint64_t _object, const std::string& _name );
		void				RemoveDebugName( const uint64_t _object );
		const std::string	GetDebugName( const uint64_t _object ) const;
	private:
		Device( Device const& ) = delete;
		Device& operator=( Device const& ) = delete;

        bool SelectPhysicalDevice( Instance& _instance,
                                   VkPhysicalDeviceFeatures& _outAvailableFeatures,
                                   std::vector<VkExtensionProperties>& _outAvailableExtensions );
        std::vector<const char*>
        GetDesiredExtensions( const std::vector<VkExtensionProperties>& _availableExtensions,
                              const std::vector<const char*> _desiredExtensions );
        static bool IsExtensionAvailable( const std::vector<VkExtensionProperties>& _availableExtensions,
                                          std::string _requiredExtension );
        void GetQueueFamiliesIndices( VkSurfaceKHR _surface,
                                      uint32_t& _outGraphics,
                                      uint32_t& _outCompute,
                                      uint32_t& _outPresent );
        bool ResetCommandPool();
	};
}