#pragma once

#include <memory>
#include "fanEcsChunkAllocator.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // Small aligned memory buffer
    //==================================================================================================================================================================================================
    class EcsChunk
    {
    public:
        static EcsChunkAllocator sAllocator;
        using CpyFunc = void* ( * )( void*, const void*, size_t );

        void Create( CpyFunc _cpyFunction, const int _componentSize, const int _alignment );
        void Destroy();
        bool Empty() const;
        bool Full() const;
        int Size() const;
        int Capacity() const;
        void* At( const int _index );
        void Set( const int _index, void* _data );
        void Remove( const int _index );
        void PushBack( void* _data );
        void PopBack();
        void EmplaceBack();
        void Clear();

    private:
        int mCapacity      = 0;
        int mSize          = 0;
        int mComponentSize = 0;
        void* mBuffer        = nullptr;
        void* mAlignedBuffer = nullptr;
        CpyFunc mCpyFunction = nullptr;
    };
}