#pragma once

#include "core/fanAssert.hpp"
#include "core/unit_tests/fanUnitTest.hpp"
#include "core/fanString.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    class UnitTestString : public UnitTest<UnitTestString>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return { { &UnitTestString::TestConstructor,       "Constructor" },
            };
        }

        void Create() override
        {
        }

        void Destroy() override
        {
        }

        void TestConstructor()
        {
            static_assert(String<2>::sCapacity == 1);
            static_assert(String<64>::sCapacity == 63);
            static_assert(String<128>::sCapacity == 127);

            // default constructor
            String<8> str;
            TEST_ASSERT( str == "" )
            TEST_ASSERT( str.mSize == 0 )
            TEST_ASSERT( str.Empty() )

            // char * constructor
            str = String<8>("");
            TEST_ASSERT( str.mSize == 0 )
            TEST_ASSERT( str.Empty() )
            str = String<8>("t");
            TEST_ASSERT( str.mSize == 1 )
            TEST_ASSERT( str == "t" )
            str = String<8>("toto");
            TEST_ASSERT( str.mSize == 4 )
            TEST_ASSERT( str == "toto" )
            str = String<8>("toto1234");
            TEST_ASSERT( str.mSize == 7 )
            TEST_ASSERT( str == "toto123" )

            // std::string constructor
            str = String<8>(std::string(""));
            TEST_ASSERT( str.mSize == 0 )
            TEST_ASSERT( str.Empty() )
            str = String<8>(std::string("t"));
            TEST_ASSERT( str.mSize == 1 )
            TEST_ASSERT( str == "t" )
            str = String<8>(std::string("toto"));
            TEST_ASSERT( str.mSize == 4 )
            TEST_ASSERT( str == "toto" )
            str = String<8>(std::string("toto1234"));
            TEST_ASSERT( str.mSize == 7 )
            TEST_ASSERT( str == "toto123" )
        }
    };
}