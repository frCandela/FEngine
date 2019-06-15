#pragma once

#include "AllInclude.h"

#include "vkDevice.h"

class Buffer {
public:
	Buffer( Device * _device) :
		m_device(_device){
	}

	~Buffer() {
		if ( m_bufferMemory != VK_NULL_HANDLE ) { 
			vkFreeMemory(m_device->vkDevice, m_bufferMemory, nullptr);  
			m_bufferMemory = VK_NULL_HANDLE; 
		}

		if ( m_buffer !=  VK_NULL_HANDLE ) { 
			vkDestroyBuffer(m_device->vkDevice, m_buffer, nullptr);
			m_buffer = VK_NULL_HANDLE;
		}

	}
	bool Create(VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlagBits _memoryProperties) {

		VkBufferCreateInfo bufferCreateInfo;
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.pNext = nullptr;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = _size;
		bufferCreateInfo.usage = _usage;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.queueFamilyIndexCount = 0;
		bufferCreateInfo.pQueueFamilyIndices = nullptr;
		if (vkCreateBuffer(m_device->vkDevice, &bufferCreateInfo, nullptr, &m_buffer) != VK_SUCCESS) {
			std::cout << "Could not create buffer" << std::endl;
			return false;
		}
		std::cout << std::hex << "VkBuffer\t\t" << m_buffer << std::dec << std::endl;		

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(m_device->vkDevice, m_buffer, &memoryRequirements);

		VkMemoryAllocateInfo bufferMemoryAllocateInfo;
		bufferMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		bufferMemoryAllocateInfo.pNext = nullptr;
		bufferMemoryAllocateInfo.allocationSize = memoryRequirements.size;
		bufferMemoryAllocateInfo.memoryTypeIndex = m_device->FindMemoryType(memoryRequirements.memoryTypeBits, _memoryProperties);

		if (vkAllocateMemory(m_device->vkDevice, &bufferMemoryAllocateInfo, nullptr, &m_bufferMemory) != VK_SUCCESS) {
			std::cout << "Could not allocate buffer" << std::endl;
			return false;
		}
		std::cout << std::hex << "VkDeviceMemory\t" << m_bufferMemory << std::dec << std::endl;

		if (vkBindBufferMemory(m_device->vkDevice, m_buffer, m_bufferMemory, 0) != VK_SUCCESS) {
			std::cout << "Could not bind memory to buffer" << std::endl;
			return false;
		}

		return true;
	}
private:
	Device * m_device;
	VkBuffer m_buffer;
	VkDeviceMemory m_bufferMemory;

	
};