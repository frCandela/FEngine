#pragma once

#include "core/fanUnitTest.hpp"
#include "core/fanAssert.hpp"
#include "core/fanDebug.hpp"

#include <cassert>

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    class UnitTestFanAssert : public UnitTest<UnitTestFanAssert>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return { { &UnitTestFanAssert::TestAssertTrue,                "Assert true"                  },
                     { &UnitTestFanAssert::TestAssertFalseErrorType,      "Assert false error type"      },
                     { &UnitTestFanAssert::TestAssertFalseDisplayMessage, "Assert false display message" },
                     { &UnitTestFanAssert::TestAssertMessage,             "Assert message"               },

            };
        }

        void Create() override
        {
            Debug::Get().onNewLog.Connect( &UnitTestFanAssert::OnNewLog, this );
        }
        void Destroy() override
        {
            Debug::Get().onNewLog.Disconnect( (size_t) this );
        }


        void OnNewLog( const Debug::LogItem _logItem )
        {
            mlastLog = _logItem;
            ++logCounter;
        }

        Debug::LogItem mlastLog;
        int logCounter = 0;

        void TestAssertTrue()
        {
            logCounter = 0;
            fanAssert( true );
            TEST_ASSERT( logCounter == 0 );
        }

        void TestAssertFalseDisplayMessage()
        {
            logCounter = 0;
            fanAssert( false  );
            TEST_ASSERT( logCounter == 1 );
        }

        void TestAssertFalseErrorType()
        {
            logCounter = 0;
            fanAssert( false  );
            TEST_ASSERT( logCounter == 1 );
            TEST_ASSERT( mlastLog.severity == Debug::Severity::error );
        }

        void TestAssertMessage()
        {
            logCounter = 0;
            fanAssertMsg( false , "message" );
            TEST_ASSERT( logCounter == 1 );
            TEST_ASSERT( mlastLog.message ==
            "Assert:message func:TestAssertMessage line:70 "
            "file:D:\\FEngine\\code\\core/unit_tests/fanUnitTestFanAssert.hpp" )
        }
    };
}