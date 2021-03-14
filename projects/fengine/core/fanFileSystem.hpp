#pragma once

#include <string>
#include "core/fanAssert.hpp"

namespace fan
{
    //==========================================================================================================================
    //==========================================================================================================================
    class FileSystem
    {
    public:
        static bool SetProjectPath( const std::string& _projectPath );
        static void Reset();
        static bool IsAbsolute( const std::string& _path );
        static std::string GetProjectPath() { return sProjectPath; }
        static std::string NormalizePath( const std::string& _path );
        static std::string Directory( const std::string& _path );
        static std::string FileName( const std::string& _path );

    private:
        static std::string sProjectPath;
    };
}