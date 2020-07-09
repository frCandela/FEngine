#pragma once

#include "glfw/glfw3.h"
#include <vector>
#include <string>

namespace fan
{
	struct Instance;

	//================================================================================================================================
	// Vulkan device
	//================================================================================================================================
	struct Device
	{
		Device(){}
		void Create( Instance& _instance, VkSurfaceKHR _surface );
		void Destroy();

		VkPhysicalDevice	physicalDevice	= VK_NULL_HANDLE;
		VkDevice			device			= VK_NULL_HANDLE;
		VkCommandPool		commandPool		= VK_NULL_HANDLE;
		VkQueue				graphicsQueue	= VK_NULL_HANDLE;

		VkPhysicalDeviceProperties		 deviceProperties;
		VkPhysicalDeviceMemoryProperties memoryProperties;

		uint32_t		FindMemoryType( uint32_t _typeFilter, VkMemoryPropertyFlags _properties );
		VkFormat		FindDepthFormat();
		VkCommandBuffer BeginSingleTimeCommands();
		void			EndSingleTimeCommands( VkCommandBuffer _commandBuffer );
	
	private:
		Device( Device const& ) = delete;
		Device& operator=( Device const& ) = delete;	
		
		bool						SelectPhysicalDevice( Instance& _instance, VkPhysicalDeviceFeatures& _outAvailableFeatures, std::vector<VkExtensionProperties>& _outAvailableExtensions );
		std::vector < const char*>	GetDesiredExtensions( const std::vector<VkExtensionProperties>& _availableExtensions, const std::vector < const char*> _desiredExtensions );
		static bool					IsExtensionAvailable( const std::vector<VkExtensionProperties>& _availableExtensions, std::string _requiredExtension );
		void						GetQueueFamiliesIndices( VkSurfaceKHR _surface, uint32_t& _outGraphics, uint32_t& _outCompute, uint32_t& _outPresent );
		bool						ResetCommandPool();
	};
}