#pragma once

namespace fan
{
	class Device;

	//================================================================================================================================
	//================================================================================================================================
	class Sampler
	{
	public:
		Sampler(Device & _device);
		~Sampler();

		// Create a Sampler
		void CreateSampler(float _maxLod, float _maxAnisotropy);

		VkSampler GetSampler() { return m_sampler; }

	private:
		Device & m_device;

		VkSampler m_sampler;
	};
}