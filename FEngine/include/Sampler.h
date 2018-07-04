#pragma once

#include "Device.h"

class Sampler
{
public:
	Device& m_device;
	VkSampler sampler;

	Sampler(Device& device);
	~Sampler();
	void createSampler(uint32_t mipLevels);

};
