#include "vulkan/Shader.h"

#include <fstream>

namespace vk
{
	Shader::Shader(Device& device, std::string path) :
		m_device(device),
		m_path(path)
	{
		CreateShaderModule();
	}

	Shader::~Shader()
	{
		vkDestroyShaderModule(m_device.device, shaderModule, nullptr);
	}


	void Shader::CreateShaderModule()
	{
		// Load shader code
		auto code = ReadFile(m_path);

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(m_device.device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
			throw std::runtime_error("failed to create shader module!");
	}

	std::vector<char> Shader::ReadFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);//ate -> seek to the end of stream immediately after open 

		if (!file.is_open())
			throw std::runtime_error("failed to open file " + filename);

		//Allocate the buffer
		size_t fileSize = (size_t)file.tellg(); // tellg -> position in input sequence
		std::vector<char> buffer(fileSize);

		//Read the file
		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}
}