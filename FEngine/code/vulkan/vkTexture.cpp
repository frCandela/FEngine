#include "vkTexture.h"
#include "vkRenderer.h"

#pragma warning(push, 0)   
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#pragma warning(pop)

namespace vk {
	bool Texture::CreateTextureImage(VkCommandBuffer _commandBuffer, std::vector<std::string> _paths) {
		std::vector< stbi_uc*> pixels;
		std::vector< glm::ivec3 > sizes;
		uint_fast32_t totalSize = 0;

		glm::ivec3 defaultSize;
		int defaultTexChannels;
		stbi_uc* defaultPixelData = stbi_load(_paths[0].c_str(), &defaultSize.x, &defaultSize.y, &defaultTexChannels, STBI_rgb_alpha);
		assert(defaultPixelData);

		// Load pixel data
		for (std::string path : _paths)
		{
			glm::ivec3 size;
			int texChannels;
			stbi_uc* pixelData = stbi_load(path.c_str(), &size.x, &size.y, &texChannels, STBI_rgb_alpha);
			if (!pixelData)
			{
				pixelData = defaultPixelData;
				size = defaultSize;
				texChannels = defaultTexChannels;
			}

			m_mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(size.x, size.y)))) + 1;
			m_mipLevels = 1;

			pixels.push_back(pixelData);
			sizes.push_back(size);
			totalSize += size.x * size.y * 4;
		}

		m_layerCount = static_cast<uint32_t>(_paths.size());

		// Create a buffer in host visible memory
		vk::Buffer stagingBuffer(m_device);
		stagingBuffer.Create(totalSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		// Copy the pixel values from the image loading library to the buffer
		stagingBuffer.Map(totalSize);

		size_t offset = 0;
		for (unsigned i = 0; i < m_layerCount; ++i)
		{
			glm::ivec3 size = sizes[i];
			stbi_uc* pixelData = pixels[i];
			uint32_t imageSize = size.x * size.y * 4;

			memcpy((char*)stagingBuffer.GetMappedData() + offset, pixelData, imageSize);
			offset += imageSize;
			stbi_image_free(pixelData);
		}

		/////////////////////////////////// Image ///////////////////////////////////

		// Setup buffer copy regions for array layers
		std::vector<VkBufferImageCopy> bufferCopyRegions;
		offset = 0;
		for (uint32_t layer = 0; layer < m_layerCount; layer++)
		{
			glm::ivec3 size = sizes[layer];
			uint32_t imageSize = size.x * size.y * 4;

			VkBufferImageCopy bufferCopyRegion = {};
			bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			bufferCopyRegion.imageSubresource.mipLevel = 0;
			bufferCopyRegion.imageSubresource.baseArrayLayer = layer;
			bufferCopyRegion.imageSubresource.layerCount = 1;
			bufferCopyRegion.imageExtent.width = size.x;
			bufferCopyRegion.imageExtent.height = size.y;
			bufferCopyRegion.imageExtent.depth = 1;
			bufferCopyRegion.bufferOffset = offset;

			bufferCopyRegions.push_back(bufferCopyRegion);

			// Increase offset into staging buffer for next level / face
			offset += imageSize;
		}

		// Create optimal tiled target image
		glm::ivec3 size = sizes[0];
		VkImageCreateInfo imageCreateInfo{};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.extent = { static_cast<uint32_t>(size.x),  static_cast<uint32_t>(size.y), 1 };
		imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageCreateInfo.arrayLayers = m_layerCount;

		if (vkCreateImage(m_device->vkDevice, &imageCreateInfo, nullptr, &m_image) != VK_SUCCESS)
			throw std::runtime_error("failed to vkCreateImage!");

		VkMemoryRequirements memReqs;
		vkGetImageMemoryRequirements(m_device->vkDevice, m_image, &memReqs);

		VkMemoryAllocateInfo memAllocInfo{};
		memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memAllocInfo.allocationSize = memReqs.size;
		memAllocInfo.memoryTypeIndex = m_device->FindMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (vkAllocateMemory(m_device->vkDevice, &memAllocInfo, nullptr, &m_deviceMemory) != VK_SUCCESS)
			throw std::runtime_error("failed to create vkAllocateMemory!");
		if (vkBindImageMemory(m_device->vkDevice, m_image, m_deviceMemory, 0) != VK_SUCCESS)
			throw std::runtime_error("failed to create vkBindImageMemory!");

		// Image barrier for optimal image (target)
		// Set initial layout for all array layers (faces) of the optimal (target) tiled texture
		VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT , 0, 1, 0, m_layerCount };
		TransitionImageLayout(_commandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);

		// Copy the cube map faces from the staging buffer to the optimal tiled image
		vkCmdCopyBufferToImage(
			_commandBuffer,
			stagingBuffer.GetBuffer(),
			m_image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			static_cast<uint32_t>(bufferCopyRegions.size()),
			bufferCopyRegions.data()
		);

		TransitionImageLayout(_commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange);

		/////////////////////////////////// image view ///////////////////////////////////

		CreateImageView(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_VIEW_TYPE_2D_ARRAY, { VK_IMAGE_ASPECT_COLOR_BIT, 0,1,0,m_layerCount });

		return true;
	}

	// Load an image from the disk and upload it into a Vulkan image object
	bool Texture::LoadTexture(std::string _path) {
		m_path = _path;

		// Load image from disk
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(_path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

		if (pixels == nullptr) {
			return false;
		}

		m_mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

		Load(pixels, texWidth, texHeight, m_mipLevels);

		stbi_image_free(pixels);

		return true;
	}

	void  Texture::Load(void* _data, int _width, int _height, uint32_t _mipLevels) {
		m_mipLevels = _mipLevels;
		VkDeviceSize imageSize = _width * _height * 4 * sizeof(char);

		// Create a buffer in host visible memory
		Buffer stagingBuffer(m_device);
		stagingBuffer.Create(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stagingBuffer.SetData(_data, imageSize);

		// Create the image in Vulkan
		VkExtent2D extent = { static_cast<uint32_t>(_width), static_cast<uint32_t>(_height) };
		CreateImage(extent, m_mipLevels, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		// Transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

		VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT , 0, m_mipLevels, 0, 1 };

		VkCommandBuffer cmd = Renderer::GetGlobalRenderer()->BeginSingleTimeCommands();
		TransitionImageLayout(cmd, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);
		CopyBufferToImage(cmd, stagingBuffer.GetBuffer(), static_cast<uint32_t>(_width), static_cast<uint32_t>(_height));
		
		if (m_mipLevels > 1){
			GenerateMipmaps(cmd, VK_FORMAT_R8G8B8A8_UNORM, _width, _height, m_mipLevels);
		} else {
			TransitionImageLayout(cmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange);
		}

		// Creates the image View
		CreateImageView(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_VIEW_TYPE_2D, { VK_IMAGE_ASPECT_COLOR_BIT, 0, m_mipLevels, 0, 1 });

		Renderer::GetGlobalRenderer()->EndSingleTimeCommands(cmd);
	}
}