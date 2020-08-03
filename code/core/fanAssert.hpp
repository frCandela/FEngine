#pragma once

#include "core/fanSystem.hpp"

namespace fan
{
    #ifdef NDEBUG

        #define fanAssertMsg
        #define fanAssert

    #else

        struct AssertUtils
        {
            static bool sFanAssertBreakEnabled;
            static void LogAssertMessage( const char* _msg, const char* _func, int _line, const char* _file );
        };

        #define fanAssertMsg( _expression, _msg )                                       \
            do                                                                          \
            {                                                                           \
                if( ! (_expression) )                                                   \
                {                                                                       \
                    if( AssertUtils::sFanAssertBreakEnabled ){ FAN_DEBUG_BREAK }        \
                    AssertUtils::LogAssertMessage( _msg, __func__, __LINE__, __FILE__ );\
                }                                                                       \
            }                                                                           \
            while( false )

        #define fanAssert( _expression ) fanAssertMsg( _expression, "" )

    #endif
}

