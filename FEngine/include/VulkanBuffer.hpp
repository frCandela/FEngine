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
		VkDevice m_device;
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
		VkDescriptorBufferInfo descriptor;
		VkDeviceSize size = 0;
		VkDeviceSize alignment = 0;
		void* mapped = nullptr;

		/** @brief Usage flags to be filled by external source at buffer creation (to query at some later point) */
		VkBufferUsageFlags usageFlags;
		/** @brief Memory propertys flags to be filled by external source at buffer creation (to query at some later point) */
		VkMemoryPropertyFlags memoryPropertyFlags;

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
			return vkMapMemory(m_device, memory, offset, size, 0, &mapped);
		}

		/**
		* Unmap a mapped memory range
		*
		* @note Does not return a result as vkUnmapMemory can't fail
		*/
		void unmap()
		{
			if (mapped)
			{
				vkUnmapMemory(m_device, memory);
				mapped = nullptr;
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
			return vkBindBufferMemory(m_device, buffer, memory, offset);
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
			descriptor.buffer = buffer;
			descriptor.range = size;
		}

		/** 
		* Release all Vulkan resources held by this buffer
		*/
		void destroy()
		{
			if (buffer)
			{
				vkDestroyBuffer(m_device, buffer, nullptr);
			}
			if (memory)
			{
				vkFreeMemory(m_device, memory, nullptr);
			}
		}

		VkResult createBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, vks::Buffer *buffer, VkDeviceSize size, vk::Device& dev)
		{
			buffer->m_device = dev.device;

			// Create the buffer handle
			VkBufferCreateInfo bufferCreateInfo = vks::initializers::bufferCreateInfo(usageFlags, size);
			VK_CHECK_RESULT(vkCreateBuffer(dev.device, &bufferCreateInfo, nullptr, &buffer->buffer));

			// Create the memory backing up the buffer handle
			VkMemoryRequirements memReqs;
			VkMemoryAllocateInfo memAlloc = vks::initializers::memoryAllocateInfo();
			vkGetBufferMemoryRequirements(dev.device, buffer->buffer, &memReqs);
			memAlloc.allocationSize = memReqs.size;
			// Find a memory type index that fits the properties of the buffer
			memAlloc.memoryTypeIndex = dev.findMemoryType(memReqs.memoryTypeBits, memoryPropertyFlags);

			VK_CHECK_RESULT(vkAllocateMemory(dev.device, &memAlloc, nullptr, &buffer->memory));

			buffer->alignment = memReqs.alignment;
			buffer->size = memAlloc.allocationSize;
			buffer->usageFlags = usageFlags;
			buffer->memoryPropertyFlags = memoryPropertyFlags;

			// Initialize a default descriptor that covers the whole buffer size
			buffer->setupDescriptor();

			// Attach the memory to the buffer object
			return buffer->bind();
		}

		// Helper function for creating buffers
		static void createBuffer(vk::Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
		{
			// Buffer info structure
			VkBufferCreateInfo bufferInfo = {};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = usage;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if (vkCreateBuffer(device.device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
				throw std::runtime_error("failed to create buffer!");

			// Query memory requirements
			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(device.device, buffer, &memRequirements);

			// Allow memory to buffer
			VkMemoryAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = device.findMemoryType(memRequirements.memoryTypeBits, properties);

			if (vkAllocateMemory(device.device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
				throw std::runtime_error("failed to allocate buffer memory!");

			vkBindBufferMemory(device.device, buffer, bufferMemory, 0);
		}
	};
}