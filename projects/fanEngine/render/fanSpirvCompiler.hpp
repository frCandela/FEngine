#pragma once

#include <vector>
#include <cstdint>
#include <string>

namespace fan
{
    //==================================================================================================================================================================================================
    // compiles glsl to spirv using the lunarg sdk executable and system commands
    //==================================================================================================================================================================================================
    struct SpirvCompiler
    {
        static std::vector<char> GetFromGlsl( const std::string _filename );
    };
}
