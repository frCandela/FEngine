#include "render/core/fanImage.hpp"

#include "render/core/fanDevice.hpp"
#include "core/fanDebug.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Image::Destroy( Device& _device )
	{
		if ( mImage != VK_NULL_HANDLE )
		{
			vkDestroyImage( _device.mDevice, mImage, VK_NULL_HANDLE );
			_device.RemoveDebugName( (uint64_t)mImage );
			mImage = VK_NULL_HANDLE;
		}

		if ( mMemory != VK_NULL_HANDLE )
		{
			vkFreeMemory( _device.mDevice, mMemory, VK_NULL_HANDLE );
			_device.RemoveDebugName( (uint64_t)mMemory );
			mMemory = VK_NULL_HANDLE;
		}
	}

	//========================================================================================================
	//========================================================================================================
    bool Image::Create( Device& _device,
                        const VkFormat _format,
                        const VkExtent2D _size,
                        const VkImageUsageFlags _usage,
                        const VkMemoryPropertyFlags _memoryProperties )
	{
		assert( mImage == VK_NULL_HANDLE );
		assert( mMemory == VK_NULL_HANDLE );		

		VkImageCreateInfo imageCreateInfo;
		imageCreateInfo.sType					= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.pNext					= nullptr;
		imageCreateInfo.flags					= 0;
		imageCreateInfo.imageType				= VK_IMAGE_TYPE_2D;
		imageCreateInfo.format					= _format;
		imageCreateInfo.extent.width			= _size.width;
		imageCreateInfo.extent.height			= _size.height;
		imageCreateInfo.extent.depth			= 1;
		imageCreateInfo.mipLevels				= 1;
		imageCreateInfo.arrayLayers				= 1;
		imageCreateInfo.samples					= VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling					= VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage					= _usage;
		imageCreateInfo.sharingMode				= VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.queueFamilyIndexCount	= 0;
		imageCreateInfo.pQueueFamilyIndices		= nullptr;
		imageCreateInfo.initialLayout			= VK_IMAGE_LAYOUT_UNDEFINED;

		if ( vkCreateImage( _device.mDevice, &imageCreateInfo, VK_NULL_HANDLE, &mImage ) != VK_SUCCESS )
		{
			Debug::Error( "Could not allocate image" );
			return false;
		}
		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements( _device.mDevice, mImage, &memoryRequirements );

		VkMemoryAllocateInfo bufferMemoryAllocateInfo;
		bufferMemoryAllocateInfo.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		bufferMemoryAllocateInfo.pNext				= nullptr;
		bufferMemoryAllocateInfo.allocationSize		= memoryRequirements.size;
		bufferMemoryAllocateInfo.memoryTypeIndex	= _device.FindMemoryType( memoryRequirements.memoryTypeBits,
                                                                              _memoryProperties );

		if ( vkAllocateMemory( _device.mDevice, &bufferMemoryAllocateInfo, nullptr, &mMemory ) != VK_SUCCESS )
		{
			Debug::Error( "Could not allocate buffer" );
			return false;
		}

		if ( vkBindImageMemory( _device.mDevice, mImage, mMemory, 0 ) != VK_SUCCESS )
		{
			Debug::Error( "Could not bind memory to image" );
			return false;
		}
		Debug::Log() << std::hex << "VkImage               " << mImage << std::dec << Debug::Endl();
		Debug::Log() << "VkDeviceMemory        " << mMemory << std::dec << Debug::Endl();

		_device.AddDebugName( (uint64_t)mImage, "Image" );
		_device.AddDebugName( (uint64_t)mMemory, "Image" );

		return true;
	}

	//========================================================================================================
	//========================================================================================================
    void Image::TransitionImageLayout( VkCommandBuffer _commandBuffer,
                                       VkFormat _format,
                                       VkImageLayout _oldLayout,
                                       VkImageLayout _newLayout,
                                       uint32_t _mipLevels )
	{
		// Synchronize access to resources
		VkImageMemoryBarrier barrier = {};
		barrier.sType				= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout			= _oldLayout;
		barrier.newLayout			= _newLayout;
		barrier.srcQueueFamilyIndex	= VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex	= VK_QUEUE_FAMILY_IGNORED;
		barrier.image				= mImage;

		// Use the right subresource aspect
		if ( _newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL )
		{
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			const bool hasStencilComponent = _format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
			                                 _format == VK_FORMAT_D24_UNORM_S8_UINT;
			if( hasStencilComponent )
			{
				barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

		barrier.subresourceRange.baseMipLevel	= 0;
		barrier.subresourceRange.levelCount		= _mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount		= 1;

		// Set the access masks and pipeline stages based on the layouts in the transition.
		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if ( _oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL )
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage		 = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
        else if( _oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                 _newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL )
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
        else if( _oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
                 _newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL )
		{
			barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else
			throw std::invalid_argument( "unsupported layout transition!" );


		vkCmdPipelineBarrier(
			_commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
	}
}