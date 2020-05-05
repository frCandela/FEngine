#include "render/core/fanImageView.hpp"

#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	ImageView::ImageView( Device& _device ) :
		m_device( _device )
	{}

	//================================================================================================================================
	//================================================================================================================================
	ImageView::~ImageView()
	{
		Destroy();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ImageView::Destroy()
	{
		if ( m_imageView != VK_NULL_HANDLE )
		{
			vkDestroyImageView( m_device.vkDevice, m_imageView, nullptr );
			m_imageView = VK_NULL_HANDLE;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ImageView::SetImage( VkImage _image )
	{
		Destroy();
		Create( _image, m_format, m_aspectFlags, m_viewType );
	}

	//================================================================================================================================
	//================================================================================================================================
	bool ImageView::Create( VkImage _image, const VkFormat _format, const VkImageAspectFlags _aspectFlags, const VkImageViewType _viewType )
	{
		m_format = _format;
		m_aspectFlags = _aspectFlags;
		m_viewType = _viewType;

		VkImageViewCreateInfo imageViewCreateInfo;
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = nullptr;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.image = _image;
		imageViewCreateInfo.viewType = _viewType;
		imageViewCreateInfo.format = _format;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = _aspectFlags;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

		VkResult result = vkCreateImageView( m_device.vkDevice, &imageViewCreateInfo, nullptr, &m_imageView );
		Debug::Get() << Debug::Severity::log << std::hex << "VkImageView           " << m_imageView << std::dec << Debug::Endl();
		return result == VK_SUCCESS;
	}
}