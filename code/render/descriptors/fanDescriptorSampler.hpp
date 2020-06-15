#pragma once

#include "glfw/glfw3.h"

namespace fan
{
	class Device;
	class Texture;
	class Sampler;

	//================================================================================================================================
	// Utility for making sampler descriptors
	//================================================================================================================================
	class DescriptorSampler
	{
	public:
		DescriptorSampler( Device& _device, VkSampler _sampler );
		~DescriptorSampler();

		void Update();

		VkDescriptorSetLayout GetLayout() { return m_descriptorSetLayout; }
		VkDescriptorSet		  GetSet() { return m_descriptorSet; }
	private:
		Device& m_device;

		VkDescriptorSetLayout	m_descriptorSetLayout;
		VkDescriptorPool		m_descriptorPool;
		VkDescriptorSet			m_descriptorSet;
	};
}