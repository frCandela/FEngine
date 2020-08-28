#pragma once

#include "glfw/glfw3.h"

namespace fan
{
	struct Device;

	//========================================================================================================
	// for setting sampling of images
	//========================================================================================================
	struct Sampler
	{
		void Destroy( Device& _device );
		void Create( Device& _device, const float _maxLod, const float _maxAnisotropy, const VkFilter _filter );

		VkSampler mSampler = VK_NULL_HANDLE;
	};
}