#pragma once

#include "glfw/glfw3.h"

namespace fan
{
	struct Device;

	//================================================================================================================================
	// abstraction of a gpu buffer
	//================================================================================================================================
	struct Buffer
	{	
		bool Create( Device& _device, VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _memoryProperties, VkDeviceSize  _alignement = 1 );
		void Destroy( Device& _device );

		void		SetData( Device& _device, const void* _data, VkDeviceSize _size, VkDeviceSize _offset = 0 );
		VkResult	Bind( Device& _device, VkDeviceSize _offset = 0 );
		VkResult	Map( Device& _device, VkDeviceSize _size = VK_WHOLE_SIZE, VkDeviceSize _offset = 0 );
		void		Unmap( Device& _device );
		VkResult	Flush( Device& _device, VkDeviceSize _size = VK_WHOLE_SIZE, VkDeviceSize _offset = 0 );
		void		CopyBufferTo( VkCommandBuffer _commandBuffer, VkBuffer _dstBuffer, VkDeviceSize _size );

		VkBuffer		buffer = VK_NULL_HANDLE;
		VkDeviceMemory	memory = VK_NULL_HANDLE;
		VkDeviceSize	size = 0;
		VkDeviceSize	alignment = 0;

		void* mappedData;
	};
}