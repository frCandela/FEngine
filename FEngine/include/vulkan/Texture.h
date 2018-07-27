#pragma once

#include "Image.h"
#include "CommandPool.h"


namespace vk
{
	class Texture : public Image
	{
	public:
		Texture(Device& device, vk::CommandPool& commandPool);

		/// Load an image and upload it into a Vulkan image object
		void LoadTexture(std::string path);		
		
		/// Copy a buffer to an Image
		void CopyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height);
		
		/// Generate mipmaps for a VkImage
		void GenerateMipmaps(VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

		uint32_t m_mipLevels = 1;

	private:
		std::string m_path;

	};
}