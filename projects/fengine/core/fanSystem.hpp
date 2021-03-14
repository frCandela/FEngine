#pragma once

#include <string>

namespace fan
{
    //==========================================================================================================================
    //==========================================================================================================================
    struct System
    {
        static bool HasDebugger();
        static bool StartProcessAndWait( const std::string& _cmd, const std::string& _args, const std::string& _logsPath );
        static uint64_t LastModified( const std::string& _path );
        static bool Exists( const std::string& _path );
    };
}

#define FAN_DEBUG_BREAK if( fan::System::HasDebugger() ) { __debugbreak(); }

