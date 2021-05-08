#pragma once

#ifdef FAN_WIN32
#include "core/platforms/fanWinFile.hpp"
namespace fan
{
    using File = WinFile;
}
#endif