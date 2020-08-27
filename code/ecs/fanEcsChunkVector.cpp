#include "ecs/fanEcsChunkVector.hpp"
#include "core/fanAssert.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void EcsChunkVector::Create( CpyFunction _cpyFunction, const int _componentSize, const int _alignment )
	{
        mCpyFunction   = _cpyFunction;
        mComponentSize = _componentSize;
        mAlignment     = _alignment;
		mChunks.emplace_back();
		mChunks.rbegin()->Create( mCpyFunction, mComponentSize, mAlignment );
        mChunkCapacity = mChunks.rbegin()->Capacity();
	}

	//========================================================================================================
	//========================================================================================================
	void EcsChunkVector::Remove( const int& _index )
	{
		const int chunkIndex = _index / mChunkCapacity;
		const int elementIndex = _index % mChunkCapacity;
        fanAssert( chunkIndex < int( mChunks.size() ) );
        fanAssert( elementIndex < mChunks[chunkIndex].Size() );

		EcsChunk& chunk = mChunks[chunkIndex];

		// Last chunk ? just remove the element & back swap locally in the chunk
		if( chunkIndex == (int)mChunks.size() - 1 )
		{
			chunk.Remove( elementIndex );
		}
		else
		{
			// back swap the removed element with the last element of the last chunk
			EcsChunk& lastChunk = *mChunks.rbegin();
			chunk.Set( elementIndex, lastChunk.At( lastChunk.Size() - 1 ) );
			lastChunk.PopBack();
		}

		EcsChunk& lastChunk = *mChunks.rbegin();
		if( lastChunk.Empty() && mChunks.size() > 1 )
		{
			lastChunk.Destroy();
			mChunks.pop_back();
		}
	}

	//========================================================================================================
	//========================================================================================================
	void* EcsChunkVector::At( const int& _index )
	{
		const int chunkIndex = _index / mChunkCapacity;
		const int elementIndex = _index % mChunkCapacity;
        fanAssert( chunkIndex < int( mChunks.size() ) );
        fanAssert( elementIndex < mChunks[chunkIndex].Size() );

		return mChunks[chunkIndex].At( elementIndex );
	}

	//========================================================================================================
	//========================================================================================================
	int EcsChunkVector::PushBack( void* _data )
	{
		// Create a new chunk if necessary
		if( mChunks.rbegin()->Full() )
		{
			mChunks.emplace_back();
			mChunks.rbegin()->Create( mCpyFunction, mComponentSize, mAlignment );
		}

		// Push to the last chunk
		EcsChunk& chunk = *mChunks.rbegin();
		const int chunkIndex = int( mChunks.size() - 1 );
		const int elementIndex = chunk.Size();
		chunk.PushBack( _data );
		return chunkIndex * mChunkCapacity + elementIndex;
	}

	//========================================================================================================
	//========================================================================================================
	int EcsChunkVector::EmplaceBack()
	{
		// Create a new chunk if necessary
		if( mChunks.rbegin()->Full() )
		{
			mChunks.emplace_back();
			mChunks.rbegin()->Create( mCpyFunction, mComponentSize, mAlignment );
		}

		// Emplace to the last chunk
		EcsChunk& chunk = *mChunks.rbegin();
		const int chunkIndex = int( mChunks.size() - 1 );
		const int elementIndex = chunk.Size();
		chunk.EmplaceBack();
		return chunkIndex * mChunkCapacity + elementIndex;
	}

	//========================================================================================================
	//========================================================================================================
	void EcsChunkVector::Clear()
	{
		if( mChunks.empty() )
		{
			return;
		}

		for( int i = NumChunk() - 1; i > 0; i-- )
		{
			mChunks[i].Destroy();
			mChunks.pop_back();
		}
		mChunks[0].Clear();
        fanAssert( mChunks.size() == 1 );
        fanAssert( mChunks[0].Empty() );
	}
}