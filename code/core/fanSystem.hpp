#pragma once

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    struct System
    {
        static bool HasDebugger();
    };

    #define FAN_DEBUG_BREAK if( System::HasDebugger() ) { __debugbreak(); }
}

