#pragma once

#include <vector>
#include "core/ecs/fanEcsChunk.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // Dynamic array of fixed size chunks
    // Can be indexed like a continuous array
    //==================================================================================================================================================================================================
    class EcsChunkVector
    {
    public:
        using CpyFunction = void* ( * )( void*, const void*, size_t );

        void Create( CpyFunction _cpyFunction, const int _componentSize, const int _alignment );
        void Remove( const int& _index );
        void* At( const int& _index );
        int PushBack( void* _data );
        int EmplaceBack();
        void Clear();

        const EcsChunk& GetChunk( const int _index ) const { return mChunks[_index]; }
        EcsChunk& GetChunk( const int _index ) { return mChunks[_index]; }
        int NumChunk() const { return int( mChunks.size() ); }

    private:
        std::vector<EcsChunk> mChunks;
        int                   mComponentSize;
        int                   mAlignment;
        int                   mChunkCapacity;
        CpyFunction           mCpyFunction = nullptr;
    };
}