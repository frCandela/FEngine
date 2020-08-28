#pragma once

#include "glfw/glfw3.h"
#include <string>
#include <vector>

namespace fan
{
	struct Device;

	//========================================================================================================
	// a vulkan shader
	// Can be created from glsl a shader file (compiled to spirv )
	//========================================================================================================
	struct Shader
	{
		bool Create( Device& _device, const std::string _path );
		void Destroy( Device& _device );

		VkShaderModule	mShaderModule = VK_NULL_HANDLE;

	private:
		std::vector<char> ReadFile( const std::string& _filename );
	};
}