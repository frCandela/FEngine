#include "core/fanSystem.hpp"
#include "core/fanDebug.hpp"

namespace fan
{
    static bool gFanAssertBreakEnabled = true;

    #define fanAssertMsg( _expression, _msg )                               \
        do                                                                  \
        {                                                                   \
            if( ! _expression )                                             \
            {                                                               \
                if( gFanAssertBreakEnabled ){ FAN_DEBUG_BREAK }             \
                                                                            \
                Debug::Error() << "Assert:"                                 \
                    << _msg                                                 \
                    << " func:" << __func__                                 \
                    << " line:" << __LINE__                                 \
                    << " file:" << __FILE__                                 \
                    << Debug::Endl();                                       \
            }                                                               \
        }                                                                   \
        while( false )

    #define fanAssert( _expression ) fanAssertMsg( _expression, "" )
}