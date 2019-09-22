#include "fanGlobalIncludes.h"

#include "renderer/core/fanShader.h"
#include "renderer/core/fanDevice.h"
#include "renderer/util/fanSpirvCompiler.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Shader::Shader(Device & _device) :
		m_device(_device) {

	}

	//================================================================================================================================
	//================================================================================================================================
	Shader::~Shader() {
		Destroy();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Shader::Create(const std::string _path) {
		m_path = _path;

		std::vector<unsigned int> spirvCode = SpirvCompiler::Compile(_path);
		if (spirvCode.empty()) {
			Debug::Get() << Debug::Severity::error << "Could not create shader module: " << _path << Debug::Endl();

			std::fs::directory_entry path(_path);
			std::string extension = path.path().extension().generic_string();
			std::string tmpPath = (extension == ".frag" ? defaultFragmentShader : defaultVertexShader);
			Debug::Get() << Debug::Severity::log << "loading default shader " << tmpPath << Debug::Endl();
			spirvCode = SpirvCompiler::Compile(tmpPath);

			if (spirvCode.empty()) {
				return false;
			}
		}


		VkShaderModuleCreateInfo shaderModuleCreateInfo;
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.pNext = nullptr;
		shaderModuleCreateInfo.flags = 0;
		shaderModuleCreateInfo.codeSize = spirvCode.size() * sizeof(unsigned int);
		shaderModuleCreateInfo.pCode = spirvCode.data();

		if (vkCreateShaderModule(m_device.vkDevice, &shaderModuleCreateInfo, nullptr, &m_shaderModule) != VK_SUCCESS) {
			Debug::Get() << Debug::Severity::error << "Could not create shader module: " << _path << Debug::Endl();
			return false;
		}
		Debug::Get() << Debug::Severity::log << std::hex << "VkShaderModule        " << m_shaderModule << std::dec << Debug::Endl();

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Shader::Reload() {
		Destroy();
		Create(m_path);
	}

	//================================================================================================================================
	//================================================================================================================================
	std::vector<char> Shader::ReadFile(const std::string& _filename) {
		std::ifstream file(_filename, std::ios::ate | std::ios::binary); //ate -> seek to the end of stream immediately after open 

		if (file.is_open() == false) {
			Debug::Get() << Debug::Severity::error << "failed to open file: " << _filename << Debug::Endl();
			return {};
		}

		//Allocate the buffer
		size_t fileSize = (size_t)file.tellg(); // tellg -> position in input sequence
		std::vector<char> buffer(fileSize);

		//Read the file
		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

	void Shader::Destroy() {
		if (m_shaderModule != VK_NULL_HANDLE) {
			vkDestroyShaderModule(m_device.vkDevice, m_shaderModule, nullptr);
			m_shaderModule = VK_NULL_HANDLE;
		}
	}
}