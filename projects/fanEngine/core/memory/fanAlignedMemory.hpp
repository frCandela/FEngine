#pragma once

#include <stdlib.h>
#include <cstring>
#include "core/fanAssert.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // creates a buffer with a specific size & alignment
    //==================================================================================================================================================================================================
    template< typename T >
    class AlignedMemory
    {
    public:
        AlignedMemory() :
                mData( nullptr ), mSize( 0 ), mAlignment( 0 ) {}

        ~AlignedMemory()
        {
            AlignedFree( mData );
        }

        size_t Alignment() const { return mAlignment; }
        void SetAlignement( const size_t _alignment )
        {
            fanAssert( ( _alignment & ( _alignment - 1 ) ) == 0 );
            fanAssert( _alignment >= sizeof( T ) );

            mAlignment = _alignment;
        }

        size_t Size() const { return mSize; }
        void Resize( size_t _size )
        {
            fanAssert( mAlignment > 0 );

            void* oldData = mData;

            mData = AlignedMalloc( _size * mAlignment, (int)mAlignment );

            if( oldData != nullptr )
            {
                std::memcpy( mData, oldData, mSize );
                AlignedFree( oldData );
                oldData = nullptr;
            }

            mSize = _size;
        }

        T& operator[]( const int& _pos )
        {
            fanAssert( mData != nullptr );
            void* adress = static_cast< char* >( mData ) + mAlignment * _pos;
            return *static_cast< T* >( adress );
        }

    private:
        void* mData;
        size_t mSize;
        size_t mAlignment;

        //==============================================================================================================================================================================================
        //==============================================================================================================================================================================================
        void* AlignedMalloc( size_t _size, size_t _alignment )
        {
            void* data = nullptr;
#if defined(FAN_MSVC) || defined(FAN_MINGW)
            data = _aligned_malloc( _size, _alignment );
#else
            int res = posix_memalign( &data, _alignment, _size );
            if( res != 0 )
                data = nullptr;
#endif
            return data;
        }

        //==============================================================================================================================================================================================
        //==============================================================================================================================================================================================
        void AlignedFree( void* _data )
        {
            if( _data != nullptr )
            {
#if    defined(FAN_MSVC) || defined(FAN_MINGW)
                _aligned_free( _data );
#else
                free( _data );
#endif
            }
        }
    };
}
