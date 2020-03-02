#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanEntity.hpp"

namespace fan {

	//==============================================================================================================================================================
	// Contains the entities, components, singleton components, type information
	// and various utilities for processing them
	//==============================================================================================================================================================
	class EntityWorld
	{
	public:
		EntityWorld();

		//================================
		template< typename _componentType >
		_componentType& AddComponent( EntityID _entityID )
		{
			static_assert( std::is_base_of< ecComponent, _componentType>::value );
			const ComponentIndex index = m_typeIndices[_componentType::s_typeInfo ];
			Entity& entity = GetEntity( _entityID );
			assert( !entity.signature[index] ); // this entity already have this component
			assert( entity.componentCount < Entity::s_maxComponentsPerEntity );
			_componentType& component = (_componentType&)m_components[index].NewComponent();
			component.Init();
			component.componentIndex = index;
			entity.components[entity.componentCount] = &component;
			entity.componentCount++;
			entity.signature[index] = 1;
			return component;
		}

		//================================
		template< typename _componentType >
		void RemoveComponent( EntityID _entityID )
		{
			Entity& entity = GetEntity( _entityID );
			assert( entity.signature[_componentType::s_typeID] == 1 ); // this entity doesn't have this component
			_componentType& component = entity.GetComponent<_componentType>();
			m_components[_componentType::s_typeID].RemoveComponent( component.chunckIndex, component.index );
			entity.signature[_componentType::s_typeID] = 0;

			for( int componentIndex = 0; componentIndex < entity.componentCount; componentIndex++ )
			{
				if( entity.components[componentIndex]->typeID == _componentType::s_typeID )
				{
					entity.componentCount--;
					entity.components[componentIndex] = entity.components[entity.componentCount]; // swap
					entity.components[entity.componentCount] = nullptr;
					return;
				}
			}
			assert( false ); // component not found
		}

		//================================
		template< typename _tagType >
		void AddTag( EntityID _entityID  )
		{
			static_assert( std::is_base_of< Tag, _tagType>::value );
			Entity& entity = GetEntity( _entityID );
			const ComponentIndex index = m_typeIndices[_tagType::s_typeInfo];
			entity.signature[index] = 1;
		}

		//================================
		template< typename _componentType >
		_componentType& GetSingletonComponent()
		{
			static_assert( std::is_base_of< SingletonComponent, _componentType>::value );
			return  * static_cast<_componentType*>(m_singletonComponents[_componentType::s_typeInfo]);
		}
		//================================
		EntityID CreateEntity()
		{
			Entity entity;
			entity.signature = Signature( 1 ) << ecAliveBit;
			EntityID  id = (EntityID)m_entities.size();
			m_entities.push_back( entity );
			return id;
		}

		//================================
		void DeleteEntity( EntityID _entityID )
		{
			m_entities[_entityID].Kill();
		}

		//================================
		EntityHandle GetHandle( EntityID _entityID )
		{
			Entity& entity = m_entities[_entityID];
			if( entity.handle != 0 )
			{
				return entity.handle;
			}
			else
			{
				entity.handle = m_nextHandle++;
				m_handles[ entity.handle ] = _entityID;
				return entity.handle;
			}
		}

		//================================
		Entity& GetEntity( const EntityID _id )
		{
			assert( _id < m_entities.size() );
			return m_entities[_id];
		}



		//================================================================================================================================
		// Place the dead entities at the end of the vector
		//================================================================================================================================
		void SortEntities()
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
		void RemoveDeadEntities()
		{
			if( m_entities.empty() ) { return; }

			int reverseIndex = (int)m_entities.size() - 1;
			while( reverseIndex >= 0 )
			{
				Entity& entity = m_entities[reverseIndex];
				if( entity.IsAlive() ) { break; } // we removed all dead entities

	 			// Remove corresponding handle
				if( entity.handle != 0 ){ m_handles.erase( entity.handle ); }

				// Remove the component
				for( int componentIndex = 0; componentIndex < entity.componentCount; componentIndex++ )
				{
					ecComponent& component = *entity.components[componentIndex];
					m_components[ component.componentIndex ].RemoveComponent( component.chunckIndex, component.chunckComponentIndex );
				}
				m_entities.pop_back();
				--reverseIndex;
			}
		}

		//================================================================================================================================
		// Find all entities matching the signature of the system and runs it
		//================================================================================================================================
		template< typename _systemType >
		void RunSystem( const float _delta )
		{
			static_assert( std::is_base_of< System, _systemType>::value );
			std::vector<Entity*> matchEntities;

			const Signature systemSignature = _systemType::GetSignature(*this);

			matchEntities.reserve( m_entities.size() );
			for (int entityIndex = 0; entityIndex < m_entities.size() ; entityIndex++)
			{
				Entity& entity = m_entities[entityIndex];
				if( (entity.signature & systemSignature ) == systemSignature )
				{
					matchEntities.push_back( &entity );
				}						
			}
			_systemType::Run( *this, matchEntities, _delta );
		}	

		template< typename _tagOrComponentType >
		Signature GetSignature() const
		{
			static_assert( std::is_base_of< Tag, _tagOrComponentType>::value || std::is_base_of< ecComponent, _tagOrComponentType>::value );
			return Signature(1) << m_typeIndices.at( _tagOrComponentType::s_typeInfo );
		}
	//private:


		template< typename _componentType >
		void AddComponentType()
		{
			static_assert( std::is_base_of< ecComponent, _componentType>::value );			
			ComponentsCollection chunck;
			chunck.Init<_componentType>( _componentType::s_typeName );
			m_components.push_back( chunck );
			m_typeIndices[_componentType::s_typeInfo] = m_nextTypeIndex++;
		}

		template< typename _tagType >
		void AddTagType()
		{
			static_assert( std::is_base_of< Tag, _tagType>::value );
			m_typeIndices[_tagType::s_typeInfo] = m_nextTypeIndex++;
		}

		template< typename _componentType >
		void AddSingletonComponentType()
		{
			static_assert( std::is_base_of< SingletonComponent, _componentType>::value );		
			assert( m_singletonComponents.find( _componentType::s_typeInfo ) == m_singletonComponents.end() );
			m_singletonComponents[_componentType::s_typeInfo] = new _componentType();
		}

		std::unordered_map< uint32_t, ComponentIndex >	m_typeIndices;
		std::unordered_map< EntityHandle, EntityID >	m_handles;
		std::unordered_map< uint32_t, SingletonComponent* >	m_singletonComponents;
		ComponentIndex m_nextTypeIndex = 0;

		std::vector< Entity > m_entities;
		std::vector< ComponentsCollection > m_components;
		EntityHandle m_nextHandle = 1; // 0 is a null handle
	};
}