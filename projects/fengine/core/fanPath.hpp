#pragma once

#include <string>
#include "core/fanAssert.hpp"

namespace fan
{
    //==========================================================================================================================
    //==========================================================================================================================
    class Path
    {
    public:
        static bool SetProjectPath( const std::string& _projectPath );
        static void Reset();
        static std::string GetProjectPath() { return sProjectPath; }
        static bool IsAbsolute( const std::string& _path );
        static bool IsRootDrive(const std::string& _path );
        static bool IsDirectory(const std::string& _path );
        static bool IsFile( const std::string& _path ) { return !IsDirectory(_path); };
        static std::string Normalize( const std::string& _path );
        static std::string Directory( const std::string& _path );
        static std::string FileName( const std::string& _path );
        static std::string Parent( const std::string& _path );

    private:
        static std::string sProjectPath;
    };
}