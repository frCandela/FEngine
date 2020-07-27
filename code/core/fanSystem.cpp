#include "core/fanSystem.hpp"

#ifdef  FAN_WIN32
    #include <windows.h>
#endif

namespace fan
{
    bool System::HasDebugger()
    {
        #ifdef  FAN_WIN32
                return IsDebuggerPresent();
        #else
                return false;
        #endif
    }
}