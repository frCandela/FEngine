#pragma once

#include "Image.h"
#include "CommandPool.h"

namespace vk
{
	class DepthImage : public Image
	{
	public:
		DepthImage(Device& device, CommandPool& rCommandPool);

		// Create a depth image, memory and view
		void CreateDepthResources(uint32_t width, uint32_t height);
		
		//Select a format with a depth component that supports usage as depth attachment
		VkFormat FindDepthFormat();

	private:
		// Select a format for the depth buffer 
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	};

}

