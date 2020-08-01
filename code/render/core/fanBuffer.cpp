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
		if ( mMemory != VK_NULL_HANDLE )
		{
			vkFreeMemory( _device.mDevice, mMemory, nullptr );
			_device.RemoveDebugName( (uint64_t)mMemory );
			mMemory = VK_NULL_HANDLE;
		}

		if ( mBuffer != VK_NULL_HANDLE )
		{
			vkDestroyBuffer( _device.mDevice, mBuffer, nullptr );
			_device.RemoveDebugName( (uint64_t)mBuffer );
			mBuffer = VK_NULL_HANDLE;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Buffer::Create( Device& _device, VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _memoryProperties, VkDeviceSize  _alignement )
	{
		assert( mBuffer == VK_NULL_HANDLE );
		assert( mMemory == VK_NULL_HANDLE );
		assert( _alignement > 0 );

		mSize = _size;
		mAlignment = _alignement;

		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.pNext = nullptr;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = _size;
		bufferCreateInfo.usage = _usage;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		//bufferCreateInfo.queueFamilyIndexCount = 0;
		//bufferCreateInfo.pQueueFamilyIndices = nullptr;
		if ( vkCreateBuffer( _device.mDevice, &bufferCreateInfo, nullptr, &mBuffer ) != VK_SUCCESS )
		{
			Debug::Error( "Could not create buffer" );
			return false;
		}
		//Debug::Get() << Debug::Severity::log << std::hex << "VkBuffer              " << mBuffer << std::dec << Debug::Endl();

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements( _device.mDevice, mBuffer, &memoryRequirements );

		VkMemoryAllocateInfo bufferMemoryAllocateInfo;
		bufferMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		bufferMemoryAllocateInfo.pNext = nullptr;
		bufferMemoryAllocateInfo.allocationSize = memoryRequirements.size;
		bufferMemoryAllocateInfo.memoryTypeIndex = _device.FindMemoryType( memoryRequirements.memoryTypeBits, _memoryProperties );

		if ( vkAllocateMemory( _device.mDevice, &bufferMemoryAllocateInfo, nullptr, &mMemory ) != VK_SUCCESS )
		{
			Debug::Error( "Could not allocate buffer" );
			return false;
		}
		//Debug::Get() << Debug::Severity::log << std::hex << "VkDeviceMemory        " << mMemory << std::dec << Debug::Endl();

		Bind( _device );

		_device.AddDebugName( (uint64_t)mMemory, "fan Buffer_" );
		_device.AddDebugName( (uint64_t)mBuffer, "fan Buffer_" );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Buffer::SetData( Device& _device, const void* _data, VkDeviceSize _size, VkDeviceSize _offset )
	{
		assert( _size <= mSize );
		vkMapMemory( _device.mDevice, mMemory, _offset, _size, 0, &mappedData );
		memcpy( mappedData, _data, (size_t)_size );
		vkUnmapMemory( _device.mDevice, mMemory );
	}

	//================================================================================================================================
	//================================================================================================================================
	VkResult Buffer::Bind( Device& _device, VkDeviceSize _offset )
	{
		VkResult result = vkBindBufferMemory( _device.mDevice, mBuffer, mMemory, _offset );
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
		return vkMapMemory( _device.mDevice, mMemory, _offset, _size, 0, &mappedData );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Buffer::Unmap( Device& _device )
	{
		if ( mappedData && mMemory )
		{
			vkUnmapMemory( _device.mDevice, mMemory );
			mappedData = nullptr;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	VkResult Buffer::Flush( Device& _device, VkDeviceSize _size, VkDeviceSize _offset )
	{
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = mMemory;
		mappedRange.offset = _offset;
		mappedRange.size = _size;
		return vkFlushMappedMemoryRanges( _device.mDevice, 1, &mappedRange );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Buffer::CopyBufferTo( VkCommandBuffer _commandBuffer, VkBuffer _dstBuffer, VkDeviceSize _size )
	{
		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = _size;
		vkCmdCopyBuffer( _commandBuffer, mBuffer, _dstBuffer, 1, &copyRegion );
	}
}