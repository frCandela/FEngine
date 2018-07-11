#pragma once

#include "Image.h"

class DepthImage : public Image
{
public:
	DepthImage(Device& device);
	void createDepthResources(uint32_t width, uint32_t height);
	VkFormat findDepthFormat();

private:
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
};

