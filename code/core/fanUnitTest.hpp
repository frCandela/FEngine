#pragma once

#include <vector>
#include <string>
#include <exception>
#include "core/fanSystem.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    #define TEST_ASSERT( _condition )        \
    if( ! (_condition) ) {                   \
        if( gFanAssertBreakEnabled )         \
        {                                    \
            FAN_DEBUG_BREAK                  \
        }                                    \
        throw std::exception("test failed"); \
    }

    //========================================================================================================
    //========================================================================================================
    struct UnitTestResult
    {
        enum class Status { Success, Failed, Unknown };
        struct TestResult
        {
            std::string mName = "";
            Status      mStatus = Status::Unknown;
        };
        std::vector<TestResult>  mTestDisplays;
        Status                   mTotalStatus  = Status::Unknown;
    };


    //========================================================================================================
    // A unit test class is instanciated for every TestMethod returned by GetTests()
    // Unit test minimal example :
    /*
    class UnitTestSomething : public UnitTest<UnitTestSomething>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return {
                    { &UnitTestSomething::TestSomething,     "something" },
                    { &UnitTestSomething::TestSomethingElse, "something else" },
            };
        }
        void Create() override {}
        void Destroy() override {}
        void TestSomething() { TEST_ASSERT( true ); }
        void TestSomethingElse() { TEST_ASSERT( false ); }
    };
    */
    //========================================================================================================
    template< typename UnitTestType> class UnitTest {
    public:
        virtual void Create() = 0;
        virtual void Destroy() = 0;

        struct TestMethod
        {
            using Method = void ( UnitTestType::* )();
            Method      mMethod;
            std::string mName;
        };

        static UnitTestResult RunTests()
        {
            std::vector<TestMethod> tests = UnitTestType::GetTests();
            UnitTestResult          results;
            results.mTotalStatus = UnitTestResult::Status::Success;
            results.mTestDisplays.reserve( tests.size() );

            for( const TestMethod& test : tests )
            {
                bool success = true;
                UnitTestType unitTest;
                try
                {
                    unitTest.Create();
                    ( ( unitTest ).*( test.mMethod ) )();
                }
                catch( ... )
                {
                    success = false;
                }

                try
                {
                    unitTest.Destroy();
                }
                catch( ... )
                {
                    success = false;
                }

                if( success )
                {
                    results.mTestDisplays.push_back( { test.mName, UnitTestResult::Status::Success } );
                }
                else
                {
                    results.mTestDisplays.push_back( { test.mName, UnitTestResult::Status::Failed } );
                    results.mTotalStatus = UnitTestResult::Status::Failed;
                }
            }
            return results;
        }
    };
}