#pragma once

#include <string>
#include <vector>
#include "core/fanAssert.hpp"

namespace fan
{
    //==========================================================================================================================
    //==========================================================================================================================
    class Path
    {
    public:
        static void Reset();
        static bool SetProjectPath( const std::string& _projectPath );
        static std::string GetProjectPath() { return sProjectPath; }

        static bool IsAbsolute( const std::string& _path );
        static bool IsRootDrive(const std::string& _path );
        static bool IsDirectory(const std::string& _path );
        static bool IsFile( const std::string& _path ) { return !IsDirectory(_path); };

        static std::string MakeRelative( const std::string& _path );
        static std::string Normalize( const std::string& _path );
        static std::string NormalizeSlashes( const std::string& _path );
        static std::string Directory( const std::string& _path );
        static std::string Extension( const std::string& _path );
        static std::string FileName( const std::string& _path );
        static std::string Parent( const std::string& _path );
        static std::vector<std::string> ListDirectory( const std::string& _path );

    private:
        static std::string sProjectPath;
    };
}