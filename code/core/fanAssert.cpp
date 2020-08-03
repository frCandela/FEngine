#include "core/fanAssert.hpp"
#include "core/fanDebug.hpp"

namespace fan
{
    bool AssertUtils::sFanAssertBreakEnabled = true;

    void AssertUtils::LogAssertMessage(const char *_message)
    {
        Debug::Error() << "Assert:"
                    << _message
                    << " func:" << __func__
                    << " line:" << __LINE__
                    << " file:" << __FILE__
                    << Debug::Endl();
    }
}