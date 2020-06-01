#include "ecs/fanEcsArchetype.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void EcsArchetype::Create( const std::vector< EcsComponentInfo >& _componentsInfo, const EcsSignature& _signature )
	{
		const int numComponents = int( _componentsInfo.size() );

		m_signature = _signature;
		m_chunks.resize( numComponents );

		for( int i = 0; i < numComponents; i++ )
		{
			if( m_signature[i] )
			{
				const EcsComponentInfo& info = _componentsInfo[i];
				m_chunks[i].Create( info.copy, info.size, info.alignment );
			}
		}
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
		for( EcsChunkVector& chunkVector : m_chunks )
		{
			chunkVector.Clear();
		}
		m_entities.clear();
	}
}