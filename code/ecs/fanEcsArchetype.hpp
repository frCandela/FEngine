#pragma once

#include "ecs/fanEcsChunkVector.hpp"
#include "ecs/fanEcsComponent.hpp"

namespace fan
{
	//================================================================================================================================
	// An archetype is a collection of entities with the same signature
	// All component of the same entity are at the same index
	// components are put in arrays of small buffers called chunks
	//================================================================================================================================
	class EcsArchetype
	{
	public:
		void Create( const std::vector< EcsComponentInfo >& _componentsInfo, const EcsSignature& _signature );
		void RemoveEntity( const int _entityIndex );
		void Clear();
		int	 Size() const{ return int( m_entities.size() ); }
		bool Empty() const{ return m_entities.empty(); }
		void PushBackEntityData( const EcsEntityData& _entityData )	{ m_entities.push_back( _entityData ); }

		const EcsEntityData&	GetEntityData ( const int _index ) const	{ return m_entities[_index];	}
		EcsEntityData&			GetEntityData ( const int _index )			{ return m_entities[_index];	}
		const EcsChunkVector&	GetChunkVector( const int _index ) const	{ return m_chunks[_index];		}
		EcsChunkVector&			GetChunkVector( const int _index )			{ return m_chunks[_index];		}
		const EcsSignature&		GetSignature  () const						{ return m_signature;			}


	private:
		EcsSignature					m_signature;
		std::vector< EcsChunkVector >	m_chunks;		// one index per component type
		std::vector< EcsEntityData >	m_entities;
	};
}