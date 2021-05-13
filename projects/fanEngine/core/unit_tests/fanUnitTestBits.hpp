#pragma once

#include "core/fanAssert.hpp"
#include "core/unit_tests/fanUnitTest.hpp"
#include "core/fanBits.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    class UnitTestBits : public UnitTest<UnitTestBits>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return { { &UnitTestBits::TestBitSet,       "Bit set" },
                     { &UnitTestBits::TestBitClear,     "Bit clear" },
                     { &UnitTestBits::TestBitFlip,      "Bit flip" },
                     { &UnitTestBits::TestBitTrueFalse, "Bit is true, bit is false" },
                     { &UnitTestBits::TestBitmaskTrue,  "Bitmask true" },
                     { &UnitTestBits::TestBitmaskSet,   "Bitmask set" },
                     { &UnitTestBits::TestBitmaskClear, "Bitmask clear" },
                     { &UnitTestBits::TestBitmaskFlip,  "Bitmask flip" },
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
                TEST_ASSERT( BIT_TRUE( bitfield, i ) )
                TEST_ASSERT( !BIT_FALSE( bitfield, i ) )

                BIT_CLEAR( bitfield, i );
                TEST_ASSERT( !BIT_TRUE( bitfield, i ) )
                TEST_ASSERT( BIT_FALSE( bitfield, i ) )
            }
        }

        enum TestMask
        {
            None = 0, Test1 = 1, Test2 = 4, Test3 = 16, Test4 = 64
        };

        void TestBitmaskTrue()
        {
            int bitset = TestMask::Test1;
            TEST_ASSERT( BITMASK_TRUE_ANY( bitset, TestMask::Test1 ) )
            TEST_ASSERT( BITMASK_TRUE_ALL( bitset, TestMask::Test1 ) )

            bitset = TestMask::Test1 | TestMask::Test2;
            TEST_ASSERT( BITMASK_TRUE_ANY( bitset, TestMask::Test1 ) )
            TEST_ASSERT( BITMASK_TRUE_ANY( bitset, TestMask::Test2 ) )
            TEST_ASSERT( !BITMASK_TRUE_ANY( bitset, TestMask::Test3 ) )
            TEST_ASSERT( BITMASK_TRUE_ALL( bitset, TestMask::Test1 ) )
            TEST_ASSERT( BITMASK_TRUE_ALL( bitset, TestMask::Test2 ) )
            TEST_ASSERT( !BITMASK_TRUE_ALL( bitset, TestMask::Test3 ) )
            TEST_ASSERT( BITMASK_TRUE_ALL( bitset, TestMask::Test1 | TestMask::Test2 ) )
            TEST_ASSERT( !BITMASK_TRUE_ALL( bitset, TestMask::Test1 | TestMask::Test2 | TestMask::Test3 ) )

            bitset = TestMask::Test1 | TestMask::Test2 | TestMask::Test3;
            TEST_ASSERT( BITMASK_TRUE_ANY( bitset, TestMask::Test1 ) )
            TEST_ASSERT( BITMASK_TRUE_ANY( bitset, TestMask::Test2 ) )
            TEST_ASSERT( BITMASK_TRUE_ANY( bitset, TestMask::Test3 ) )
            TEST_ASSERT( !BITMASK_TRUE_ANY( bitset, TestMask::Test4 ) )
            TEST_ASSERT( BITMASK_TRUE_ALL( bitset, TestMask::Test1 ) )
            TEST_ASSERT( BITMASK_TRUE_ALL( bitset, TestMask::Test2 ) )
            TEST_ASSERT( BITMASK_TRUE_ALL( bitset, TestMask::Test3 ) )
            TEST_ASSERT( BITMASK_TRUE_ALL( bitset, TestMask::Test1 | TestMask::Test2 ) )
            TEST_ASSERT( BITMASK_TRUE_ALL( bitset, TestMask::Test2 | TestMask::Test3 ) )
            TEST_ASSERT( BITMASK_TRUE_ALL( bitset, TestMask::Test2 | TestMask::Test3 ) )
            TEST_ASSERT( BITMASK_TRUE_ALL( bitset, TestMask::Test1 | TestMask::Test2 | TestMask::Test3 ) )
            TEST_ASSERT( !BITMASK_TRUE_ALL( bitset, TestMask::Test1 | TestMask::Test2 | TestMask::Test3 | TestMask::Test4 ) )
        }

        void TestBitmaskSet()
        {
            int bitset = TestMask::None;
            BITMASK_SET( bitset, TestMask::None );
            TEST_ASSERT( bitset == TestMask::None )

            bitset = TestMask::None;
            BITMASK_SET( bitset, TestMask::Test1 );
            TEST_ASSERT( BITMASK_TRUE_ANY( bitset, TestMask::Test1 ) )

            bitset = TestMask::None;
            BITMASK_SET( bitset, TestMask::Test2 );
            BITMASK_SET( bitset, TestMask::Test3 );
            TEST_ASSERT( BITMASK_TRUE_ANY( bitset, TestMask::Test2 ) )
            TEST_ASSERT( BITMASK_TRUE_ANY( bitset, TestMask::Test3 ) )
            TEST_ASSERT( BITMASK_TRUE_ALL( bitset, TestMask::Test2 | TestMask::Test3 ) )
        };

        void TestBitmaskClear()
        {
            int bitset = TestMask::None;
            BITMASK_CLEAR( bitset, TestMask::None );
            TEST_ASSERT( bitset == TestMask::None )

            bitset = TestMask::Test1;
            BITMASK_CLEAR( bitset, TestMask::Test1 );
            TEST_ASSERT( !BITMASK_TRUE_ANY( bitset, TestMask::Test1 ) )

            bitset = TestMask::Test1 | TestMask::Test2;
            BITMASK_CLEAR( bitset, TestMask::Test2 );
            TEST_ASSERT( bitset == TestMask::Test1 )
            BITMASK_CLEAR( bitset, TestMask::Test1 );
            TEST_ASSERT( bitset == TestMask::None )
        };

        void TestBitmaskFlip()
        {
            int bitset = TestMask::None;
            TEST_ASSERT( bitset == TestMask::None )

            bitset = TestMask::None;
            BITMASK_FLIP( bitset, TestMask::Test1 );
            TEST_ASSERT( BITMASK_TRUE_ANY( bitset, TestMask::Test1 ) )

            bitset = TestMask::Test1;
            BITMASK_FLIP( bitset, TestMask::Test1 );
            TEST_ASSERT( !BITMASK_TRUE_ANY( bitset, TestMask::Test1 ) )

            bitset = TestMask::Test1 | TestMask::Test2;
            BITMASK_FLIP( bitset, TestMask::Test2 );
            TEST_ASSERT( bitset == TestMask::Test1 )
            BITMASK_FLIP( bitset, TestMask::Test1 );
            TEST_ASSERT( bitset == TestMask::None )
            BITMASK_FLIP( bitset, TestMask::Test2 );
            TEST_ASSERT( bitset == TestMask::Test2 )
            BITMASK_FLIP( bitset, TestMask::Test1 );
            TEST_ASSERT( bitset == ( TestMask::Test1 | TestMask::Test2 ) )
        };
    };
}