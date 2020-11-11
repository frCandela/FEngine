#include "fanEcsChunk.hpp"
#include "core/fanAssert.hpp"

namespace fan
{
	EcsChunkAllocator EcsChunk::sAllocator;

	//========================================================================================================
	//========================================================================================================
	void EcsChunk::Create( CpyFunc _cpyFunction, const int _componentSize, const int _alignment )
	{
        mCpyFunction   = _cpyFunction;
        mComponentSize = _componentSize;
        mSize          = 0;
        mBuffer        = sAllocator.Alloc();

		size_t space = EcsChunkAllocator::sChunkSize;
        mAlignedBuffer = mBuffer;
		void* result = std::align( _alignment, _componentSize, mAlignedBuffer, space );
		fanAssert( result != nullptr );
		fanAssert( result == mAlignedBuffer );
		(void)result;

        mCapacity = int( space / _componentSize );
	}

	//========================================================================================================
	//========================================================================================================
	void EcsChunk::Destroy()
	{
		sAllocator.Free( mBuffer );
        mBuffer        = nullptr;
        mAlignedBuffer = nullptr;
	}

	//========================================================================================================
	//========================================================================================================
	bool EcsChunk::Empty() const		{ return mSize == 0; }
	bool EcsChunk::Full() const			{ return mSize == mCapacity; }
	int	 EcsChunk::Size() const			{ return mSize; }
	int	 EcsChunk::Capacity() const		{ return mCapacity; }

	//========================================================================================================
	//========================================================================================================
	void* EcsChunk::At( const int _index )
	{
		fanAssert( _index < mSize );
		uint8_t* buffer = static_cast<uint8_t*>( mAlignedBuffer );
		return &buffer[_index * mComponentSize];
	}

	//========================================================================================================
	//========================================================================================================
	void EcsChunk::Set( const int _index, void* _data )
	{
        fanAssert( _index < mSize );
		mCpyFunction( At( _index ), _data, mComponentSize );
	}

	//========================================================================================================
	//========================================================================================================
	void EcsChunk::Remove( const int _index )
	{
        fanAssert( _index < mSize );
		// back swap
		if( mSize != 1 && _index != mSize - 1 )
		{
			Set( _index, At( mSize - 1 ) );
		}
		mSize--;
	}

	//========================================================================================================
	//========================================================================================================
	void EcsChunk::PushBack( void* _data )
	{
        fanAssert( mSize < mCapacity );
		const int index = mSize;
		mSize++;
		Set( index, _data );
	}

	//========================================================================================================
	//========================================================================================================
	void EcsChunk::PopBack()
	{
        fanAssert( mSize > 0 );
		mSize--;
	}

	//========================================================================================================
	//========================================================================================================
	void EcsChunk::EmplaceBack()
	{
        fanAssert( mSize < mCapacity );
		mSize++;
	}

	//========================================================================================================
	//========================================================================================================
	void EcsChunk::Clear()
	{
        mSize = 0;
	}
}