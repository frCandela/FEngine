#pragma once

#include "core/fanSystem.hpp"

namespace fan
{
    struct AssertUtils
    {
        static bool sFanAssertBreakEnabled;
        static void LogAssertMessage( const char* _message );
    };

    #define fanAssertMsg( _expression, _msg )                                   \
        do                                                                      \
        {                                                                       \
            if( ! (_expression) )                                               \
            {                                                                   \
                if( AssertUtils::sFanAssertBreakEnabled ){ FAN_DEBUG_BREAK }    \
                AssertUtils::LogAssertMessage( _msg );                          \
            }                                                                   \
        }                                                                       \
        while( false )

    #define fanAssert( _expression ) fanAssertMsg( _expression, "" )
}