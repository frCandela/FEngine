#pragma once

#ifndef GLFW_INCLUDE_VULKAN
	#define GLFW_INCLUDE_VULKAN
	#include <GLFW/glfw3.h>
#endif // !GLFW_INCLUDE_VULKAN

#include <string>

#include "Device.h"
#include "CommandPool.h"

namespace vk
{
	class Image
	{
	public:
		Image(Device& device, CommandPool& rCommandPool);
		virtual ~Image();

		VkImage image;
		VkDeviceMemory deviceMemory;
		VkImageView imageView;// images are accessed through image views rather than directly

		void CreateImageView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

	protected:
		Device & m_device;
		CommandPool& m_rCommandPool;

		/// Create a Vulkan Image
		void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		
		/// Handle layout transitions to transfer queue family ownership
		void TransitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
		
	};
}