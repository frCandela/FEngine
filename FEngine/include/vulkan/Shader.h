#pragma once

#include <iostream>
#include <utility>

#include "Device.h"

namespace vk
{	
	class Shader
	{
	public:
		Shader(Device& device, std::string path);
		~Shader();

		// Creates a shader module from an spv file 
		void CreateShaderModule();	

		VkShaderModule shaderModule;
	private:
		Device& m_device;
		std::string m_path;

		// Reads a file and returns it as a vector<char> (used for loading shaders)
		std::vector<char> ReadFile(const std::string& filename);
		
	};

}

