#include "fanEntityWorld.hpp"

#include "fanEcComponent.hpp"
#include "fanTag.hpp"
#include "fanSystem.hpp"

#include "scene/ecs/components/fanSceneNode.hpp"
#include "scene/ecs/components/fanTransform2.hpp"
#include "scene/ecs/components/fanDirectionalLight2.hpp"
#include "scene/ecs/components/fanpointLight2.hpp"
#include "scene/ecs/components/fanMeshRenderer2.hpp"
#include "scene/ecs/components/fanMaterial2.hpp"
#include "scene/ecs/components/fanCamera2.hpp"
#include "scene/ecs/components/fanParticleEmitter.hpp"
#include "scene/ecs/singletonComponents/fanRenderWorld.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( sc_sunLight, "sunlight" );

	REGISTER_TAG( tag_editorOnly, "editor_only" );
	REGISTER_TAG( tag_alwaysUpdate, "always_update" );	

	//================================================================================================================================
	//================================================================================================================================
	EntityWorld::EntityWorld()
	{
		AddSingletonComponentType<sc_sunLight>();
		AddSingletonComponentType<RenderWorld>();

		AddComponentType<SceneNode>();
		AddComponentType<Transform2>();
		AddComponentType<DirectionalLight2>();
		AddComponentType<PointLight2>();
		AddComponentType<MeshRenderer2>();
		AddComponentType<Material2>();
		AddComponentType<Camera2>();
		AddComponentType<ParticleEmitter>();
		
		
		AddTagType<tag_alwaysUpdate>();
		AddTagType<tag_editorOnly>();
			   
		assert( m_nextTypeIndex < 1 << ( 8 * sizeof( ComponentIndex ) ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	EntityID EntityWorld::CreateEntity()
	{
		Entity entity;
		entity.signature = Signature( 1 ) << ecAliveBit;
		EntityID  id = (EntityID)m_entities.size();
		m_entities.push_back( entity );
		return id;
	}

	//================================================================================================================================
	//================================================================================================================================
	ecComponent& EntityWorld::AddComponent( const EntityID _entityID, const ComponentIndex _index )
	{
		Entity& entity = GetEntity( _entityID );
		assert( !entity.signature[_index] ); // this entity already have this component
		assert( entity.componentCount < Entity::s_maxComponentsPerEntity );

		const ComponentInfo& info = m_componentInfo[_index];

		// alloc data
		ecComponent&		 componentBase = m_components[_index].NewComponent();
		ChunckIndex			 chunckIndex = componentBase.chunckIndex;
		ChunckComponentIndex chunckComponentIndex = componentBase.chunckComponentIndex;
		ecComponent&		 component = info.instanciate( &componentBase );			

		// set component
		info.clear( component );
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
	bool EntityWorld::HasComponent( const EntityID _entityID, ComponentIndex _index ) 
	{ 
		return m_entities[_entityID].HasComponent( _index ); 
	}

	//================================================================================================================================
	//================================================================================================================================
	void EntityWorld::KillEntity( EntityID _entityID )
	{
		m_entities[_entityID].Kill();
	}

	//================================================================================================================================
	//================================================================================================================================
	EntityHandle EntityWorld::CreateHandle( EntityID _entityID )
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
	EntityID EntityWorld::GetEntityID( EntityHandle _handle )
	{
		return m_handles[_handle];
	}

	//================================================================================================================================
	//================================================================================================================================
	Entity& EntityWorld::GetEntity( const EntityID _id )
	{
		assert( _id < m_entities.size() );
		return m_entities[_id];
	}

	//================================================================================================================================
	// Place the dead entities at the end of the vector
	//================================================================================================================================
	void EntityWorld::SortEntities()
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
	void EntityWorld::RemoveDeadEntities()
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
				ecComponent& component = *entity.components[componentIndex];
				m_components[component.componentIndex].RemoveComponent( component.chunckIndex, component.chunckComponentIndex );
			}
			m_entities.pop_back();
			--reverseIndex;
		}
	}

	//================================================================================================================================
	// Removes the dead entities at the end of the entity vector
	//================================================================================================================================
	void EntityWorld::GetVectorComponentInfo( std::vector< const ComponentInfo*>& _outVector ) const
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
}
