#include "render/core/fanDevice.hpp"
#include "render/core/fanInstance.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Device::Device( Instance* _instance, VkSurfaceKHR _surface ) :
		m_instance( _instance ),
		m_surface( _surface )
	{
		Create();
		CreateCommandPool();
	}

	//================================================================================================================================
	//================================================================================================================================
	Device::~Device()
	{
		vkDestroyCommandPool( vkDevice, m_commandPool, nullptr );

		vkDestroyDevice( vkDevice, nullptr );
		vkDevice = VK_NULL_HANDLE;
	}

	//================================================================================================================================
	//================================================================================================================================
	VkCommandBuffer Device::BeginSingleTimeCommands()
	{
		// Allocate a temporary command buffer for memory transfer operations
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers( vkDevice, &allocInfo, &commandBuffer );

		// Start recording the command buffer
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer( commandBuffer, &beginInfo );

		return commandBuffer;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Device::EndSingleTimeCommands( VkCommandBuffer _commandBuffer )
	{
		vkEndCommandBuffer( _commandBuffer );

		// Execute the command buffer to complete the transfer
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &_commandBuffer;

		vkQueueSubmit( GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE );
		vkQueueWaitIdle( GetGraphicsQueue() );

		// Cleaning
		vkFreeCommandBuffers( vkDevice, m_commandPool, 1, &_commandBuffer );
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Device::ResetCommandPool()
	{
		VkCommandPoolResetFlags releaseResources = VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT;

		if ( vkResetCommandPool( vkDevice, m_commandPool, releaseResources ) != VK_SUCCESS )
		{
			Debug::Error( "Could not reset command pool." );
			return false;
		}
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Device::CreateCommandPool()
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo;
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.pNext = nullptr;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = GetGraphicsQueueFamilyIndex();

		if ( vkCreateCommandPool( vkDevice, &commandPoolCreateInfo, nullptr, &m_commandPool ) != VK_SUCCESS )
		{
			Debug::Error( "Could not allocate command pool." );
			return false;
		}
		Debug::Get() << Debug::Severity::log << std::hex << "VkCommandPool         " << m_commandPool << std::dec << Debug::Endl();
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	uint32_t Device::FindMemoryType( uint32_t _typeFilter, VkMemoryPropertyFlags _properties )
	{
		//check for the support of the properties
		for ( uint32_t propertyIndex = 0; propertyIndex < m_memoryProperties.memoryTypeCount; propertyIndex++ )
		{
			if ( ( _typeFilter & ( 1 << propertyIndex ) ) && ( m_memoryProperties.memoryTypes[ propertyIndex ].propertyFlags & _properties ) == _properties )
			{
				return propertyIndex;
			}
		}
		Debug::Get() << Debug::Severity::error << "Failed to find suitable memory type " << _typeFilter << " " << _properties << Debug::Endl();
		return ~0u;
	}

	//================================================================================================================================
	//================================================================================================================================
	VkFormat Device::FindDepthFormat()
	{
		const std::vector<VkFormat> candidates{
			VK_FORMAT_D32_SFLOAT
			,VK_FORMAT_D32_SFLOAT_S8_UINT
			,VK_FORMAT_D24_UNORM_S8_UINT
		};
		VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
		VkFormatFeatureFlags features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

		for ( int candidateIndex = 0; candidateIndex < candidates.size(); candidateIndex++ )
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties( vkPhysicalDevice, candidates[ candidateIndex ], &props );

			if ( tiling == VK_IMAGE_TILING_LINEAR && ( props.linearTilingFeatures & features ) == features )
			{
				return candidates[ candidateIndex ];
			}

			else if ( tiling == VK_IMAGE_TILING_OPTIMAL && ( props.optimalTilingFeatures & features ) == features )
			{
				return candidates[ candidateIndex ];
			}
		}
		return VK_FORMAT_MAX_ENUM;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Device::Create()
	{
		SelectPhysicalDevice();
		GetQueueFamilies();
		std::vector< const char* > existingExtensions = GetDesiredExtensions(
			{ VK_KHR_SWAPCHAIN_EXTENSION_NAME

			}
		);

		float queuePriority = 1.0f;
		std::vector <VkDeviceQueueCreateInfo> queueCreateInfos;

		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.pNext = nullptr;
		queueCreateInfo.flags = 0;
		queueCreateInfo.queueFamilyIndex = m_graphicsQueueFamilyIndex;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back( queueCreateInfo );

		VkPhysicalDeviceFeatures desiredFeatures = {};
		desiredFeatures.samplerAnisotropy = m_availableFeatures.samplerAnisotropy == VK_TRUE;
		desiredFeatures.shaderUniformBufferArrayDynamicIndexing = VK_TRUE;

		VkPhysicalDeviceDescriptorIndexingFeaturesEXT indexingFeatures = {};
		indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
		indexingFeatures.pNext = nullptr;
		//indexingFeatures.runtimeDescriptorArray = VK_TRUE;

		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pNext = &indexingFeatures;
		deviceCreateInfo.flags = 0;
		deviceCreateInfo.queueCreateInfoCount = static_cast< uint32_t >( queueCreateInfos.size() );
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.enabledLayerCount = static_cast< uint32_t >( m_instance->GetValidationLayers().size() );
		deviceCreateInfo.ppEnabledLayerNames = m_instance->GetValidationLayers().data();
		deviceCreateInfo.enabledExtensionCount = static_cast< uint32_t >( existingExtensions.size() );
		deviceCreateInfo.ppEnabledExtensionNames = existingExtensions.data();
		deviceCreateInfo.pEnabledFeatures = &desiredFeatures;

		if ( vkCreateDevice( vkPhysicalDevice, &deviceCreateInfo, nullptr, &vkDevice ) != VK_SUCCESS )
		{
			return false;
		}
		Debug::Get() << Debug::Severity::log << std::hex << "vkDevice:             " << m_deviceProperties.deviceName << std::dec << Debug::Endl();

		vkGetDeviceQueue( vkDevice, m_graphicsQueueFamilyIndex, 0, &m_graphicsQueue );
		vkGetDeviceQueue( vkDevice, m_computeQueueFamilyIndex, 0, &m_computeQueue );
		vkGetDeviceQueue( vkDevice, m_presentQueueFamilyIndex, 0, &m_presentQueue );

		vkGetPhysicalDeviceMemoryProperties( vkPhysicalDevice, &m_memoryProperties );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Device::SelectPhysicalDevice()
	{
		uint32_t devicesCount;
		if ( vkEnumeratePhysicalDevices( m_instance->vkInstance, &devicesCount, nullptr ) != VK_SUCCESS ) { return false; }
		std::vector< VkPhysicalDevice> availableDevices( devicesCount );
		if ( vkEnumeratePhysicalDevices( m_instance->vkInstance, &devicesCount, availableDevices.data() ) != VK_SUCCESS ) { return false; }

		for ( int deviceIndex = 0; deviceIndex < availableDevices.size(); deviceIndex++ )
		{
			vkPhysicalDevice = availableDevices[ deviceIndex ];

			uint32_t extensionsCount;
			if ( vkEnumerateDeviceExtensionProperties( vkPhysicalDevice, nullptr, &extensionsCount, nullptr ) != VK_SUCCESS ) { return false; }
			m_availableExtensions.resize( extensionsCount );
			if ( vkEnumerateDeviceExtensionProperties( vkPhysicalDevice, nullptr, &extensionsCount, m_availableExtensions.data() ) != VK_SUCCESS ) { return false; }

			vkGetPhysicalDeviceProperties( vkPhysicalDevice, &m_deviceProperties );
			vkGetPhysicalDeviceFeatures( vkPhysicalDevice, &m_availableFeatures );

			if ( m_deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU )
			{
				break;
			}
		}
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	std::vector < const char*> Device::GetDesiredExtensions( const std::vector < const char*> _desiredExtensions )
	{
		std::vector < const char*> existingExtensions;
		existingExtensions.reserve( _desiredExtensions.size() );

		for ( int extensionIndex = 0; extensionIndex < _desiredExtensions.size(); extensionIndex++ )
		{
			if ( IsExtensionAvailable( _desiredExtensions[ extensionIndex ] ) )
			{
				existingExtensions.push_back( _desiredExtensions[ extensionIndex ] );
			}
		}
		return existingExtensions;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Device::IsExtensionAvailable( std::string _requiredExtension )
	{
		for ( int availableExtensionIndex = 0; availableExtensionIndex < m_availableExtensions.size(); availableExtensionIndex++ )
		{
			if ( _requiredExtension.compare( m_availableExtensions[ availableExtensionIndex ].extensionName ) == 0 )
			{
				return true;
			}
		}
		return false;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Device::GetQueueFamilies()
	{
		uint32_t queueFamiliesCount;
		vkGetPhysicalDeviceQueueFamilyProperties( vkPhysicalDevice, &queueFamiliesCount, nullptr );
		m_queueFamilyProperties.resize( queueFamiliesCount );
		vkGetPhysicalDeviceQueueFamilyProperties( vkPhysicalDevice, &queueFamiliesCount, m_queueFamilyProperties.data() );

		VkQueueFlags desiredGraphicsCapabilities = VK_QUEUE_GRAPHICS_BIT;
		VkQueueFlags desiredComputeCapabilities = VK_QUEUE_COMPUTE_BIT;

		for ( int queueIndex = 0; queueIndex < m_queueFamilyProperties.size(); queueIndex++ )
		{
			if ( ( m_queueFamilyProperties[ queueIndex ].queueCount > 0 ) &&
				( m_queueFamilyProperties[ queueIndex ].queueFlags & desiredGraphicsCapabilities ) )
			{
				m_graphicsQueueFamilyIndex = queueIndex;
				break;
			}
		}
		for ( int queueIndex = 0; queueIndex < m_queueFamilyProperties.size(); queueIndex++ )
		{
			if ( ( m_queueFamilyProperties[ queueIndex ].queueCount > 0 ) &&
				( m_queueFamilyProperties[ queueIndex ].queueFlags & desiredComputeCapabilities ) )
			{
				m_computeQueueFamilyIndex = queueIndex;
				break;
			}
		}

		for ( int queueIndex = 0; queueIndex < m_queueFamilyProperties.size(); queueIndex++ )
		{
			if ( m_queueFamilyProperties[ queueIndex ].queueCount > 0 )
			{
				VkBool32 presentationSupported;
				if ( vkGetPhysicalDeviceSurfaceSupportKHR( vkPhysicalDevice, queueIndex, m_surface, &presentationSupported ) == VK_SUCCESS &&
					 presentationSupported == VK_TRUE )
				{
					m_presentQueueFamilyIndex = queueIndex;
					break;
				}
			}
		}
	}
}