#include "ecs/fanEcsArchetype.hpp"

namespace fan
{	
	//========================================================================================================
	//========================================================================================================
	EcsArchetype::EcsArchetype()
	{
		mChunkVectors.resize( ecsSignatureLength );
	}

	//========================================================================================================
	//========================================================================================================
    void EcsArchetype::Create( const std::vector<EcsComponentInfo>& _componentsInfo,
                               const EcsSignature& _signature )
	{
        mSignature = _signature;

		const int numComponents = int( _componentsInfo.size() );
		for( int i = 0; i < numComponents; i++ )
		{
			if( _signature[i] )
			{
                fanAssert( _componentsInfo[i].mIndex == i );
				AddComponentType( _componentsInfo[i] );
			}
		}
	}

	//========================================================================================================
	// Allocate chunks at the index of the new component
	//========================================================================================================
	void EcsArchetype::AddComponentType( const EcsComponentInfo& _componentsInfo )
	{
        mChunkVectors[_componentsInfo.mIndex].Create( _componentsInfo.copy,
                                                      _componentsInfo.mSize,
                                                      _componentsInfo.mAlignment );
	}

	//========================================================================================================
	// Returns true if the last entity was swapped with the removed one
	//========================================================================================================
	void EcsArchetype::RemoveEntity( const int _entityIndex )
	{
		// pop the last element
		if( _entityIndex == Size() - 1 )
		{
			mEntities.pop_back();
		}
		else
		{
			// else swap with the last
			mEntities[_entityIndex] = *mEntities.rbegin();
			mEntities.pop_back();
		}
	}

	//========================================================================================================
	//========================================================================================================
	void EcsArchetype::Clear()
	{
		for( EcsChunkVector& chunkVector : mChunkVectors )
		{
			chunkVector.Clear();
		}
		mEntities.clear();
	}
}