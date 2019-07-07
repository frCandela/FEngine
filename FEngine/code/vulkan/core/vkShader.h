#pragma once

namespace vk {

	class Device;

	class Shader {
	public:
		Shader(Device & _device);
		~Shader();

		bool Create(const std::string _path);
		void Reload();

		VkShaderModule GetModule() { return m_shaderModule; }

		const char * defaultVertexShader = "shaders/empty.vert";
		const char * defaultFragmentShader = "shaders/empty.frag";

	private:
		Device &		m_device;

		VkShaderModule	m_shaderModule;
		std::string		m_path;

		std::vector<char> ReadFile(const std::string& _filename);
		void Destroy();
	};
}