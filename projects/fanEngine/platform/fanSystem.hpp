#pragma once

#include <string>
#include <vector>

namespace std
{
    class mutex;
}

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct System
    {
        enum class BuildType
        {
            Debug, Release, Error
        };

        static bool HasDebugger();
        static BuildType GetBuildType();
        static bool StartProcessAndWait( const std::string& _cmd, const std::string& _args, const std::string& _logsPath );
        static uint64_t LastModified( const std::string& _path );
        static bool Exists( const std::string& _path );
        static std::vector<std::string> ListDirectory( const std::string& _directoryPath );
        static bool WatchDirectory( const std::string _path, const bool* _enabled, std::vector<std::string>* _outFilesChanged, std::mutex* _mutex );
        static double GetTime();
    };
}

#define FAN_DEBUG_BREAK if( fan::System::HasDebugger() ) { __debugbreak(); }

