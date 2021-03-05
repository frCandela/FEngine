#pragma once

#include <string>
#include "core/fanAssert.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    class FileSystem
    {
    public:
        static bool SetProjectPath( const std::string& _projectPath );
        static void Reset();
        static bool IsAbsolute( const std::string& _Path );
        static std::string GetProjectPath() { return s_projectPath; }
        static std::string NormalizePath( const std::string& _path );

    private:
        static std::string s_projectPath;
    };
}