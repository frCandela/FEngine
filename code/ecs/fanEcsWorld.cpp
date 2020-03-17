#include "fanEcsWorld.hpp"

#include "fanComponent.hpp"
#include "fanTag.hpp"
#include "ecs/fanSingletonComponent.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	EcsWorld::EcsWorld( void ( *initializeTypes )( EcsWorld& ) )
	{
		initializeTypes( *this );
	}

	//================================================================================================================================
	//================================================================================================================================
	EcsWorld::~EcsWorld()
	{
		// delete singleton components
		for ( std::pair<uint32_t, SingletonComponent*> pair : m_singletonComponents )
		{
			delete pair.second;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	EntityID EcsWorld::CreateEntity()
	{
		Entity entity;
		entity.signature = Signature( 1 ) << ecAliveBit;
		EntityID  id = (EntityID)m_entities.size();
		m_entities.push_back( entity );
		return id;
	}

	//================================================================================================================================
	//================================================================================================================================
	Component& EcsWorld::AddComponent( const EntityID _entityID, const ComponentIndex _index )
	{
		Entity& entity = GetEntity( _entityID );
		assert( !entity.signature[_index] ); // this entity already have this component
		assert( entity.componentCount < Entity::s_maxComponentsPerEntity );

		const ComponentInfo& info = m_componentInfo[_index];

		// alloc data
		Component&		 componentBase = m_components[_index].NewComponent();
		ChunckIndex			 chunckIndex = componentBase.chunckIndex;
		ChunckComponentIndex chunckComponentIndex = componentBase.chunckComponentIndex;
		Component&		 component = info.instanciate( &componentBase );			

		// set component
		info.init( component );
		component.componentIndex = _index;
		component.chunckIndex = chunckIndex;
		component.chunckComponentIndex = chunckComponentIndex;

		// set entity
		entity.components[entity.componentCount] = &component;
		entity.componentCount++;
		entity.signature[_index] = 1;

		return component;
	}

	//================================================================================================================================
	//================================================================================================================================
	Component& EcsWorld::GetComponent( const EntityID _entityID, const ComponentIndex _index )
	{
		Entity& entity = GetEntity( _entityID );
		assert( entity.signature[_index] ); // entity has have this component
		for( int i = 0; i < entity.componentCount; i++ )
		{
			if( entity.components[i]->componentIndex == _index )
			{
				return *entity.components[i];
			}
		}
		assert( false );
		return *(Component*)( 0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void EcsWorld::RemoveComponent( const EntityID _entityID, const ComponentIndex _index )
	{		
		Entity& entity = GetEntity( _entityID );
		assert( entity.signature[_index] == 1 ); // this entity doesn't have this component
		Component& component = GetComponent( _entityID, _index );

		m_components[_index].RemoveComponent( component.chunckIndex, component.chunckComponentIndex );
		entity.signature[_index] = 0;

		for( int componentIndex = 0; componentIndex < entity.componentCount; componentIndex++ )
		{
			if( entity.components[componentIndex]->componentIndex == _index )
			{
				entity.componentCount--;
				entity.components[componentIndex] = entity.components[entity.componentCount]; // swap
				entity.components[entity.componentCount] = nullptr;
				return;
			}
		}
		assert( false ); // component not found
	}

	//================================================================================================================================
	//================================================================================================================================
	bool EcsWorld::HasComponent( const EntityID _entityID, ComponentIndex _index ) 
	{ 
		return m_entities[_entityID].HasComponent( _index ); 
	}

	//================================================================================================================================
	//================================================================================================================================
	void EcsWorld::KillEntity( const EntityID _entityID )
	{
		m_entities[_entityID].Kill();
	}

	//================================================================================================================================
	//================================================================================================================================
	EntityHandle EcsWorld::CreateHandle( const EntityID _entityID )
	{
		Entity& entity = m_entities[_entityID];
		if( entity.handle != 0 )
		{
			return entity.handle;
		}
		else
		{
			entity.handle = m_nextHandle++;
			m_handles[entity.handle] = _entityID;
			return entity.handle;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	EntityID EcsWorld::GetEntityID( const EntityHandle _handle )
	{
		return m_handles[_handle];
	}

	//================================================================================================================================
	//================================================================================================================================
	Entity& EcsWorld::GetEntity( const EntityID _id )
	{
		assert( _id < m_entities.size() );
		return m_entities[_id];
	}

	//================================================================================================================================
	//================================================================================================================================
	bool EcsWorld::EntityMatchSignature( EntityID _entityID, const Signature& _signature )
	{
		Entity& entity = m_entities[_entityID];
		return  ( entity.signature & _signature ) == _signature ;
	}

	//================================================================================================================================
	// Place the dead entities at the end of the vector
	//================================================================================================================================
	void EcsWorld::SortEntities()
	{
		const EntityID numEntities = static_cast<EntityID>( m_entities.size() );

		if( numEntities == 0 ) { return; }

		EntityID forwardIndex = 0;
		EntityID reverseIndex = numEntities - 1;

		while( true )
		{
			while( forwardIndex < numEntities - 1 && m_entities[forwardIndex].IsAlive() ) { ++forwardIndex; } // Finds a dead entity
			if( forwardIndex == numEntities - 1 ) break;

			while( reverseIndex > 0 && !m_entities[reverseIndex].IsAlive() ) { --reverseIndex; } // Finds a live entity
			if( reverseIndex == 0 ) break;

			if( forwardIndex > reverseIndex ) break;

			// Swap handles if necessary
			EntityHandle handleForward = m_entities[forwardIndex].handle;
			EntityHandle handleReverse = m_entities[reverseIndex].handle;
			if( handleForward != 0 ) { m_handles[handleForward] = reverseIndex; }
			if( handleReverse != 0 ) { m_handles[handleReverse] = forwardIndex; }

			// Swap entities
			std::swap( m_entities[reverseIndex], m_entities[forwardIndex] );
			++forwardIndex; --reverseIndex;
		}
	}


	//================================================================================================================================
	// Removes the dead entities at the end of the entity vector
	//================================================================================================================================
	void EcsWorld::RemoveDeadEntities()
	{
		if( m_entities.empty() ) { return; }

		int reverseIndex = (int)m_entities.size() - 1;
		while( reverseIndex >= 0 )
		{
			Entity& entity = m_entities[reverseIndex];
			if( entity.IsAlive() ) { break; } // we removed all dead entities

			// Remove corresponding handle
			if( entity.handle != 0 ) { m_handles.erase( entity.handle ); }

			// Remove the component
			for( int componentIndex = 0; componentIndex < entity.componentCount; componentIndex++ )
			{
				Component& component = *entity.components[componentIndex];
				m_components[component.componentIndex].RemoveComponent( component.chunckIndex, component.chunckComponentIndex );
			}
			m_entities.pop_back();
			--reverseIndex;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Component& EcsWorld::GetComponentAt( const EntityID _entityID, int _componentIndex )
	{ 
		Entity& entity = m_entities[_entityID];
		assert( _componentIndex < entity.componentCount );
		return *entity.components[_componentIndex];
	}

	//================================================================================================================================
	// Removes the dead entities at the end of the entity vector
	//================================================================================================================================
	void EcsWorld::GetVectorComponentInfo( std::vector< const ComponentInfo*>& _outVector ) const
	{
		_outVector.clear();
		for( auto& pair : m_componentInfo )
		{
			const ComponentInfo& info = pair.second;
			if( info.editorPath != nullptr )
			{
				_outVector.push_back( &info );
			}
		}
	}

	//================================================================================================================================
	// Find all entities matching the signature of the system
	//================================================================================================================================
	std::vector<EntityID> EcsWorld::Match( const Signature _signature )
	{
		// find matching entities in the whole world
		std::vector<EntityID> matchingEntities;
		matchingEntities.reserve( m_entities.size() );
		for( int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++ )
		{
			if( EntityMatchSignature( entityIndex, _signature ) )
			{
				matchingEntities.push_back( entityIndex );
			}
		}
		return matchingEntities;
	}

	//================================================================================================================================
	// Find all entities matching the signature of the system in the provided entities subset
	//================================================================================================================================
	std::vector<EntityID>	EcsWorld::MatchSubset( const Signature _signature, const std::vector<EntityID>& _subset )
	{
		// find matching entities in the subset
		std::vector<EntityID> matchingEntities;
		matchingEntities.reserve( _subset.size() );
		for( EntityID entityID : _subset )
		{
			if( EntityMatchSignature( entityID, _signature ) )
			{
				matchingEntities.push_back( entityID );
			}
		}
		return matchingEntities;
	}
}
