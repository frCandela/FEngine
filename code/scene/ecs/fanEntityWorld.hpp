#pragma once

// #include <iostream>
// #include <bitset>
// #include <vector>
// #include <array>
// #include <unordered_map>
// #include <cassert>
#include "scene/fanScenePrecompiled.hpp"
#include "core/fanHash.hpp"

namespace fan
{
	class ComponentsCollection;
	class EntityWorld;
	struct Entity;


	static constexpr uint32_t signatureLength = 32;
	using Signature = std::bitset<signatureLength>;
	static constexpr uint32_t ecAliveBit = signatureLength - 1;
	using EntityHandle = uint64_t;
	using EntityID = uint32_t;
	using ComponentIndex = uint8_t;
	using SingletonComponentID = int;
	using ChunckIndex = uint8_t;
	using ChunckComponentIndex = uint16_t;

#define DECLARE_COMPONENT()													\
	private:																\
	friend class EntityWorld;												\
	static const uint32_t s_typeInfo;										\
	static const char* s_typeName;											\

#define REGISTER_COMPONENT( _componentType, _name)				\
	const uint32_t _componentType::s_typeInfo = SSID(#_name);	\
	const char* _componentType::s_typeName = _name;				\

#define DECLARE_SINGLETON_COMPONENT()		\
	private:								\
	friend class EntityWorld;				\
	static const uint32_t s_typeInfo;		\
	static const char* s_typeName;			\

#define REGISTER_SINGLETON_COMPONENT( _componentType, _name)	\
	const uint32_t _componentType::s_typeInfo = SSID(#_name);	\
	const char* _componentType::s_typeName = _name;				\

#define DECLARE_TAG()													\
	public:																\
	static const uint32_t s_typeInfo;									\
	static const char* s_typeName;										\

#define REGISTER_TAG( _componentType, _name)					\
	const uint32_t _componentType::s_typeInfo = SSID(#_name);	\
	const char* _componentType::s_typeName = _name;				\

#define DECLARE_SYSTEM()						\
	private:									\
	friend class EntityWorld;					\
	static const char* s_typeName;				\

#define REGISTER_SYSTEM( _systemType, _name)							\
	const char* _systemType::s_typeName = _name;						\

	//==============================================================================================================================================================
	// Component is a data struct that stores no logic. Components are processed through System::Run() calls
	// If your component is unique, create a SingletonComponent instead
	//
	// Component must call the (DECLARE/REGISTER)_COMPONENT macro and implement an Init() method.
	// It also must be registered in the EntityWorld constructor to be assigned a unique ID
	// - localTypeID is a unique id that also correspond to the index of the ComponentsCollection in its specific Entity world
	// - chunckIndex is the index of the Chunck in the ComponentsCollection
	// - index is the index of the component inside the chunck
	//==============================================================================================================================================================
	struct ecComponent
	{
	private:
		friend class ComponentsCollection; 
		friend class EntityWorld;
		ComponentIndex		 componentIndex;
		ChunckIndex			 chunckIndex;
		ChunckComponentIndex chunckComponentIndex;

	public:
		Signature GetSignature() const { return Signature( 1 ) << componentIndex; }
	};
	static constexpr size_t sizeComponent = sizeof( ecComponent );
	static_assert( sizeComponent == 4 );

	//==============================================================================================================================================================
	// A singleton is a unique component, it can be accessed by systems
	//==============================================================================================================================================================
	struct SingletonComponent {};

	//==============================================================================================================================================================
	// A Tag is a component with no data, it is used to change the signature of entities for more fine grained system calls
	//==============================================================================================================================================================
	struct Tag {};

	//==============================================================================================================================================================
	// An Entity is a wrapper class for components
	//
	// - signature is a bitset representing the components referenced by the entity
	// - handle is a unique key that allows access to an entity,( 0 == invalid )
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

		template< typename _tagType >
		bool HasTag() const  
		{ 
			static_assert( std::is_base_of< Tag, _tagType>::value );
			return signature[_tagType::s_typeID] == 1;
		}
		//================================
		template< typename _componentType >
		bool HasComponent() const { return signature[_componentType::s_typeID] == 1; }
		void Kill() { signature[ecAliveBit] = 0; }
		bool IsAlive() const { return  signature[ecAliveBit] == 1; }
	};
	static constexpr size_t sizeEntity = sizeof( Entity );
	static_assert( sizeEntity == 128 );

	//==============================================================================================================================================================
	// A System is a static function Run() with no state that processes entites
	//
	// System must call the (DECLARE/REGISTER)_SYSTEM macro and implement Init() method that set its signature.
	// It also must implement a static Run(..) method that runs its logic.
	// It also must be registered in the EntityWorld constructor to be assigned a unique signature
	//==============================================================================================================================================================
	struct System 
	{
	};

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct PositionComponent : public ecComponent
	{	
		DECLARE_COMPONENT()
	public:
		void Init() { x = y = z = 0.f; }
		float x;
		float y;
		float z;		
	};

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct ColorComponent : public ecComponent
	{
		DECLARE_COMPONENT()
	public:
		void Init() { r = g = b = a = 0; }
		char r;
		char g;
		char b;
		char a;
	};

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct sc_sunLight : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		int test = 0;
	};

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct tag_editorOnly	: Tag { DECLARE_TAG()	};
	struct tag_alwaysUpdate : Tag { DECLARE_TAG()	};	// Updates even when the scene is not playing

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct UpdateAABBFromRigidbodySystem : System
	{
		DECLARE_SYSTEM()
	public:

		static Signature GetSignature( const EntityWorld& _world );
		static void Run( EntityWorld& _world, const std::vector<Entity*>& _entities, const float _delta );
	};

	//==============================================================================================================================================================
	// ComponentsCollection holds an array of chunks that store components.
	//
	// components size are defined in the Init, chunks maximum size is 65536 bytes
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
		void* At( const ChunckIndex _chunckIndex, const ChunckComponentIndex _componentIndex )
		{
			uint8_t* data = static_cast<uint8_t*>( m_chunks[_chunckIndex].data );
			uint8_t* component = data + m_componentSize * (uint32_t)_componentIndex;
			return component;
		}

		//================================
		// Deletes a component
		void RemoveComponent( const ChunckIndex _chunckIndex, const ChunckComponentIndex _index )
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
					const ChunckComponentIndex index = chunck.recycleList[chunck.recycleList.size() - 1];
					chunck.recycleList.pop_back();
					chunck.count++;
					ecComponent& component = *static_cast<ecComponent*>( At( chunckIndex, index ) );
					component.chunckIndex = chunckIndex;
					component.chunckComponentIndex = index;
					return component;
				}
				else if( chunck.count < m_componentCount )
				{
					// create index
					const ChunckComponentIndex index = chunck.count;
					chunck.count++;
					ecComponent& component = *static_cast<ecComponent*>( At( chunckIndex, index ) );
					component.chunckIndex = chunckIndex;
					component.chunckComponentIndex = index;
					return component;
				}
			}

			// create chunck
			Chunck& newChunck = AllocChunck();
			ecComponent& component = *static_cast<ecComponent*>( At( (ChunckIndex)m_chunks.size() - 1, 0 ) );
			component.chunckIndex = static_cast<ChunckIndex>( m_chunks.size() - 1 );
			component.chunckComponentIndex = 0;
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
			std::vector< ChunckComponentIndex> recycleList;
			ChunckComponentIndex count = 0;
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
	// Contains the entities, components, singleton components, type information
	// and various utilities for processing them
	//==============================================================================================================================================================
	class EntityWorld
	{
	public:
		EntityWorld()
		{
			AddSingletonComponentType<sc_sunLight>();

			AddComponentType<PositionComponent>();
			AddComponentType<ColorComponent>();

			AddTagType<tag_alwaysUpdate>();
			AddTagType<tag_editorOnly>();

			assert( m_nextTypeIndex < 1 << ( 8 * sizeof( ComponentIndex ) ) );

		}

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