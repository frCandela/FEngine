#include "fanGlobalIncludes.h"

#include "renderer/core/fanBuffer.h"
#include "renderer/core/fanDevice.h"

namespace fan
{
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
	bool Buffer::Create(VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _memoryProperties, VkDeviceSize  _alignement ) {
		assert( _alignement > 0 );

		m_size = _size;
		m_alignment = _alignement;

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
			Debug::Error("Could not create buffer");
			return false;
		}
		//Debug::Get() << Debug::Severity::log << std::hex << "VkBuffer        " << m_buffer << std::dec << Debug::Endl();

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(m_device.vkDevice, m_buffer, &memoryRequirements);

		VkMemoryAllocateInfo bufferMemoryAllocateInfo;
		bufferMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		bufferMemoryAllocateInfo.pNext = nullptr;
		bufferMemoryAllocateInfo.allocationSize = memoryRequirements.size;
		bufferMemoryAllocateInfo.memoryTypeIndex = m_device.FindMemoryType(memoryRequirements.memoryTypeBits, _memoryProperties);

		if (vkAllocateMemory(m_device.vkDevice, &bufferMemoryAllocateInfo, nullptr, &m_memory) != VK_SUCCESS) {
			Debug::Error("Could not allocate buffer");
			return false;
		}
		//Debug::Get() << Debug::Severity::log << std::hex << "VkDeviceMemory        " << m_memory << std::dec << Debug::Endl();

		Bind();

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Buffer::SetData(const void * _data, VkDeviceSize _size, VkDeviceSize _offset) {
		assert( _size <= m_size);
		vkMapMemory(m_device.vkDevice, m_memory, _offset, _size, 0, &m_mappedData);
		memcpy(m_mappedData, _data, _size);
		vkUnmapMemory(m_device.vkDevice, m_memory);
	}

	//================================================================================================================================
	//================================================================================================================================
	VkResult Buffer::Bind(VkDeviceSize _offset)
	{
		VkResult result = vkBindBufferMemory(m_device.vkDevice, m_buffer, m_memory, _offset);
		if (result != VK_SUCCESS) {
			Debug::Error("Could not bind memory to buffer");
		}
		return result;
	}

	//================================================================================================================================
	//================================================================================================================================
	VkResult Buffer::Map(VkDeviceSize _size, VkDeviceSize _offset) {
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
	VkResult Buffer::Flush(VkDeviceSize _size, VkDeviceSize _offset)
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