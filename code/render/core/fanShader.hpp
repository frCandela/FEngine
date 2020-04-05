#pragma once

#include "render/fanRenderPrecompiled.hpp"

namespace fan
{
	class Device;

	//================================================================================================================================
	// a vulkan shader
	// Can be created from glsl a shader file (compiled to spirv )
	//================================================================================================================================
	class Shader
	{
	public:
		Shader( Device& _device );
		~Shader();

		bool Create( const std::string _path );
		void Reload();

		VkShaderModule GetModule() { return m_shaderModule; }

		const char* defaultVertexShader = "code/shaders/empty.vert";
		const char* defaultFragmentShader = "code/shaders/empty.frag";

	private:
		Device& m_device;

		VkShaderModule	m_shaderModule;
		std::string		m_path;

		std::vector<char> ReadFile( const std::string& _filename );
		void Destroy();
	};
}