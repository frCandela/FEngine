#pragma once

#include <iostream>
#include <utility>

#include "Device.h"

namespace vk
{
	class ShaderModule
	{
	public:
		VkShaderModule module;
		Device& m_device;

		ShaderModule(VkShaderModule&& shaderModule, Device& device) :
			module(shaderModule),
			m_device(device)
		{
		}

		~ShaderModule()
		{
			vkDestroyShaderModule(m_device.device, module, nullptr);
		}

	};

	class Shader
	{
	public:
		Shader(Device& device, std::string path) :
			m_device(device),
			m_path(path)
		{}
		~Shader() {}

		VkShaderModule createShaderModule();
		static std::vector<char> readFile(const std::string& filename);

		ShaderModule GetShaderModule()
		{
			return ShaderModule(createShaderModule(), m_device);
		}

		Device& m_device;
		std::string m_path;
	};

}

