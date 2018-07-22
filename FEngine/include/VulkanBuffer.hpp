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

#include "vulkan/vulkan.h"
#include "VulkanTools.h"

#include "vulkan/Device.h"

namespace vks
{	
	/**
	* @brief Encapsulates access to a Vulkan buffer backed up by device memory
	* @note To be filled by an external source like the VulkanDevice
	*/
	struct Buffer
	{
		//Buffer( vk::Device& device ) : m_device(device){}

		VkDevice m_device;
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
			return vkMapMemory(m_device, memory, offset, size, 0, &mappedData);
		}


		//Unmap a mapped memory range
		void unmap()
		{
			if (mappedData)
			{
				vkUnmapMemory(m_device, memory);
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
			return vkBindBufferMemory(m_device, m_buffer, memory, offset);
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
				vkDestroyBuffer(m_device, m_buffer, nullptr);
			}
			if (memory)
			{
				vkFreeMemory(m_device, memory, nullptr);
			}
		}

		VkResult createBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size, vk::Device& dev)
		{
			m_device = dev.device;

			// Create the buffer handle
			VkBufferCreateInfo bufferCreateInfo = {};
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreateInfo.size = size;
			bufferCreateInfo.usage = usageFlags;

			VK_CHECK_RESULT(vkCreateBuffer(dev.device, &bufferCreateInfo, nullptr, &m_buffer));

			// Create the memory backing up the buffer handle
			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(dev.device, m_buffer, &memRequirements);

			VkMemoryAllocateInfo memAlloc = {};
			memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memAlloc.allocationSize = memRequirements.size;
			memAlloc.memoryTypeIndex = dev.findMemoryType(memRequirements.memoryTypeBits, memoryPropertyFlags);			// Find a memory type index that fits the properties of the buffer

			VK_CHECK_RESULT(vkAllocateMemory(dev.device, &memAlloc, nullptr, &memory));

			alignment = memRequirements.alignment;
			m_size = memAlloc.allocationSize;
			m_usageFlags = usageFlags;
			m_memoryPropertyFlags = memoryPropertyFlags;

			// Initialize a default descriptor that covers the whole buffer size
			setupDescriptor();

			// Attach the memory to the buffer object
			return bind();
		}

		// Helper function for creating buffers
		static void createBuffer(vk::Device& dev, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryPropertyFlags, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
		{
			// Buffer info structure
			VkBufferCreateInfo bufferCreateInfo = {};
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreateInfo.size = size;
			bufferCreateInfo.usage = usage;
			//bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if (vkCreateBuffer(dev.device, &bufferCreateInfo, nullptr, &buffer) != VK_SUCCESS)
				throw std::runtime_error("failed to create buffer!");

			// Query memory requirements
			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(dev.device, buffer, &memRequirements);

			// Allow memory to buffer
			VkMemoryAllocateInfo memAlloc = {};
			memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memAlloc.allocationSize = memRequirements.size;
			memAlloc.memoryTypeIndex = dev.findMemoryType(memRequirements.memoryTypeBits, memoryPropertyFlags);

			if (vkAllocateMemory(dev.device, &memAlloc, nullptr, &bufferMemory) != VK_SUCCESS)
				throw std::runtime_error("failed to allocate buffer memory!");

			vkBindBufferMemory(dev.device, buffer, bufferMemory, 0);
		}
	};
}