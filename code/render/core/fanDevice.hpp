#pragma once

#include "glfw/glfw3.h"
#include <vector>
#include <string>

namespace fan
{
	class Instance;

	//================================================================================================================================
	// Vulkan device (gpu)
	//================================================================================================================================
	class Device
	{
	public:
		Device( Instance* _instance, VkSurfaceKHR _surface );
		~Device();

		VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;
		VkDevice vkDevice = VK_NULL_HANDLE;

		VkCommandPool	GetCommandPool() const { return m_commandPool; }
		VkQueue&		GetGraphicsQueue() { return m_graphicsQueue; }
		uint32_t		GetGraphicsQueueFamilyIndex() { return m_graphicsQueueFamilyIndex; }
		uint32_t		FindMemoryType( uint32_t _typeFilter, VkMemoryPropertyFlags _properties );
		VkFormat		FindDepthFormat();

		VkCommandBuffer BeginSingleTimeCommands();
		void			EndSingleTimeCommands( VkCommandBuffer _commandBuffer );

		const VkPhysicalDeviceProperties& GetDeviceProperties() const { return m_deviceProperties; }

	private:
		Device( Device const& ) = delete;
		Device& operator=( Device const& ) = delete;

		Instance*		m_instance;
		VkSurfaceKHR	m_surface;
		VkCommandPool	m_commandPool;

		VkPhysicalDeviceFeatures				m_availableFeatures;
		VkPhysicalDeviceProperties				m_deviceProperties;
		VkPhysicalDeviceMemoryProperties		m_memoryProperties;
		std::vector<VkExtensionProperties>		m_availableExtensions;
		std::vector<VkQueueFamilyProperties>	m_queueFamilyProperties;
		uint32_t	m_graphicsQueueFamilyIndex;
		uint32_t	m_computeQueueFamilyIndex;
		uint32_t	m_presentQueueFamilyIndex;
		VkQueue		m_graphicsQueue;
		VkQueue		m_computeQueue;
		VkQueue		m_presentQueue;

		// Device
		bool Create();
		bool SelectPhysicalDevice();
		std::vector < const char*> GetDesiredExtensions( const std::vector < const char*> _desiredExtensions );
		bool IsExtensionAvailable( std::string _requiredExtension );
		void GetQueueFamilies();

		// Command pool
		bool CreateCommandPool();
		bool ResetCommandPool();

	};
}