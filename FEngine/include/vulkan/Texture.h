#pragma once

#include "Image.h"
#include "CommandPool.h"

namespace vk
{
	class Texture : public Image
	{
	public:
		Texture(Device& device);

		/// Load an image and upload it into a Vulkan image object
		void LoadTexture(std::string path, vk::CommandPool& commandPool);

	private:
		std::string m_path;
	};
}