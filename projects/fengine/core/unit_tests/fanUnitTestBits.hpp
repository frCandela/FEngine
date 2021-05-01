#pragma once

#include "core/fanAssert.hpp"
#include "core/unit_tests/fanUnitTest.hpp"
#include "core/fanBits.hpp"

namespace fan
{
    //======================================================================================================================
    //======================================================================================================================
    class UnitTestBits : public UnitTest<UnitTestBits>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return { { &UnitTestBits::TestBitSet,       "Bit set" },
                     { &UnitTestBits::TestBitClear,     "Bit clear" },
                     { &UnitTestBits::TestBitFlip,      "Bit flip" },
                     { &UnitTestBits::TestBitTrueFalse, "Bit is true, bit is false" },
            };
        }

        void Create() override
        {
        }

        void Destroy() override
        {
        }

        void TestBitSet()
        {
            for( int i = 0; i < 32; ++i )
            {
                int bitfield = 0;
                BIT_SET( bitfield, i );
                TEST_ASSERT( bitfield == ( 1 << i ) )
            }
        }

        void TestBitClear()
        {
            for( int i = 0; i < 32; ++i )
            {
                int bitfield = ~0;
                BIT_CLEAR( bitfield, i );
                TEST_ASSERT( bitfield == ~( 1 << i ) )
            }
        }

        void TestBitFlip()
        {
            for( int i = 0; i < 32; ++i )
            {
                int bitfield = 0;
                BIT_FLIP( bitfield, i );
                TEST_ASSERT( bitfield == ( 1 << i ) )
            }

            for( int i = 0; i < 32; ++i )
            {
                int bitfield = ~0;
                BIT_FLIP( bitfield, i );
                TEST_ASSERT( bitfield == ~( 1 << i ) )
            }

            for( int i = 0; i < 32; ++i )
            {
                int bitfield = 0;
                BIT_FLIP( bitfield, i );
                BIT_FLIP( bitfield, i );
                TEST_ASSERT( bitfield == 0 )
            }
        }

        void TestBitTrueFalse()
        {
            for( int i = 0; i < 32; ++i )
            {
                int bitfield = 0;
                BIT_SET( bitfield, i );
                TEST_ASSERT( BIT_IS_TRUE( bitfield, i ) )
                TEST_ASSERT( !BIT_IS_FALSE( bitfield, i ) )

                BIT_CLEAR( bitfield, i );
                TEST_ASSERT( !BIT_IS_TRUE( bitfield, i ) )
                TEST_ASSERT( BIT_IS_FALSE( bitfield, i ) )
            }
        }
    };
}