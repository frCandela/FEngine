#include "Includes.h"

#include "vulkan/vkImage.h"
#include "vulkan/vkDevice.h"

namespace vk {

	//================================================================================================================================
	//================================================================================================================================
	Image::Image(Device * _device) :
		m_device(_device) {
	}

	//================================================================================================================================
	//================================================================================================================================
	Image::~Image() {
		if (m_image != VK_NULL_HANDLE) {
			vkDestroyImage(m_device->vkDevice, m_image, nullptr);
			m_image = VK_NULL_HANDLE;

		}

		if (m_imageMemory != VK_NULL_HANDLE) {
			vkFreeMemory(m_device->vkDevice, m_imageMemory, nullptr);
			m_imageMemory = VK_NULL_HANDLE;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Image::Create(VkFormat _format, VkExtent2D _size, VkImageUsageFlags _usage, VkMemoryPropertyFlags _memoryProperties) {

		VkImageCreateInfo imageCreateInfo;
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.pNext = nullptr;
		imageCreateInfo.flags = 0;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = _format;
		imageCreateInfo.extent.width = _size.width;
		imageCreateInfo.extent.height = _size.height;
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage = _usage;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.queueFamilyIndexCount = 0;
		imageCreateInfo.pQueueFamilyIndices = nullptr;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		if (vkCreateImage(m_device->vkDevice, &imageCreateInfo, nullptr, &m_image) != VK_SUCCESS) {
			std::cout << "Could not allocate image" << std::endl;
			return false;
		}
		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(m_device->vkDevice, m_image, &memoryRequirements);

		VkMemoryAllocateInfo bufferMemoryAllocateInfo;
		bufferMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		bufferMemoryAllocateInfo.pNext = nullptr;
		bufferMemoryAllocateInfo.allocationSize = memoryRequirements.size;
		bufferMemoryAllocateInfo.memoryTypeIndex = m_device->FindMemoryType(memoryRequirements.memoryTypeBits, _memoryProperties);

		if (vkAllocateMemory(m_device->vkDevice, &bufferMemoryAllocateInfo, nullptr, &m_imageMemory) != VK_SUCCESS) {
			std::cout << "Could not allocate buffer" << std::endl;
			return false;
		}

		if (vkBindImageMemory(m_device->vkDevice, m_image, m_imageMemory, 0) != VK_SUCCESS) {
			std::cout << "Could not bind memory to image" << std::endl;
			return false;
		}
		std::cout << std::hex << "VkImage\t\t\t" << m_image << std::dec << std::endl;
		std::cout << std::hex << "VkDeviceMemory\t\t" << m_imageMemory << std::dec << std::endl;

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Image::TransitionImageLayout(VkCommandBuffer _commandBuffer, VkFormat _format, VkImageLayout _oldLayout, VkImageLayout _newLayout, uint32_t _mipLevels)
	{
		// Synchronize access to resources
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = _oldLayout;
		barrier.newLayout = _newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = m_image;

		// Use the right subresource aspect
		if (_newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			if (HasStencilComponent(_format))
				barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
		else
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = _mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		// Set the access masks and pipeline stages based on the layouts in the transition.
		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (_oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && _newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else
			throw std::invalid_argument("unsupported layout transition!");


		vkCmdPipelineBarrier(
			_commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
	}

}