#include "core/fanAssert.hpp"
#include "core/fanDebug.hpp"


#ifndef NDEBUG
namespace fan
{
    bool AssertUtils::sFanAssertBreakEnabled = true;

    //========================================================================================================
    //========================================================================================================
    void AssertUtils::LogAssertMessage( const char* _msg, const char* _func, int _line, const char* _file )
    {
        Debug::Error() << "Assert:"
                << _msg
                << " func:" << _func
                << " line:" << _line
                << " file:" << _file
                << Debug::Endl();
    }
}
#endif
