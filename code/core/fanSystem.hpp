#pragma once

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    struct System
    {
        static bool HasDebugger();
    };

}

#define FAN_DEBUG_BREAK if( fan::System::HasDebugger() ) { __debugbreak(); }

