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
	class ComponentsCollection;
	class EntityWorld;
	struct Entity;


	static constexpr uint32_t signatureLength = 32;
	using Signature = std::bitset<signatureLength>;
	static constexpr uint32_t ecAliveBit = signatureLength - 1;
	using EntityHandle = uint64_t;
	using EntityID = uint32_t;
	using ComponentID = uint8_t;
	using TagID = ComponentID;
	using SingletonComponentID = int;
	using ChunckIndex = uint8_t;
	using ComponentIndex = uint16_t;

#define DECLARE_COMPONENT()													\
	private:																\
	friend class EntityWorld;												\
	static ComponentID s_typeID;											\
	static const char* s_typeName;											\
	public:																	\
	static Signature GetSignature() { return Signature(1) << s_typeID; }	\

#define REGISTER_COMPONENT( _componentType, _name)	\
	ComponentID _componentType::s_typeID = 0;		\
	const char* _componentType::s_typeName = _name;	\

#define DECLARE_SINGLETON_COMPONENT()		\
	private:								\
	friend class EntityWorld;				\
	static SingletonComponentID s_typeID;	\
	static const char* s_typeName;			\

#define REGISTER_SINGLETON_COMPONENT( _componentType, _name)	\
	SingletonComponentID _componentType::s_typeID = -1;			\
	const char* _componentType::s_typeName = _name;				\

#define DECLARE_TAG()														\
	private:																\
	friend class EntityWorld;												\
	friend struct Entity;													\
	static TagID s_typeID;													\
	static const char* s_typeName;											\
	public:																	\
	static Signature GetSignature() { return Signature(1) << s_typeID; }	\

#define REGISTER_TAG( _componentType, _name)			\
	TagID _componentType::s_typeID;						\
	const char* _componentType::s_typeName = _name;		\

#define DECLARE_SYSTEM()						\
	private:									\
	friend class EntityWorld;					\
	static Signature s_signature;				\
	static const char* s_typeName;				\

#define REGISTER_SYSTEM( _systemType, _name)							\
	Signature _systemType::s_signature = Signature(1) << ecAliveBit;	\
	const char* _systemType::s_typeName = _name;						\

	//==============================================================================================================================================================
	// Component is a data struct that stores no logic. Components are processed through System::Run() calls
	// If your component is unique, create a SingletonComponent instead
	//
	// Component must call the (DECLARE/REGISTER)_COMPONENT macro and implement an Init() method.
	// It also must be registered in the EntityWorld constructor to be assigned a unique ID
	// - typeID is a unique id that also correspond to the index of the ComponentsCollection in the Entity world
	// - chunckIndex is the index of the Chunck in the ComponentsCollection
	// - index is the index of the component inside the chunck
	//==============================================================================================================================================================
	struct ecComponent
	{
	private:
		friend class ComponentsCollection; 
		friend class EntityWorld;
		ComponentID typeID;
		ChunckIndex chunckIndex;
		ComponentIndex index;
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

		static void Init()
		{
			s_signature = 
				  tag_editorOnly::GetSignature() 
				| ColorComponent::GetSignature();
		}

		static void Run( EntityWorld& _world, const std::vector<Entity*>& _entities, const float _delta )
		{

		}
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
	// Contains the entities, components, singleton components, type information
	// and various utilities for processing them
	//==============================================================================================================================================================
	class EntityWorld
	{
	public:
		EntityWorld()
		{
			AddSingletonComponentType<sc_sunLight>();

			// Changing this order will invalidate all the save files
			AddComponentType<PositionComponent>();
			AddComponentType<ColorComponent>();
			assert( m_components.size() < 1 << ( 8 * sizeof( ComponentID ) ) );			

			m_nextTagID = ( TagID)m_components.size();

			AddTagType<tag_alwaysUpdate>();
			AddTagType<tag_editorOnly>();

			assert( m_nextTagID < 1 << ( 8 * sizeof( ComponentID ) ) );

			AddSystemType < UpdateAABBFromRigidbodySystem >();
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
		template< typename _tagType >
		void AddTag( EntityID _entityID  )
		{
			static_assert( std::is_base_of< Tag, _tagType>::value );
			Entity& entity = GetEntity( _entityID );
			entity.signature[_tagType::s_typeID] = 1;
		}

		//================================
		template< typename _componentType >
		_componentType& GetSingletonComponent()
		{
			static_assert( std::is_base_of< SingletonComponent, _componentType>::value );
			return  * static_cast<_componentType*>(m_singletonComponents[_componentType::s_typeID]);
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

		//================================================================================================================================
		// Find all entities matching the signature of the system and runs it
		//================================================================================================================================
		template< typename _systemType >
		void RunSystem( const float _delta )
		{
			static_assert( std::is_base_of< System, _systemType>::value );
			std::vector<Entity*> matchEntities;

			const Signature systemSignature = _systemType::s_signature;

			matchEntities.reserve( m_entities.size() );
			for (int entityIndex = 0; entityIndex < m_entities.size() ; entityIndex++)
			{
				Entity& entity = m_entities[entityIndex];
				if( (entity.signature & systemSignature ) == systemSignature )
				{
					matchEntities.push_back( &entity );
				}

				_systemType::Run( *this, matchEntities, _delta );				
			}
		}		

	//private:


		template< typename _componentType >
		void AddComponentType()
		{
			static_assert( std::is_base_of< ecComponent, _componentType>::value );
			if( _componentType::s_typeID == 0 )
			{
				_componentType::s_typeID = (ComponentID)m_components.size();
			}			
			ComponentsCollection chunck;
			chunck.Init<_componentType>( _componentType::s_typeName );
			m_components.push_back( chunck );
		}

		template< typename _tagType >
		void AddTagType()
		{
			static_assert( std::is_base_of< Tag, _tagType>::value );
			if( _tagType::s_typeID == 0 )
			{				
				_tagType::s_typeID = m_nextTagID++;
			}
		}

		template< typename _componentType >
		void AddSingletonComponentType()
		{
			static_assert( std::is_base_of< SingletonComponent, _componentType>::value );
			if( _componentType::s_typeID == -1 )
			{
				_componentType::s_typeID = (SingletonComponentID)m_singletonComponents.size();
			}			
			m_singletonComponents.push_back( new _componentType() );
		}

		template< typename _systemType >
		void AddSystemType()
		{
			static_assert( std::is_base_of< System, _systemType>::value );
			_systemType::Init();
		}

		std::unordered_map< EntityHandle, EntityID > m_handles;
		std::vector< Entity > m_entities;
		std::vector< ComponentsCollection > m_components;
		std::vector< SingletonComponent* > m_singletonComponents;
		EntityHandle m_nextHandle = 1; // 0 is a null handle
		TagID m_nextTagID = 0;
	};
}