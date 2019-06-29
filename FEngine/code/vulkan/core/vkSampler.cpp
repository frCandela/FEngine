#include "Includes.h"

#include "vulkan/core/vkSampler.h"
#include "vulkan/core/vkDevice.h"

namespace vk {
	//================================================================================================================================
	//================================================================================================================================
	Sampler::Sampler(Device & _device) :
		m_device(_device) {

	}

	//================================================================================================================================
	//================================================================================================================================
	Sampler::~Sampler() {
		vkDestroySampler(m_device.vkDevice, m_sampler, nullptr);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Sampler::CreateSampler(float _maxLod, float _maxAnisotropy) {
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = _maxAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

		samplerInfo.minLod = 0;//to test

		samplerInfo.maxLod = _maxLod;
		samplerInfo.mipLodBias = 0;

		if (vkCreateSampler(m_device.vkDevice, &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS)
			throw std::runtime_error("failed to create texture sampler!");
	}
}