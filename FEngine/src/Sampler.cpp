#include "Sampler.h"

namespace vk
{

	Sampler::Sampler(Device& device, uint32_t mipLevels) :
		m_device(device)
	{
		createSampler(mipLevels);
	}

	Sampler::~Sampler()
	{
		vkDestroySampler(m_device.device, sampler, nullptr);
	}

	// Create a texture sampler to access a texture
	void Sampler::createSampler(uint32_t mipLevels)
	{
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = 16;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

		samplerInfo.minLod = 0;//static_cast<float>(textureImage->m_mipLevels / 2); to test

		samplerInfo.maxLod = static_cast<float>(mipLevels);// textureImage->m_mipLevels);
		samplerInfo.mipLodBias = 0;

		if (vkCreateSampler(m_device.device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
			throw std::runtime_error("failed to create texture sampler!");
	}
}