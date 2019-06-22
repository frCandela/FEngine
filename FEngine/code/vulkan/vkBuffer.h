#pragma once

#include "AllInclude.h"

#include "vkDevice.h"

namespace vk {
	class Buffer {
	public:
		Buffer(Device * _device) :
			m_device(_device) {
		}
		~Buffer() {
			Destroy();
		}
		void Destroy() {
			if (m_memory != VK_NULL_HANDLE) {
				vkFreeMemory(m_device->vkDevice, m_memory, nullptr);
				m_memory = VK_NULL_HANDLE;
			}

			if (m_buffer != VK_NULL_HANDLE) {
				vkDestroyBuffer(m_device->vkDevice, m_buffer, nullptr);
				m_buffer = VK_NULL_HANDLE;
			}
		}
		bool Create(VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _memoryProperties) {

			VkBufferCreateInfo bufferCreateInfo = {};
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreateInfo.pNext = nullptr;
			bufferCreateInfo.flags = 0;
			bufferCreateInfo.size = _size;
			bufferCreateInfo.usage = _usage;
			bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			//bufferCreateInfo.queueFamilyIndexCount = 0;
			//bufferCreateInfo.pQueueFamilyIndices = nullptr;
			if (vkCreateBuffer(m_device->vkDevice, &bufferCreateInfo, nullptr, &m_buffer) != VK_SUCCESS) {
				std::cout << "Could not create buffer" << std::endl;
				return false;
			}
			//std::cout << std::hex << "VkBuffer\t\t" << m_buffer << std::dec << std::endl;

			VkMemoryRequirements memoryRequirements;
			vkGetBufferMemoryRequirements(m_device->vkDevice, m_buffer, &memoryRequirements);

			VkMemoryAllocateInfo bufferMemoryAllocateInfo;
			bufferMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			bufferMemoryAllocateInfo.pNext = nullptr;
			bufferMemoryAllocateInfo.allocationSize = memoryRequirements.size;
			bufferMemoryAllocateInfo.memoryTypeIndex = m_device->FindMemoryType(memoryRequirements.memoryTypeBits, _memoryProperties);

			if (vkAllocateMemory(m_device->vkDevice, &bufferMemoryAllocateInfo, nullptr, &m_memory) != VK_SUCCESS) {
				std::cout << "Could not allocate buffer" << std::endl;
				return false;
			}
			//std::cout << std::hex << "VkDeviceMemory\t\t" << m_memory << std::dec << std::endl;

			Bind();

			return true;
		}
		
		void SetData(const void * _data, VkDeviceSize _size) {
			vkMapMemory(m_device->vkDevice, m_memory, 0, _size, 0, &m_mappedData);
			memcpy(m_mappedData, _data, _size);
			vkUnmapMemory(m_device->vkDevice, m_memory);
		}
		VkResult Bind(VkDeviceSize _offset = 0)
		{
			VkResult result = vkBindBufferMemory(m_device->vkDevice, m_buffer, m_memory, _offset);
			if (result != VK_SUCCESS) {
				std::cout << "Could not bind memory to buffer" << std::endl;
			}
			return result;
		}
		VkResult Map(VkDeviceSize _size = VK_WHOLE_SIZE, VkDeviceSize _offset = 0) {
			return vkMapMemory(m_device->vkDevice, m_memory, _offset, _size, 0, &m_mappedData);
		}
		void Unmap()
		{
			if (m_mappedData)
			{
				vkUnmapMemory(m_device->vkDevice, m_memory);
				m_mappedData = nullptr;
			}
		}
		VkResult Flush(VkDeviceSize _size = VK_WHOLE_SIZE, VkDeviceSize _offset = 0)
		{
			VkMappedMemoryRange mappedRange = {};
			mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			mappedRange.memory = m_memory;
			mappedRange.offset = _offset;
			mappedRange.size = _size;
			return vkFlushMappedMemoryRanges(m_device->vkDevice, 1, &mappedRange);
		}
		void CopyBufferTo(VkCommandBuffer _commandBuffer, VkBuffer _dstBuffer, VkDeviceSize _size) {
			VkBufferCopy copyRegion = {};
			copyRegion.srcOffset = 0;
			copyRegion.dstOffset = 0;
			copyRegion.size = _size;
			vkCmdCopyBuffer(_commandBuffer, m_buffer, _dstBuffer, 1, &copyRegion);
		}

		VkBuffer GetBuffer() { return m_buffer; }
		VkDeviceMemory GetMemory() { return m_memory; }
		void * GetMappedData() { return m_mappedData;}		
	
	private:
		Device * m_device;

		VkBuffer m_buffer;
		VkDeviceMemory m_memory;

		void * m_mappedData;
		VkDeviceSize m_size = 0;
		VkDeviceSize m_alignment = 0;
	};
}