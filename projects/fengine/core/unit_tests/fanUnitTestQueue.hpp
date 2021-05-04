#pragma once

#include "core/unit_tests/fanUnitTest.hpp"
#include "core/collections/fanQueue.hpp"

namespace fan
{
    //======================================================================================================================
    //======================================================================================================================
    class UnitTestQueue : public UnitTest<UnitTestQueue>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return {
                    { &UnitTestQueue::TestConstructor, "constructor" },
                    { &UnitTestQueue::TestSize,        "size" },
                    { &UnitTestQueue::TestClear,       "clear" },
                    { &UnitTestQueue::TestFrontBack,   "front, back" },
                    { &UnitTestQueue::TestOrdering,    "ordering" },
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
            Queue<int, 16> queue;
            static_assert( queue.sMaxSize == 16 );
        }

        void TestSize()
        {
            Queue<int, 2> queue;
            TEST_ASSERT( queue.Empty() );

            queue.Push( 12 );
            TEST_ASSERT( queue.mSize == 1 );
            queue.Push( 12 );
            TEST_ASSERT( queue.mSize == 2 );
            TEST_ASSERT( queue.Full() );

            queue.Pop();
            TEST_ASSERT( queue.mSize == 1 );
            queue.Pop();
            TEST_ASSERT( queue.mSize == 0 );
            TEST_ASSERT( queue.Empty() );
        }

        void TestClear()
        {
            Queue<int, 16> queue;
            TEST_ASSERT( queue.Empty() )

            queue.Push( 12 );
            TEST_ASSERT( !queue.Empty() )
            queue.Clear();
            TEST_ASSERT( queue.Empty() )

            queue.Push( 11 );
            queue.Push( 12 );
            TEST_ASSERT( !queue.Empty() )
            queue.Clear();
            TEST_ASSERT( queue.Empty() )
        }

        void TestFrontBack()
        {
            Queue<int, 16> queue;
            queue.Push( 12 );
            TEST_ASSERT( queue.Front() == 12 );
            TEST_ASSERT( queue.Back() == 12 );
            queue.Push( 42 );
            TEST_ASSERT( queue.Front() == 12 );
            TEST_ASSERT( queue.Back() == 42 );
        }

        void TestOrdering()
        {
            Queue<int, 16> queue;
            for( int       i = 0; i < queue.sMaxSize; i++ )
            {
                queue.Push( i );
                TEST_ASSERT( queue.Front() == 0 );
                TEST_ASSERT( queue.Back() == i );
            }

            for( int i = 0; i < queue.sMaxSize; i++ )
            {
                TEST_ASSERT( queue.Front() == i );
                queue.Pop();
            }

            TEST_ASSERT( queue.Empty() );
        }
    };
}