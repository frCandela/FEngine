#include "render/core/fanBuffer.hpp"

#include <cassert>
#include "render/core/fanDevice.hpp"
#include "core/fanDebug.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void Buffer::Destroy( Device& _device )
	{
		if ( memory != VK_NULL_HANDLE )
		{
			vkFreeMemory( _device.vkDevice, memory, nullptr );
			memory = VK_NULL_HANDLE;
		}

		if ( buffer != VK_NULL_HANDLE )
		{
			vkDestroyBuffer( _device.vkDevice, buffer, nullptr );
			buffer = VK_NULL_HANDLE;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Buffer::Create( Device& _device, VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _memoryProperties, VkDeviceSize  _alignement )
	{
		assert( buffer == VK_NULL_HANDLE );
		assert( memory == VK_NULL_HANDLE );
		assert( _alignement > 0 );

		size = _size;
		alignment = _alignement;

		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.pNext = nullptr;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = _size;
		bufferCreateInfo.usage = _usage;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		//bufferCreateInfo.queueFamilyIndexCount = 0;
		//bufferCreateInfo.pQueueFamilyIndices = nullptr;
		if ( vkCreateBuffer( _device.vkDevice, &bufferCreateInfo, nullptr, &buffer ) != VK_SUCCESS )
		{
			Debug::Error( "Could not create buffer" );
			return false;
		}
		//Debug::Get() << Debug::Severity::log << std::hex << "VkBuffer        " << m_buffer << std::dec << Debug::Endl();

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements( _device.vkDevice, buffer, &memoryRequirements );

		VkMemoryAllocateInfo bufferMemoryAllocateInfo;
		bufferMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		bufferMemoryAllocateInfo.pNext = nullptr;
		bufferMemoryAllocateInfo.allocationSize = memoryRequirements.size;
		bufferMemoryAllocateInfo.memoryTypeIndex = _device.FindMemoryType( memoryRequirements.memoryTypeBits, _memoryProperties );

		if ( vkAllocateMemory( _device.vkDevice, &bufferMemoryAllocateInfo, nullptr, &memory ) != VK_SUCCESS )
		{
			Debug::Error( "Could not allocate buffer" );
			return false;
		}
		//Debug::Get() << Debug::Severity::log << std::hex << "VkDeviceMemory        " << m_memory << std::dec << Debug::Endl();

		Bind( _device );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Buffer::SetData( Device& _device, const void* _data, VkDeviceSize _size, VkDeviceSize _offset )
	{
		assert( _size <= size );
		vkMapMemory( _device.vkDevice, memory, _offset, _size, 0, &mappedData );
		memcpy( mappedData, _data, (size_t)_size );
		vkUnmapMemory( _device.vkDevice, memory );
	}

	//================================================================================================================================
	//================================================================================================================================
	VkResult Buffer::Bind( Device& _device, VkDeviceSize _offset )
	{
		VkResult result = vkBindBufferMemory( _device.vkDevice, buffer, memory, _offset );
		if ( result != VK_SUCCESS )
		{
			Debug::Error( "Could not bind memory to buffer" );
		}
		return result;
	}

	//================================================================================================================================
	//================================================================================================================================
	VkResult Buffer::Map( Device& _device, VkDeviceSize _size, VkDeviceSize _offset )
	{
		return vkMapMemory( _device.vkDevice, memory, _offset, _size, 0, &mappedData );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Buffer::Unmap( Device& _device )
	{
		if ( mappedData && memory )
		{
			vkUnmapMemory( _device.vkDevice, memory );
			mappedData = nullptr;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	VkResult Buffer::Flush( Device& _device, VkDeviceSize _size, VkDeviceSize _offset )
	{
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = memory;
		mappedRange.offset = _offset;
		mappedRange.size = _size;
		return vkFlushMappedMemoryRanges( _device.vkDevice, 1, &mappedRange );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Buffer::CopyBufferTo( VkCommandBuffer _commandBuffer, VkBuffer _dstBuffer, VkDeviceSize _size )
	{
		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = _size;
		vkCmdCopyBuffer( _commandBuffer, buffer, _dstBuffer, 1, &copyRegion );
	}
}