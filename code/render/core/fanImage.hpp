#pragma once

#include "glfw/glfw3.h"

namespace fan
{
	struct Device;

	//========================================================================================================
	// 2D image with optional mipmaps
	//========================================================================================================
	struct Image
	{
        bool Create( Device& _device,
                     const VkFormat _format,
                     const VkExtent2D _size,
                     const VkImageUsageFlags _usage,
                     const VkMemoryPropertyFlags _memoryProperties );
        void Destroy( Device& _device );
        void TransitionImageLayout( VkCommandBuffer _commandBuffer,
                                    VkFormat _format,
                                    VkImageLayout _oldLayout,
                                    VkImageLayout _newLayout,
                                    uint32_t _mipLevels );

		VkImage			mImage	= VK_NULL_HANDLE;
		VkDeviceMemory	mMemory	= VK_NULL_HANDLE;
	};
}