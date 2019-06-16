#pragma once

#pragma once

#include "AllInclude.h"

#include "vkDevice.h"
#include "vkSpirvCompiler.h"

namespace vk {

	class Shader {
	public:
		Shader(Device * _device) :
			m_device(_device) {

		}

		~Shader() {
			if (m_shaderModule != VK_NULL_HANDLE ) { 
				vkDestroyShaderModule(m_device->vkDevice, m_shaderModule, nullptr);
				m_shaderModule = VK_NULL_HANDLE;
			}
		}

		bool Create( const std::string _path ) {
			m_path = _path;

			std::vector<unsigned int> spirvCode = SpirvCompiler::Compile(_path);
			if (spirvCode.empty()) {
				std::cout << "Could not create shader module: " << _path << std::endl;
				return false;
			}


			VkShaderModuleCreateInfo shaderModuleCreateInfo;
			shaderModuleCreateInfo.sType= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			shaderModuleCreateInfo.pNext= nullptr;
			shaderModuleCreateInfo.flags=0;
			shaderModuleCreateInfo.codeSize= spirvCode.size() * sizeof(unsigned int);
			shaderModuleCreateInfo.pCode = spirvCode.data();

			if ( vkCreateShaderModule(m_device->vkDevice, &shaderModuleCreateInfo, nullptr, &m_shaderModule) != VK_SUCCESS) {
				std::cout << "Could not create shader module: " << _path << std::endl;
				return false;
			}
			std::cout << std::hex << "VkShaderModule\t\t" << m_shaderModule << std::dec << std::endl;

			return true;
		}

		VkShaderModule GetModule() { return m_shaderModule; }

	private:
		Device * m_device;
		VkShaderModule m_shaderModule;
		std::string m_path;

		std::vector<char> ReadFile(const std::string& _filename) {
			std::ifstream file( _filename, std::ios::ate | std::ios::binary ); //ate -> seek to the end of stream immediately after open 

			if ( file.is_open() == false ) {
				std::cout << "failed to open file: " << _filename << std::endl;
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
	};
}