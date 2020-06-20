#include "ecs/fanEcsArchetype.hpp"

namespace fan
{	
	//================================================================================================================================
	//================================================================================================================================
	EcsArchetype::EcsArchetype()
	{
		m_chunkVectors.resize( ecsSignatureLength );
	}

	//================================================================================================================================
	//================================================================================================================================
	void EcsArchetype::Create( const std::vector< EcsComponentInfo >& _componentsInfo, const EcsSignature& _signature )
	{
		m_signature = EcsSignature(0);

		const int numComponents = int( _componentsInfo.size() );
		for( int i = 0; i < numComponents; i++ )
		{
			if( _signature[i] )
			{
				assert( _componentsInfo[i].index == i );
				AddComponentType( _componentsInfo[i] );
			}
		}
	}

	//================================================================================================================================
	// Allocate chunks at the index of the new component
	//================================================================================================================================
	void EcsArchetype::AddComponentType( const EcsComponentInfo& _componentsInfo )
	{
		m_signature[ _componentsInfo.index ] = 1;
		m_chunkVectors[ _componentsInfo.index ].Create( _componentsInfo.copy, _componentsInfo.size, _componentsInfo.alignment );
	}

	//================================================================================================================================
	// Returns true if the last entity was swapped with the removed one
	//================================================================================================================================
	void EcsArchetype::RemoveEntity( const int _entityIndex )
	{
		// pop the last element
		if( _entityIndex == Size() - 1 )
		{
			m_entities.pop_back();
		}
		else
		{
			// else swap with the last
			m_entities[_entityIndex] = *m_entities.rbegin();
			m_entities.pop_back();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void EcsArchetype::Clear()
	{
		for( EcsChunkVector& chunkVector : m_chunkVectors )
		{
			chunkVector.Clear();
		}
		m_entities.clear();
	}
}