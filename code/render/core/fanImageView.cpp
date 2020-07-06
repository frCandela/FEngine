#include "render/core/fanImageView.hpp"

#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void ImageView::Destroy( Device& _device )
	{
		if ( imageView != VK_NULL_HANDLE )
		{
			vkDestroyImageView( _device.vkDevice, imageView, nullptr );
			imageView = VK_NULL_HANDLE;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool ImageView::Create( Device& _device, VkImage _image, const VkFormat _format, const VkImageAspectFlags _aspectFlags, const VkImageViewType _viewType )
	{
		assert( imageView == VK_NULL_HANDLE );

		VkImageViewCreateInfo imageViewCreateInfo;
		imageViewCreateInfo.sType							= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext							= nullptr;
		imageViewCreateInfo.flags							= 0;
		imageViewCreateInfo.image							= _image;
		imageViewCreateInfo.viewType						= _viewType;
		imageViewCreateInfo.format							= _format;
		imageViewCreateInfo.components.a					= VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b					= VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g					= VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.r					= VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask		= _aspectFlags;
		imageViewCreateInfo.subresourceRange.baseMipLevel	= 0;
		imageViewCreateInfo.subresourceRange.levelCount		= VK_REMAINING_MIP_LEVELS;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount		= VK_REMAINING_ARRAY_LAYERS;

		VkResult result = vkCreateImageView( _device.vkDevice, &imageViewCreateInfo, nullptr, &imageView );
		Debug::Get() << Debug::Severity::log << std::hex << "VkImageView           " << imageView << std::dec << Debug::Endl();
		return result == VK_SUCCESS;
	}
}