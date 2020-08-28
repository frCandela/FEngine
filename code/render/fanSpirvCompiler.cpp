#include "render/fanSpirvCompiler.hpp"

#include "shaderc/shaderc.hpp"
#include "core/fanDebug.hpp"
#include <filesystem>
#include <fstream>

namespace fan
{
	std::vector<uint32_t> SpirvCompiler::Compile( const std::string _filename )
	{
		const std::string name = std::filesystem::path( _filename ).filename().string();

		// Find shader kind based on the file extension
		const std::string extension = std::filesystem::path( _filename ).extension().string();
		shaderc_shader_kind shaderKind;
		if ( extension == ".frag" )
		{
			shaderKind = shaderc_shader_kind::shaderc_fragment_shader;
		}
		else
		{
			assert( extension == ".vert" );
			shaderKind = shaderc_shader_kind::shaderc_vertex_shader;
		}

		std::ifstream file( _filename );
		if ( !file.is_open() )
		{
			Debug::Error() << "SpirvCompiler: Failed to load shader: " << _filename << Debug::Endl();
			return {};
		}
        std::string inputGLSL( ( std::istreambuf_iterator<char>( file ) ),
                               std::istreambuf_iterator<char>() );

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		// Like -DMY_DEFINE=1
		options.AddMacroDefinition( "MY_DEFINE", "1" );

		shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(
			inputGLSL.c_str(), inputGLSL.size(), shaderKind, name.c_str(), options );

		if ( module.GetCompilationStatus() !=
			 shaderc_compilation_status_success )
		{
			Debug::Log() << module.GetErrorMessage() << Debug::Endl();
		}

		std::vector<uint32_t> result( module.cbegin(), module.cend() );
		return result;

	}
}
