#pragma once

#include "Image.h"

class Texture : public Image
{
public:
	Texture(Device& device);

	/// Load an image and upload it into a Vulkan image object
	void LoadTexture(std::string path);

private:
	std::string m_path;
};