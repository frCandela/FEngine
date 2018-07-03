#pragma once

#ifndef GLFW_INCLUDE_VULKAN
	#define GLFW_INCLUDE_VULKAN
	#include <GLFW/glfw3.h>
#endif // !GLFW_INCLUDE_VULKAN

#include <string>

#include "FEngine.h"
class FEngine;


class Image
{
public:

	const std::string TEXTURE_PATH = "textures/chalet.jpg";
	VkImage image;
	VkDeviceMemory deviceMemory;
	VkImageView imageView;// images are accessed through image views rather than directly
	uint32_t m_mipLevels = 1;

	VkDevice& m_device;
	VkPhysicalDevice& m_physicalDevice;

	Image(VkDevice& device, VkPhysicalDevice& physicalDevice) : 
		m_device(device),
		m_physicalDevice(physicalDevice){};

	~Image() {};

	// Load an image and upload it into a Vulkan image object
	void createTextureImage();

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	void transitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
	void copyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height);
	void generateMipmaps(VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

	void Destroy()
	{
		vkDestroyImageView(m_device, imageView, nullptr);
		vkDestroyImage(m_device, image, nullptr);
		vkFreeMemory(m_device, deviceMemory, nullptr);
	}
};