#pragma once

#include <bitset>
#include "ecs/fanEcsChunkVector.hpp"
#include "ecs/fanEcsComponent.hpp"
#include "ecs/fanEcsEntity.hpp"

namespace fan {
	static constexpr uint32_t ecsSignatureLength = 8;
	using EcsSignature = std::bitset<ecsSignatureLength>;

	//================================
	// An archetype is a collection of entities with the same signature
	// All component of the same entity are at the same index
	// components are put in arrays of small buffers called chunks
	//================================
	class EcsArchetype
	{
	public:
		void Create( const std::vector< EcsComponentInfo >& _componentsInfo, const EcsSignature& _signature )
		{
			const int numComponents = int(_componentsInfo.size());

			m_signature = _signature;
			m_chunks.resize( numComponents );

			for( int i = 0; i < numComponents; i++ )
			{
				if( m_signature[i] )
				{
					const EcsComponentInfo& info = _componentsInfo[i];
					m_chunks[i].Create( info.size, info.alignment );
				}
			}
		}

		// Returns true if the last entity was swapped with the removed one
		bool RemoveEntity( const int _entityIndex )
		{
			// pop the last element
			if( _entityIndex == Size() - 1 )
			{
				m_entities.pop_back();
				return false;
			}
			
			// else swap with the last
			m_entities[_entityIndex] = *m_entities.rbegin();
			m_entities.pop_back();
			return true;
		}

		void Clear()
		{			
			for ( EcsChunkVector& chunkVector : m_chunks )
			{
				chunkVector.Clear();
			}
			m_entities.clear();
		}

		int		Size() const  { return int(m_entities.size());  }
		bool	Empty() const { return m_entities.empty(); }

		EcsSignature					m_signature;
		std::vector< EcsChunkVector >	m_chunks;		// one index per component type
		std::vector< EcsEntityData >		m_entities;	
	};
}