#include "render/core/fanDevice.hpp"

#include <vector>
#include "render/core/fanInstance.hpp"
#include "core/fanDebug.hpp"
#include "render/fanRenderGlobal.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void Device::Create( Instance& _instance, VkSurfaceKHR _surface )
	{
		assert( mDevice == VK_NULL_HANDLE );

		VkPhysicalDeviceFeatures availableFeatures;
		std::vector<VkExtensionProperties>	availableExtensions;
		SelectPhysicalDevice( _instance, availableFeatures, availableExtensions );

		std::vector< const char* > existingExtensions = GetDesiredExtensions( availableExtensions, RenderGlobal::s_desiredDeviceExtensions );

		uint32_t graphicsQueueFamilyIndex = 0, computeQueueFamilyIndex = 0, presentQueueFamilyIndex = 0;
		GetQueueFamiliesIndices( _surface, graphicsQueueFamilyIndex, computeQueueFamilyIndex, presentQueueFamilyIndex );

		float queuePriority = 1.0f;
		std::vector <VkDeviceQueueCreateInfo> queueCreateInfos;

		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.pNext = nullptr;
		queueCreateInfo.flags = 0;
		queueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back( queueCreateInfo );

		VkPhysicalDeviceFeatures desiredFeatures = {};
		desiredFeatures.samplerAnisotropy = availableFeatures.samplerAnisotropy == VK_TRUE;
		desiredFeatures.shaderUniformBufferArrayDynamicIndexing = VK_TRUE;

		VkPhysicalDeviceDescriptorIndexingFeaturesEXT indexingFeatures = {};
		indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
		indexingFeatures.pNext = nullptr;
		//indexingFeatures.runtimeDescriptorArray = VK_TRUE;

		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pNext = &indexingFeatures;
		deviceCreateInfo.flags = 0;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>( queueCreateInfos.size() );
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>( _instance.mEnabledValidationLayers.size() );
		deviceCreateInfo.ppEnabledLayerNames = _instance.mEnabledValidationLayers.data();
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>( existingExtensions.size() );
		deviceCreateInfo.ppEnabledExtensionNames = existingExtensions.data();
		deviceCreateInfo.pEnabledFeatures = &desiredFeatures;

		if( vkCreateDevice( mPhysicalDevice, &deviceCreateInfo, nullptr, &mDevice ) != VK_SUCCESS )
		{
			Debug::Error() << "vulkan device creation failed" << Debug::Endl();
		}
		Debug::Log() << std::hex << "vkDevice:             " << mDeviceProperties.deviceName << std::dec << Debug::Endl();

		VkQueue		computeQueue = VK_NULL_HANDLE, presentQueue = VK_NULL_HANDLE;
		vkGetDeviceQueue( mDevice, graphicsQueueFamilyIndex, 0, &mGraphicsQueue );
		vkGetDeviceQueue( mDevice, computeQueueFamilyIndex, 0, &computeQueue );
		vkGetDeviceQueue( mDevice, presentQueueFamilyIndex, 0, &presentQueue );
		vkGetPhysicalDeviceMemoryProperties( mPhysicalDevice, &mMemoryProperties );

		// Creates command pool 
		assert( mCommandPool == VK_NULL_HANDLE );
		VkCommandPoolCreateInfo commandPoolCreateInfo;
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.pNext = nullptr;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;

		if( vkCreateCommandPool( mDevice, &commandPoolCreateInfo, nullptr, &mCommandPool ) != VK_SUCCESS )
		{
			Debug::Error( "Could not allocate command pool." );
		}
		Debug::Log() << std::hex << "VkCommandPool         " << mCommandPool << std::dec << Debug::Endl();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Device::Destroy()
	{
		vkDestroyCommandPool( mDevice, mCommandPool, nullptr );

		vkDestroyDevice( mDevice, nullptr );
		mDevice = VK_NULL_HANDLE;
	}

	//================================================================================================================================
	//================================================================================================================================
	VkCommandBuffer Device::BeginSingleTimeCommands()
	{
		// Allocate a temporary command buffer for memory transfer operations
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = mCommandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers( mDevice, &allocInfo, &commandBuffer );

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

		vkQueueSubmit( mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE );
		vkQueueWaitIdle( mGraphicsQueue );

		// Cleaning
		vkFreeCommandBuffers( mDevice, mCommandPool, 1, &_commandBuffer );
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Device::ResetCommandPool()
	{
		VkCommandPoolResetFlags releaseResources = VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT;

		if ( vkResetCommandPool( mDevice, mCommandPool, releaseResources ) != VK_SUCCESS )
		{
			Debug::Error( "Could not reset command pool." );
			return false;
		}
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	uint32_t Device::FindMemoryType( uint32_t _typeFilter, VkMemoryPropertyFlags _properties )
	{
		//check for the support of the properties
		for ( uint32_t propertyIndex = 0; propertyIndex < mMemoryProperties.memoryTypeCount; propertyIndex++ )
		{
			if ( ( _typeFilter & ( 1 << propertyIndex ) ) && ( mMemoryProperties.memoryTypes[ propertyIndex ].propertyFlags & _properties ) == _properties )
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

		for ( int candidateIndex = 0; candidateIndex < (int)candidates.size(); candidateIndex++ )
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties( mPhysicalDevice, candidates[ candidateIndex ], &props );

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
	bool Device::SelectPhysicalDevice( Instance& _instance, VkPhysicalDeviceFeatures& _outAvailableFeatures, std::vector<VkExtensionProperties>& _outAvailableExtensions )
	{
		uint32_t devicesCount;
		if ( vkEnumeratePhysicalDevices( _instance.mInstance, &devicesCount, nullptr ) != VK_SUCCESS ) { return false; }
		std::vector< VkPhysicalDevice> availableDevices( devicesCount );
		if ( vkEnumeratePhysicalDevices( _instance.mInstance, &devicesCount, availableDevices.data() ) != VK_SUCCESS ) { return false; }

		for ( int deviceIndex = 0; deviceIndex < (int)availableDevices.size(); deviceIndex++ )
		{
			mPhysicalDevice = availableDevices[ deviceIndex ];

			uint32_t extensionsCount;
			if ( vkEnumerateDeviceExtensionProperties( mPhysicalDevice, nullptr, &extensionsCount, nullptr ) != VK_SUCCESS ) { return false; }
			_outAvailableExtensions.resize( extensionsCount );
			if ( vkEnumerateDeviceExtensionProperties( mPhysicalDevice, nullptr, &extensionsCount, _outAvailableExtensions.data() ) != VK_SUCCESS ) { return false; }

			vkGetPhysicalDeviceProperties( mPhysicalDevice, &mDeviceProperties );
			vkGetPhysicalDeviceFeatures( mPhysicalDevice, &_outAvailableFeatures );

			if ( mDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU )
			{
				break;
			}
		}
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	std::vector < const char*> Device::GetDesiredExtensions( const std::vector<VkExtensionProperties>& _availableExtensions, const std::vector < const char*> _desiredExtensions )
	{
		std::vector < const char*> existingExtensions;
		existingExtensions.reserve( _desiredExtensions.size() );

		for ( int extensionIndex = 0; extensionIndex < (int)_desiredExtensions.size(); extensionIndex++ )
		{
			if ( IsExtensionAvailable( _availableExtensions, _desiredExtensions[ extensionIndex ] ) )
			{
				existingExtensions.push_back( _desiredExtensions[ extensionIndex ] );
			}
		}
		return existingExtensions;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Device::IsExtensionAvailable( const std::vector<VkExtensionProperties>& _availableExtensions, std::string _requiredExtension )
	{
		for ( int availableExtensionIndex = 0; availableExtensionIndex < (int)_availableExtensions.size(); availableExtensionIndex++ )
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
	void Device::GetQueueFamiliesIndices( VkSurfaceKHR _surface, uint32_t& _outGraphics, uint32_t& _outCompute, uint32_t& _outPresent )
	{
		uint32_t queueFamiliesCount;
		vkGetPhysicalDeviceQueueFamilyProperties( mPhysicalDevice, &queueFamiliesCount, nullptr );
		std::vector<VkQueueFamilyProperties>	queueFamilyProperties( queueFamiliesCount );
		vkGetPhysicalDeviceQueueFamilyProperties( mPhysicalDevice, &queueFamiliesCount, queueFamilyProperties.data() );

		VkQueueFlags desiredGraphicsCapabilities = VK_QUEUE_GRAPHICS_BIT;
		VkQueueFlags desiredComputeCapabilities = VK_QUEUE_COMPUTE_BIT;

		for ( int queueIndex = 0; queueIndex < (int)queueFamilyProperties.size(); queueIndex++ )
		{
			if ( ( queueFamilyProperties[ queueIndex ].queueCount > 0 ) &&
				( queueFamilyProperties[ queueIndex ].queueFlags & desiredGraphicsCapabilities ) )
			{
				_outGraphics = queueIndex;
				break;
			}
		}
		for ( int queueIndex = 0; queueIndex < (int)queueFamilyProperties.size(); queueIndex++ )
		{
			if ( ( queueFamilyProperties[ queueIndex ].queueCount > 0 ) &&
				( queueFamilyProperties[ queueIndex ].queueFlags & desiredComputeCapabilities ) )
			{
				_outCompute = queueIndex;
				break;
			}
		}

		for ( int queueIndex = 0; queueIndex < (int)queueFamilyProperties.size(); queueIndex++ )
		{
			if ( queueFamilyProperties[ queueIndex ].queueCount > 0 )
			{
				VkBool32 presentationSupported;
				if ( vkGetPhysicalDeviceSurfaceSupportKHR( mPhysicalDevice, queueIndex, _surface, &presentationSupported ) == VK_SUCCESS &&
					 presentationSupported == VK_TRUE )
				{
					_outPresent = queueIndex;
					break;
				}
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Device::AddDebugName( const uint64_t _object, const std::string& _name )
	{
		mDebugNames[_object] = _name;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Device::RemoveDebugName( const uint64_t _object )
	{
		auto it = mDebugNames.find( _object );
		if( it != mDebugNames.end() )
		{
			mDebugNames.erase( it );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	const std::string Device::GetDebugName( const uint64_t _object ) const
	{
		auto it = mDebugNames.find( _object );
		if( it != mDebugNames.end() )
		{
			return it->second;
		}
		else
		{
			return "";
		}		
	}
}