#include "vulkan/Buffer.hpp"

namespace vk
{
	Buffer::Buffer(vk::Device& device) : m_device(device) {}

	Buffer::~Buffer()
	{
		if (m_buffer)
			vkDestroyBuffer(m_device.device, m_buffer, nullptr);

		if (memory)
			vkFreeMemory(m_device.device, memory, nullptr);
	}


	VkResult Buffer::Map(VkDeviceSize size, VkDeviceSize offset)
	{
		return vkMapMemory(m_device.device, memory, offset, size, 0, &mappedData);
	}


	void Buffer::Unmap()
	{
		if (mappedData)
		{
			vkUnmapMemory(m_device.device, memory);
			mappedData = nullptr;
		}
	}


	VkResult Buffer::Bind(VkDeviceSize offset)
	{
		return vkBindBufferMemory(m_device.device, m_buffer, memory, offset);
	}


	void Buffer::SetupDescriptor(VkDeviceSize size, VkDeviceSize offset)
	{
		descriptor.offset = offset;
		descriptor.buffer = m_buffer;
		descriptor.range = size;
	}

	VkResult Buffer::CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size)
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
		SetupDescriptor();

		// Attach the memory to the buffer object
		return Bind();
	}
}