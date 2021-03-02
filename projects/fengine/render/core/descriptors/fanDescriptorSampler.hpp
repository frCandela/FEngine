#pragma once

#include "glfw/glfw3.h"

namespace fan
{
	struct Device;

	//========================================================================================================
	// creates  a sampler descriptor
	//========================================================================================================
	struct DescriptorSampler
	{
		void Create( Device& _device, VkSampler _sampler );
		void Destroy( Device& _device );

		VkDescriptorSetLayout	mDescriptorSetLayout = VK_NULL_HANDLE;;
		VkDescriptorPool		mDescriptorPool = VK_NULL_HANDLE;
		VkDescriptorSet			mDescriptorSet = VK_NULL_HANDLE;
	};
}