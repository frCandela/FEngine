#pragma once

#include "scene/fanScenePrecompiled.hpp"
#include "core/fanHash.hpp"

namespace fan
{
	class ComponentsCollection;
	class EntityWorld;
	struct Entity;
	struct ecComponent;

	static constexpr uint32_t signatureLength = 32;
	using Signature = std::bitset<signatureLength>;
	static constexpr uint32_t ecAliveBit = signatureLength - 1;
	using EntityHandle = uint64_t;
	using EntityID = uint32_t;
	using ComponentIndex = uint8_t;
	using SingletonComponentID = int;
	using ChunckIndex = uint8_t;
	using ChunckComponentIndex = uint16_t;

#define DECLARE_COMPONENT( _componentType)															\
	private:																						\
	friend class EntityWorld;																		\
	static const uint32_t s_typeInfo;																\
	static const char* s_typeName;																	\
	static ecComponent& Instanciate( void * _buffer){ return *new( _buffer ) _componentType();}	\

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
	// Component must call the (DECLARE/REGISTER)_COMPONENT macro and implement an Clear() method.
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
		ComponentIndex	GetTypeIndex()	const { return componentIndex; };
		Signature		GetSignature()	const { return Signature( 1 ) << componentIndex; }
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

		bool HasTag( const ComponentIndex _index ) const		{ return signature[_index] == 1;		}
		bool HasComponent( const ComponentIndex _index ) const	{ return signature[_index] == 1;		}
		bool IsAlive() const									{ return  signature[ecAliveBit] == 1;	}
		void Kill()												{ signature[ecAliveBit] = 0;			}
	};
	static constexpr size_t sizeEntity = sizeof( Entity );
	static_assert( sizeEntity == 128 );

	//==============================================================================================================================================================
	// A System is a static function Run() with no state that processes entites
	//
	// System must call the (DECLARE/REGISTER)_SYSTEM macro and implement Clear() method that set its signature.
	// It also must implement a static Run(..) method that runs its logic.
	// It also must be registered in the EntityWorld constructor to be assigned a unique signature
	//==============================================================================================================================================================
	struct System
	{};

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
	struct tag_editorOnly : Tag { DECLARE_TAG() };
	struct tag_alwaysUpdate : Tag { DECLARE_TAG() };	// Updates even when the scene is not playing

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
}