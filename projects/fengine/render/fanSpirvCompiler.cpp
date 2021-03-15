#include "render/fanSpirvCompiler.hpp"
#include "core/fanPath.hpp"
#include "core/fanDebug.hpp"
#include <fstream>

namespace fan
{
    //==========================================================================================================================
    //==========================================================================================================================
    std::vector<char> GetSpvFromFile( const std::string& _spvPath )
    {
        std::vector<char> buffer;
        std::ifstream     file( _spvPath, std::ios::ate | std::ios::binary );
        if( file.is_open() )
        {
            size_t fileSize = (size_t)file.tellg();
            buffer.resize( fileSize );
            file.seekg( 0 );
            file.read( buffer.data(), fileSize );
            file.close();
        }
        return buffer;
    }

    //==========================================================================================================================
    //==========================================================================================================================
    void CompileGlslToSpv( const std::string& _glslPath, const std::string& _spvPath )
    {
        const std::string executablePath = std::string(FAN_VULKAN_PATH) + "/Bin/glslc.exe";
        const std::string logsPath       = _spvPath + ".logs";
        std::string       processArgs    = _glslPath + " -o " + _spvPath;
        if( System::StartProcessAndWait( executablePath, processArgs, logsPath ) )
        {
            // read logs file and outputs it on the console
            std::ifstream logsFile( logsPath );
            if( logsFile.is_open() )
            {
                std::string line;
                while( std::getline( logsFile, line ) )
                {
                    Debug::Error() << line << Debug::Endl();
                }
                logsFile.close();
                std::remove( logsPath.c_str() );
            }
        }
    }

    //==========================================================================================================================
    //==========================================================================================================================
    std::vector<char> SpirvCompiler::GetFromGlsl( const std::string _glslPath )
    {
        const std::string normalizedGlslPath = Path::Normalize( _glslPath );
        if( !System::Exists( normalizedGlslPath ) )
        {
            Debug::Error() << "file not found: " << _glslPath << Debug::Endl();
            return {};
        }
        const std::string buildDir      = Path::Directory( normalizedGlslPath ) + "spv/";
        const std::string outputSpvPath = buildDir + Path::FileName( normalizedGlslPath ) + ".spv";

        if( !System::Exists( outputSpvPath ) ||
            System::LastModified( outputSpvPath ) < System::LastModified( normalizedGlslPath ) )
        {
            CompileGlslToSpv( normalizedGlslPath, outputSpvPath );
        }
        return GetSpvFromFile( outputSpvPath );
    }
}
