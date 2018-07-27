#pragma once

#include "Image.h"
#include "CommandPool.h"

namespace vk
{
	class DepthImage : public Image
	{
	public:
		DepthImage(Device& device);
		void createDepthResources(uint32_t width, uint32_t height, CommandPool& rCommandPool);
		VkFormat findDepthFormat();

	private:
		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	};

}

