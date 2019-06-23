#pragma once

namespace vk {

	class Device;

	class Shader {
	public:
		Shader(Device * _device);
		~Shader();

		bool Create(const std::string _path);

		VkShaderModule GetModule() { return m_shaderModule; }

	private:
		Device *		m_device;

		VkShaderModule	m_shaderModule;
		std::string		m_path;

		std::vector<char> ReadFile(const std::string& _filename);
	};
}