#pragma once

namespace fan
{
    //==================================================================================================================================================================================================
    // fifo queue
    //==================================================================================================================================================================================================
    template< typename Type, int MaxSize >
    struct Queue
    {
        static_assert( MaxSize > 0 );
        static constexpr int sMaxSize = MaxSize;

        bool Empty() { return mSize == 0; }
        bool Full() { return mSize == MaxSize; }

        void Push( Type _value )
        {
            fanAssert( !Full() );
            mData[mSize++] = _value;
        }
        void Pop()
        {
            for( int i = 0; i < mSize; ++i )
            {
                mData[i] = mData[i + 1];
            }
            mSize--;
        }
        Type& Front() { return mData[0]; }
        Type& Back() { return mData[mSize - 1]; }
        Type& Front() const { return mData[0]; }
        Type& Back() const { return mData[mSize - 1]; }
        void Clear() { mSize = 0; }

        Type mData[MaxSize];
        int  mSize                    = 0;
    };
}