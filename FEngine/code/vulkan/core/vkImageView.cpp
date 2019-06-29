#include "fanIncludes.h"

#include "vulkan/core/vkImageView.h"
#include "vulkan/core/vkDevice.h"

namespace vk {

	//================================================================================================================================
	//================================================================================================================================
	ImageView::ImageView(Device & _device) :
		m_device(_device) {
	}

	//================================================================================================================================
	//================================================================================================================================
	ImageView::~ImageView() {
		if (m_imageView != VK_NULL_HANDLE) {
			vkDestroyImageView(m_device.vkDevice, m_imageView, nullptr);
			m_imageView = VK_NULL_HANDLE;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool ImageView::Create(VkImage _image, VkFormat _format, VkImageAspectFlags _aspectFlags, VkImageViewType _viewType) {

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

		VkResult result = vkCreateImageView(m_device.vkDevice, &imageViewCreateInfo, nullptr, &m_imageView);
		std::cout << std::hex << "VkImageView\t\t" << m_imageView << std::dec << std::endl;
		return result == VK_SUCCESS;
	}
}