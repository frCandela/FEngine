#include "vulkan/DepthImage.h"

namespace vk
{

	DepthImage::DepthImage(Device& device) :
		Image(device)
	{

	}

	//Select a format with a depth component that supports usage as depth attachment
	VkFormat DepthImage::findDepthFormat()
	{
		return findSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	// Create a depth image, memory and view
	void DepthImage::createDepthResources(uint32_t width, uint32_t height, CommandPool& rCommandPool)
	{
		VkFormat depthFormat = findDepthFormat();

		createImage(width, height, 1, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, deviceMemory);
		imageView = Image::createImageView(image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, m_device.device);

		// Setup a pipeline barrier for the transition
		transitionImageLayout(depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1, rCommandPool);
	}

	// Select a format for the depth buffer 
	VkFormat DepthImage::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
	{
		for (VkFormat format : candidates)
		{
			// Query support of a format
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(m_device.physicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
				return format;

			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
				return format;
		}

		throw std::runtime_error("failed to find supported format!");
	}
}