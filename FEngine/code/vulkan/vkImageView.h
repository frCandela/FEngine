#pragma once

#include "AllInclude.h"

#include "vkDevice.h"

namespace vk {
	class ImageView {
	public:
		ImageView(Device * _device) :
			m_device(_device) {
		}

		~ImageView() {
			if (m_imageView != VK_NULL_HANDLE) {
				vkDestroyImageView(m_device->vkDevice, m_imageView, nullptr);
				m_imageView = VK_NULL_HANDLE;
			}
		}

		bool Create(VkImage _image, VkFormat _format, VkImageAspectFlags _aspectFlags) {

			VkImageViewCreateInfo imageViewCreateInfo;
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.pNext = nullptr;
			imageViewCreateInfo.flags = 0;
			imageViewCreateInfo.image = _image;
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
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

			VkResult result = vkCreateImageView(m_device->vkDevice, &imageViewCreateInfo, nullptr, &m_imageView);
			std::cout << std::hex << "VkImageView\t\t" << m_imageView << std::dec << std::endl;
			return result == VK_SUCCESS;
		}

		VkImageView GetImageView() { return m_imageView; }
	private:
		Device * m_device;
		VkImageView m_imageView;
	};
}