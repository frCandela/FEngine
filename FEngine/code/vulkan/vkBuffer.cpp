#include "Includes.h"

#include "vulkan/vkBuffer.h"
#include "vulkan/vkDevice.h"

namespace vk {
	//================================================================================================================================
	//================================================================================================================================
	Buffer::Buffer(Device & _device) :
		m_device(_device) {
	}

	//================================================================================================================================
	//================================================================================================================================
	Buffer::~Buffer() {
		Destroy();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Buffer::Destroy() {
		if (m_memory != VK_NULL_HANDLE) {
			vkFreeMemory(m_device.vkDevice, m_memory, nullptr);
			m_memory = VK_NULL_HANDLE;
		}

		if (m_buffer != VK_NULL_HANDLE) {
			vkDestroyBuffer(m_device.vkDevice, m_buffer, nullptr);
			m_buffer = VK_NULL_HANDLE;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Buffer::Create(VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _memoryProperties) {

		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.pNext = nullptr;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = _size;
		bufferCreateInfo.usage = _usage;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		//bufferCreateInfo.queueFamilyIndexCount = 0;
		//bufferCreateInfo.pQueueFamilyIndices = nullptr;
		if (vkCreateBuffer(m_device.vkDevice, &bufferCreateInfo, nullptr, &m_buffer) != VK_SUCCESS) {
			std::cout << "Could not create buffer" << std::endl;
			return false;
		}
		//std::cout << std::hex << "VkBuffer\t\t" << m_buffer << std::dec << std::endl;

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(m_device.vkDevice, m_buffer, &memoryRequirements);

		VkMemoryAllocateInfo bufferMemoryAllocateInfo;
		bufferMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		bufferMemoryAllocateInfo.pNext = nullptr;
		bufferMemoryAllocateInfo.allocationSize = memoryRequirements.size;
		bufferMemoryAllocateInfo.memoryTypeIndex = m_device.FindMemoryType(memoryRequirements.memoryTypeBits, _memoryProperties);

		if (vkAllocateMemory(m_device.vkDevice, &bufferMemoryAllocateInfo, nullptr, &m_memory) != VK_SUCCESS) {
			std::cout << "Could not allocate buffer" << std::endl;
			return false;
		}
		//std::cout << std::hex << "VkDeviceMemory\t\t" << m_memory << std::dec << std::endl;

		Bind();

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Buffer::SetData(const void * _data, VkDeviceSize _size) {
		vkMapMemory(m_device.vkDevice, m_memory, 0, _size, 0, &m_mappedData);
		memcpy(m_mappedData, _data, _size);
		vkUnmapMemory(m_device.vkDevice, m_memory);
	}

	//================================================================================================================================
	//================================================================================================================================
	VkResult Buffer::Bind(VkDeviceSize _offset)
	{
		VkResult result = vkBindBufferMemory(m_device.vkDevice, m_buffer, m_memory, _offset);
		if (result != VK_SUCCESS) {
			std::cout << "Could not bind memory to buffer" << std::endl;
		}
		return result;
	}

	//================================================================================================================================
	//================================================================================================================================
	VkResult Buffer::Map(VkDeviceSize _size , VkDeviceSize _offset) {
		return vkMapMemory(m_device.vkDevice, m_memory, _offset, _size, 0, &m_mappedData);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Buffer::Unmap()
	{
		if (m_mappedData && m_memory)
		{
			vkUnmapMemory(m_device.vkDevice, m_memory);
			m_mappedData = nullptr;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	VkResult Buffer::Flush(VkDeviceSize _size, VkDeviceSize _offset )
	{
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = m_memory;
		mappedRange.offset = _offset;
		mappedRange.size = _size;
		return vkFlushMappedMemoryRanges(m_device.vkDevice, 1, &mappedRange);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Buffer::CopyBufferTo(VkCommandBuffer _commandBuffer, VkBuffer _dstBuffer, VkDeviceSize _size) {
		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = _size;
		vkCmdCopyBuffer(_commandBuffer, m_buffer, _dstBuffer, 1, &copyRegion);
	}

}