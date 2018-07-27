#include "vulkan/DepthImage.h"

namespace vk
{

	DepthImage::DepthImage(Device& device, CommandPool& rCommandPool) :
		Image(device, rCommandPool)
	{

	}

	
	VkFormat DepthImage::FindDepthFormat()
	{
		return FindSupportedFormat(
		{ 
			VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	
	void DepthImage::CreateDepthResources(uint32_t width, uint32_t height)
	{
		VkFormat depthFormat = FindDepthFormat();

		CreateImage(width, height, 1, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, deviceMemory);
		imageView = Image::CreateImageView(image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, m_device.device);

		// Setup a pipeline barrier for the transition
		TransitionImageLayout(depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
	}
	
	VkFormat DepthImage::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
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