#pragma once

#include "render/fanRenderPrecompiled.hpp"

namespace fan
{
	class Device;

	//================================================================================================================================
	// for setting sampling of images
	//================================================================================================================================
	class Sampler
	{
	public:
		Sampler( Device& _device );
		~Sampler();

		// Create a Sampler
		void CreateSampler( const float _maxLod, const float _maxAnisotropy, const VkFilter _filter );

		VkSampler GetSampler() { return m_sampler; }

	private:
		Device& m_device;

		VkSampler m_sampler;
	};
}