#pragma once

#include "AllInclude.h"

#include "vkDevice.h"
#include "vkBuffer.h"

namespace vk
{
	class Texture
	{
	public:
		Texture(Device * _device) : 
			m_device(_device) {

		}
		~Texture() {
			Destroy();
		}

		void Load(void* data, int width, int height, uint32_t _mipLevels);
		bool LoadTexture( std::string _path);

		std::string GetPath() const { return m_path; }
		VkImageView GetImageView() {	return m_imageView;		}

	private:
		VkImage m_image;
		VkDeviceMemory m_deviceMemory;
		VkImageView m_imageView;// images are accessed through image views rather than directly
		Device * m_device;
		uint32_t m_mipLevels = 1;
		uint32_t m_layerCount = 1;
		std::string m_path;

		void CopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer buffer, uint32_t width, uint32_t height) {

			// Specify which part of the buffer is going to be copied to which part of the image
			VkBufferImageCopy region = {};
			region.bufferOffset = 0;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;

			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;

			region.imageOffset = { 0, 0, 0 };
			region.imageExtent = {
				width,
				height,
				1
			};

			//Execute
			vkCmdCopyBufferToImage(
				commandBuffer,
				buffer,
				m_image,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&region
			);
		}
		void GenerateMipmaps(VkCommandBuffer commandBuffer, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t _mipLevels) {
			// Check if image format supports linear blitting
			VkFormatProperties formatProperties;
			vkGetPhysicalDeviceFormatProperties(m_device->vkPhysicalDevice, imageFormat, &formatProperties);
			if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
				throw std::runtime_error("texture image format does not support linear blitting!");

			VkImageMemoryBarrier barrier = {};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.image = m_image;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;
			barrier.subresourceRange.levelCount = 1;

			// Record each of the VkCmdBlitImage commands
			int32_t mipWidth = texWidth;
			int32_t mipHeight = texHeight;

			for (uint32_t i = 1; i < _mipLevels; ++i)
			{
				// This transition will wait for level i - 1 to be filled
				barrier.subresourceRange.baseMipLevel = i - 1;
				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

				vkCmdPipelineBarrier(commandBuffer,
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
					0, nullptr,
					0, nullptr,
					1, &barrier);

				// Specify the regions that will be used in the blit operation
				VkImageBlit blit = {};
				blit.srcOffsets[0] = { 0, 0, 0 };
				blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
				blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				blit.srcSubresource.mipLevel = i - 1;
				blit.srcSubresource.baseArrayLayer = 0;
				blit.srcSubresource.layerCount = 1;
				blit.dstOffsets[0] = { 0, 0, 0 };
				blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
				blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				blit.dstSubresource.mipLevel = i;
				blit.dstSubresource.baseArrayLayer = 0;
				blit.dstSubresource.layerCount = 1;

				// Record the blit command
				vkCmdBlitImage(commandBuffer,
					m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1, &blit,
					VK_FILTER_LINEAR);

				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				// This transition waits on the current blit command to finish
				vkCmdPipelineBarrier(commandBuffer,
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
					0, nullptr,
					0, nullptr,
					1, &barrier);

				if (mipWidth > 1) mipWidth /= 2;
				if (mipHeight > 1) mipHeight /= 2;
			}

			// Transitions the last mip level from VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			barrier.subresourceRange.baseMipLevel = _mipLevels - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);
		}
		void Destroy() {
			if (m_deviceMemory != VK_NULL_HANDLE) {
				vkFreeMemory(m_device->vkDevice, m_deviceMemory, nullptr);
				m_deviceMemory = VK_NULL_HANDLE;
			}

			if (m_imageView != VK_NULL_HANDLE) {
				vkDestroyImageView(m_device->vkDevice, m_imageView, nullptr);
				m_imageView = VK_NULL_HANDLE;
			}

			if (m_image != VK_NULL_HANDLE) {
				vkDestroyImage(m_device->vkDevice, m_image, nullptr);
				m_image = VK_NULL_HANDLE;
			}
		}
		void CreateImage(VkExtent2D _extent, uint32_t _mipLevels, VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties)
		{
			m_mipLevels = _mipLevels;

			// VK image info struct
			VkImageCreateInfo imageInfo = {};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = _extent.width;
			imageInfo.extent.height = _extent.height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = _mipLevels;
			imageInfo.arrayLayers = 1;
			imageInfo.format = _format;
			imageInfo.tiling = _tiling;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = _usage;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if (vkCreateImage(m_device->vkDevice, &imageInfo, nullptr, &m_image) != VK_SUCCESS)
				throw std::runtime_error("failed to create image!");

			std::cout << std::hex << "VkImage \t\t" << m_image << std::dec << std::endl;

			// Allocate memory for the image
			VkMemoryRequirements memRequirements;
			vkGetImageMemoryRequirements(m_device->vkDevice, m_image, &memRequirements);

			VkMemoryAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = m_device->FindMemoryType(memRequirements.memoryTypeBits, _properties);

			if (vkAllocateMemory(m_device->vkDevice, &allocInfo, nullptr, &m_deviceMemory) != VK_SUCCESS)
				throw std::runtime_error("failed to allocate image memory!");
			std::cout << std::hex << "VkDeviceMemory \t\t" << m_deviceMemory << std::dec << std::endl;

			vkBindImageMemory(m_device->vkDevice, m_image, m_deviceMemory, 0);
		}
		void CreateImageView(VkFormat format, VkImageViewType viewType, VkImageSubresourceRange subresourceRange)
		{
			VkImageViewCreateInfo viewInfo = {};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = m_image;
			viewInfo.viewType = viewType;
			viewInfo.format = format;
			viewInfo.subresourceRange = subresourceRange;

			if (vkCreateImageView(m_device->vkDevice, &viewInfo, nullptr, &m_imageView) != VK_SUCCESS)
				throw std::runtime_error("failed to create texture image view!");

			std::cout << std::hex << "VkImageView \t\t" << m_imageView << std::dec << std::endl;

		}
		void TransitionImageLayout(VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageSubresourceRange subresourceRange)
		{
			// Synchronize access to resources
			VkImageMemoryBarrier barrier = {};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout = oldLayout;
			barrier.newLayout = newLayout;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.image = m_image;
			barrier.subresourceRange = subresourceRange;

			// Set the access masks and pipeline stages based on the layouts in the transition.
			VkPipelineStageFlags sourceStage;
			VkPipelineStageFlags destinationStage;

			if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			{
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			{
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			{
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			}
			else
				throw std::invalid_argument("unsupported layout transition!");


			vkCmdPipelineBarrier(
				commandBuffer,
				sourceStage, destinationStage,
				0,
				0, nullptr,
				0, nullptr,
				1, &barrier
			);
		}
		bool CreateTextureImage(VkCommandBuffer commandBuffer, std::vector<std::string> paths);

};
}