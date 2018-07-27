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

	private:
		std::string m_path;
	};
}