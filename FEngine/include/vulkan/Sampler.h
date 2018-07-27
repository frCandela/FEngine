#pragma once

#include "Device.h"

namespace vk
{
	class Sampler
	{
	public:
		Sampler(Device& device);
		~Sampler();

		// Create a Sampler
		void CreateSampler(float maxLod, float maxAnisotropy);

		VkSampler sampler;

	private:
		Device & m_rDevice;

	};
}
