#pragma once

#include "glslang/Public/ShaderLang.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class SpirvCompiler {
	public:
		static std::vector<unsigned int> Compile(std::string _filename);

	private:
		static std::string GetFilePath(const std::string& _str);
		static std::string GetSuffix(const std::string& _name);
		static EShLanguage GetShaderStage(const std::string& _stage);
	};
}
