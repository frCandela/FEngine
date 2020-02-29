#pragma once

// #include <iostream>
// #include <bitset>
// #include <vector>
// #include <array>
// #include <unordered_map>
// #include <cassert>
#include "scene/fanScenePrecompiled.hpp"

namespace fan
{
	static constexpr uint32_t signatureLength = 32;
	using Signature = std::bitset<signatureLength>;
	static constexpr uint32_t ecAliveBit = signatureLength - 1;
	using EntityHandle = uint64_t;
	using EntityID = uint32_t;
	using ComponentID = uint8_t;
	using ChunckIndex = uint8_t;
	using ComponentIndex = uint16_t;

	//==============================================================================================================================================================
	// typeID is a unique id that also correspond to the index of the ComponentsCollection in the Entity world
	// chunckIndex is the index of the Chunck in the ComponentsCollection
	// index is the index of the component inside the chunck
	//==============================================================================================================================================================
	struct ecComponent
	{
		ComponentID typeID;
		ChunckIndex chunckIndex;
		ComponentIndex index;
	};
	static constexpr size_t sizeComponent = sizeof( ecComponent );
	static_assert( sizeComponent == 4 );

	//==============================================================================================================================================================
	// id is the index of the entity in the entity array
	//==============================================================================================================================================================
	struct Entity
	{
		static constexpr int s_maxComponentsPerEntity = 14;
		ecComponent* components[s_maxComponentsPerEntity];
		int componentCount = 0;
		Signature signature;
		EntityHandle handle = 0;

		//================================
		template< typename _componentType >
		_componentType& GetComponent()
		{
			static_assert( std::is_base_of< ecComponent, _componentType>::value );
			for( int i = 0; i < componentCount; i++ )
			{
				if( components[i]->typeID == _componentType::s_typeID )
				{
					return *static_cast<_componentType*> ( components[i] );
				}
			}
			assert( false );
			return *(_componentType*)( 0 );
		}

		//================================
		template< typename _componentType >
		bool HasComponent() const { return signature[_componentType::s_typeID] == 1; }

		void Kill() { signature[ecAliveBit] = 0; }
		bool IsAlive() { return  signature[ecAliveBit] == 1; }
	};
	static constexpr size_t sizeEntity = sizeof( Entity );
	static_assert( sizeEntity == 128 );

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct PositionComponent : public ecComponent
	{
		static ComponentID s_typeID;
		static const char* s_typeName;
		void Init() { x = y = z = 0.f; }
		float x;
		float y;
		float z;
	};

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct ColorComponent : public ecComponent
	{
		static ComponentID s_typeID;
		static const char* s_typeName;
		void Init() { r = g = b = a = 0; }
		char r;
		char g;
		char b;
		char a;
	};

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct SingletonComponent
	{
	public:
	};

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	class System
	{
	public:
	};

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	class ComponentsCollection
	{
	public:
		//================================
		template< typename _componentType >
		void Init( const std::string& _name )
		{
			m_name = _name;
			m_componentSize = sizeof( _componentType );
			m_componentCount = 65536 / m_componentSize; // 65536 bytes is the maximum size of a chunck
		}

		//================================
		// Returns the component _componentIndex of chunck _chunckIndex
		void* At( const ChunckIndex _chunckIndex, const ComponentIndex _componentIndex )
		{
			uint8_t* data = static_cast<uint8_t*>( m_chunks[_chunckIndex].data );
			uint8_t* component = data + m_componentSize * (uint32_t)_componentIndex;
			return component;
		}

		//================================
		// Deletes a component
		void RemoveComponent( const ChunckIndex _chunckIndex, const ComponentIndex _index )
		{
			m_chunks[_chunckIndex].recycleList.push_back( _index );
			m_chunks[_chunckIndex].count--;
			if( m_chunks[_chunckIndex].count == 0 )
			{
				m_chunks[_chunckIndex].recycleList.clear();
			}
		}

		//================================
		// Allocates a new component
		ecComponent& NewComponent()
		{
			for( unsigned chunckIndex = 0; chunckIndex < m_chunks.size(); chunckIndex++ )
			{
				Chunck& chunck = m_chunks[chunckIndex];
				if( !chunck.recycleList.empty() )
				{
					// recycle index
					const ComponentIndex index = chunck.recycleList[chunck.recycleList.size() - 1];
					chunck.recycleList.pop_back();
					chunck.count++;
					ecComponent& component = *static_cast<ecComponent*>( At( chunckIndex, index ) );
					component.chunckIndex = chunckIndex;
					component.index = index;
					return component;
				}
				else if( chunck.count < m_componentCount )
				{
					// create index
					const ComponentIndex index = chunck.count;
					chunck.count++;
					ecComponent& component = *static_cast<ecComponent*>( At( chunckIndex, index ) );
					component.chunckIndex = chunckIndex;
					component.index = index;
					return component;
				}
			}

			// create chunck
			Chunck& newChunck = AllocChunck();
			ecComponent& component = *static_cast<ecComponent*>( At( (ChunckIndex)m_chunks.size() - 1, 0 ) );
			component.chunckIndex = static_cast<ChunckIndex>( m_chunks.size() - 1 );
			component.index = 0;
			newChunck.count++;
			return component;
		}

	//private:
		//===============================================================================
		//===============================================================================
		// Block of memory containing components
		struct Chunck
		{
			void* data = nullptr;
			std::vector< ComponentIndex> recycleList;
			ComponentIndex count = 0;
		};

		std::string m_name = "";
		std::vector< Chunck > m_chunks;
		uint32_t m_componentSize;	// component size in bytes
		uint32_t m_componentCount;	// component count per chunck

		Chunck& AllocChunck()
		{
			Chunck chunck;
			chunck.data = malloc( (size_t)m_componentCount * m_componentSize );
			m_chunks.push_back( chunck );
			return m_chunks[m_chunks.size() - 1];
		}
	};

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	class EntityWorld
	{
	public:
		EntityWorld()
		{
			AddComponentType<PositionComponent>();
			AddComponentType<ColorComponent>();
			assert( m_components.size() < 1 << ( 8 * sizeof( ComponentID ) ) );
		}

		//================================
		template< typename _componentType >
		_componentType& AddComponent( EntityID _entityID )
		{
			Entity& entity = GetEntity( _entityID );
			assert( !entity.signature[_componentType::s_typeID] ); // this entity already have this component
			assert( entity.componentCount < Entity::s_maxComponentsPerEntity );
			_componentType& component = (_componentType&)m_components[_componentType::s_typeID].NewComponent();
			component.Init();
			component.typeID = _componentType::s_typeID;
			entity.components[entity.componentCount] = &component;
			entity.componentCount++;
			entity.signature[_componentType::s_typeID] = 1;
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
					m_components[component.typeID].RemoveComponent( component.chunckIndex, component.index );
				}
				m_entities.pop_back();
				--reverseIndex;
			}
		}



	//private:
		template< typename _componentType >
		void AddComponentType()
		{
			static_assert( std::is_base_of< ecComponent, _componentType>::value );
			_componentType::s_typeID = (ComponentID)m_components.size();
			ComponentsCollection chunck;
			chunck.Init<_componentType>( _componentType::s_typeName );
			m_components.push_back( chunck );
		}

		std::unordered_map< EntityHandle, EntityID > m_handles;
		std::vector< Entity > m_entities;
		std::vector< ComponentsCollection > m_components;
		EntityHandle m_nextHandle = 1; // 0 is a null handle
	};
}