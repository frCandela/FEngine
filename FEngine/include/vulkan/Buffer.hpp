/*
* Vulkan buffer class
*
* Encapsulates a Vulkan buffer
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once

#include <vector>

#include "Util.h"

#include "vulkan/Device.h"

namespace vks
{	
	/**
	* @brief Encapsulates access to a Vulkan buffer backed up by device memory
	* @note To be filled by an external source like the VulkanDevice
	*/
	struct Buffer
	{
		Buffer( vk::Device& device ) : m_device(device){}


		vk::Device& m_device;

		VkBuffer m_buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
		VkDescriptorBufferInfo descriptor;
		VkDeviceSize m_size = 0;
		VkDeviceSize alignment = 0;
		void* mappedData = nullptr;

		VkBufferUsageFlags m_usageFlags;
		VkMemoryPropertyFlags m_memoryPropertyFlags;

		/** 
		* Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
		* 
		* @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete buffer range.
		* @param offset (Optional) Byte offset from beginning
		* 
		* @return VkResult of the buffer mapping call
		*/
		VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0)
		{
			return vkMapMemory(m_device.device, memory, offset, size, 0, &mappedData);
		}


		//Unmap a mapped memory range
		void unmap()
		{
			if (mappedData)
			{
				vkUnmapMemory(m_device.device, memory);
				mappedData = nullptr;
			}
		}

		/** 
		* Attach the allocated memory block to the buffer
		* 
		* @param offset (Optional) Byte offset (from the beginning) for the memory region to bind
		* 
		* @return VkResult of the bindBufferMemory call
		*/
		VkResult bind(VkDeviceSize offset = 0)
		{
			return vkBindBufferMemory(m_device.device, m_buffer, memory, offset);
		}

		/**
		* Setup the default descriptor for this buffer
		*
		* @param size (Optional) Size of the memory range of the descriptor
		* @param offset (Optional) Byte offset from beginning
		*
		*/
		void setupDescriptor(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0)
		{
			descriptor.offset = offset;
			descriptor.buffer = m_buffer;
			descriptor.range = size;
		}

		/** 
		* Release all Vulkan resources held by this buffer
		*/
		void destroy()
		{
			if (m_buffer)
			{
				vkDestroyBuffer(m_device.device, m_buffer, nullptr);
			}
			if (memory)
			{
				vkFreeMemory(m_device.device, memory, nullptr);
			}
		}

		VkResult createBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size)
		{
			// Create the buffer handle
			VkBufferCreateInfo bufferCreateInfo = {};
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreateInfo.size = size;
			bufferCreateInfo.usage = usageFlags;

			VK_CHECK_RESULT(vkCreateBuffer(m_device.device, &bufferCreateInfo, nullptr, &m_buffer));

			// Create the memory backing up the buffer handle
			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(m_device.device, m_buffer, &memRequirements);

			VkMemoryAllocateInfo memAlloc = {};
			memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memAlloc.allocationSize = memRequirements.size;
			memAlloc.memoryTypeIndex = m_device.findMemoryType(memRequirements.memoryTypeBits, memoryPropertyFlags);			// Find a memory type index that fits the properties of the buffer

			VK_CHECK_RESULT(vkAllocateMemory(m_device.device, &memAlloc, nullptr, &memory));

			alignment = memRequirements.alignment;
			m_size = memAlloc.allocationSize;
			m_usageFlags = usageFlags;
			m_memoryPropertyFlags = memoryPropertyFlags;

			// Initialize a default descriptor that covers the whole buffer size
			setupDescriptor();

			// Attach the memory to the buffer object
			return bind();
		}
	};
}