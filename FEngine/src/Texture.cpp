#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <algorithm>

namespace vk
{

	Texture::Texture(Device& device) :
		Image(device)
	{

	}

	void Texture::LoadTexture(std::string path)
	{
		m_path = path;

		// Load image from disk
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * 4;

		if (!pixels)
			throw std::runtime_error("failed to load texture image!");

		m_mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

		// Create a buffer in host visible memory
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		// Copy the pixel values from the image loading library to the buffer
		void* data;
		vkMapMemory(m_device.device, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(m_device.device, stagingBufferMemory);

		stbi_image_free(pixels);

		// Create the image in Vulkan
		createImage(texWidth, texHeight, m_mipLevels, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, deviceMemory);

		// Transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps
		transitionImageLayout(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_mipLevels);
		copyBufferToImage(stagingBuffer, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

		// Clean up the staging buffer and its memory
		vkDestroyBuffer(m_device.device, stagingBuffer, nullptr);
		vkFreeMemory(m_device.device, stagingBufferMemory, nullptr);

		generateMipmaps(VK_FORMAT_R8G8B8A8_UNORM, texWidth, texHeight, m_mipLevels);

		// Creates the image View
		imageView = Image::createImageView(image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, m_mipLevels, m_device.device);

	}
}