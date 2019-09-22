#pragma once

#include "fanGlobalIncludes.h"

namespace fan
{
	class Device;

	//================================================================================================================================
	//================================================================================================================================
	class Buffer {
	public:
		Buffer(Device & _device);
		~Buffer();

		void Destroy();
		bool Create(VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _memoryProperties);

		void		SetData(const void * _data, VkDeviceSize _size, VkDeviceSize _offset = 0);
		VkResult	Bind(VkDeviceSize _offset = 0);
		VkResult	Map(VkDeviceSize _size = VK_WHOLE_SIZE, VkDeviceSize _offset = 0);
		void		Unmap();
		VkResult	Flush(VkDeviceSize _size = VK_WHOLE_SIZE, VkDeviceSize _offset = 0);
		void		CopyBufferTo(VkCommandBuffer _commandBuffer, VkBuffer _dstBuffer, VkDeviceSize _size);

		VkBuffer		GetBuffer() { return m_buffer; }
		VkDeviceMemory	GetMemory() { return m_memory; }
		void *			GetMappedData() { return m_mappedData; }

	private:
		Device & m_device;

		VkBuffer		m_buffer = VK_NULL_HANDLE;
		VkDeviceMemory	m_memory = VK_NULL_HANDLE;
		VkDeviceSize	m_size = 0;
		VkDeviceSize	m_alignment = 0;
		void *			m_mappedData;
	};
}