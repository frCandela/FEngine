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
		uint32_t m_mipLevels = 1;



		static VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, VkDevice& device);

	protected:
		Device & m_device;
		CommandPool& m_rCommandPool;

		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		void transitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
		void copyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height);
		void generateMipmaps(VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

		static bool hasStencilComponent(VkFormat format)
		{
			return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
		}

	};
}