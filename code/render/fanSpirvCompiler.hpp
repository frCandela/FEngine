#pragma once

#include <vector>
#include <cstdint>
#include <string>

namespace fan
{
	//========================================================================================================
	// compiles glsl to spirv using shaderc
	//========================================================================================================
	struct SpirvCompiler
	{
		static std::vector<uint32_t> Compile( const std::string _filename );
	};
}
