#include <bitset>
#include <unordered_map>
#include <memory>
#include <cassert>
#include "core/fanHash.hpp"


namespace fan
{
//================================
//================================
#define DECLARE_COMPONENT2( _componentType)									\
	public:																	\
	template <class T> struct ComponentInfoImpl	{							\
		static constexpr uint32_t	 s_size		{ sizeof( T )			  };\
		static constexpr uint32_t	 s_alignment{ alignof( T )			  };\
		static constexpr const char* s_name		{ #_componentType		  };\
		static constexpr uint32_t	 s_type		{ SSID( #_componentType ) };\
	};																		\
	using Info = ComponentInfoImpl< _componentType >;


	//================================
	//================================
	static constexpr uint32_t signatureLength2 = 32;
	using Signature2 = std::bitset<signatureLength2>;
	using EntityID2 = uint32_t;
	using ComponentIndex2 = uint8_t;
	static constexpr uint32_t aliveBit2 = signatureLength2 - 1;
	

	//================================
	//================================
	struct Entity2
	{
		Signature2 signature;	// signature of all components & tags
		//uint16_t  chunkIndex;	// index of the chunk  in the archetype
		uint16_t  index;		// index of the entity in the archetype chunk
	};

	//================================
	//================================
	struct Component2{};

	//================================
	//================================
	struct Position2 : Component2
	{
		DECLARE_COMPONENT2( Position2 );

		float position[2];
	};

	//================================
	//================================
	struct Speed2 : Component2
	{
		DECLARE_COMPONENT2( Speed2 );
		float speed[2];
	};

	//================================
	//================================
	struct Expiration2 : Component2
	{
		DECLARE_COMPONENT2( Expiration2 );
		float timeLeft;
	};

	//================================
	//================================
	struct ComponentInfo2
	{
		uint32_t	size;
		uint32_t	alignment;
		std::string name;
		uint32_t	type;
	};

	//================================
	//================================
	struct Chunk2
	{
		static const size_t s_chunkMaxSize = 65536*1000;
		size_t m_capacity;
		size_t m_size;
		size_t m_componentSize;
		void* m_buffer;
		void* m_alignedBuffer;

		void Create( const size_t _componentSize, const size_t _alignment )
		{
			m_componentSize = _componentSize;
			m_capacity = s_chunkMaxSize / _componentSize;
			m_size = 0;

			size_t chunckSize = m_capacity * _componentSize;
			m_buffer = new uint8_t[ chunckSize + _alignment ];

			size_t space;
			m_alignedBuffer = std::align( _alignment, chunckSize, m_buffer, space );
		}

		void* At( const size_t _index )
		{
			uint8_t* buffer = static_cast<uint8_t*>( m_alignedBuffer );
			return & buffer[_index * m_componentSize];
		}

		void Remove( const size_t _index )
		{		
			// swap elements
			if( m_size != 1 && _index != m_size - 1 )
			{
				std::memcpy( At( _index ), At( m_size - 1 ), m_componentSize );
			}
			m_size--;
		}

		void PushBack( void* _data )
		{ 
			assert( m_size < m_capacity );
			std::memcpy( At( m_size ), _data, m_componentSize );
			m_size++;
		}

		void* Append()
		{
			assert( m_size < m_capacity );
			void* component = At( m_size );
			m_size++;
			return component;
		}
	};

	//================================
	//================================
	struct Archetype
	{
		Signature2 m_signature;
		std::vector< Chunk2* > m_chunks; // one chunk per component type for now
		size_t m_size;					 // num entities in the archetype

		void Create( const std::vector< ComponentInfo2 >& _componentsInfo, const Signature2& _signature )
		{
			m_size = 0;
			m_signature = _signature;
			m_chunks.resize( m_signature.size() );
			for ( int i = 0; i < m_signature.size() - 1; i++ ) // last one is the alive bit
			{
				if( m_signature[i] )
				{
					const ComponentInfo2& info = _componentsInfo[i];
					m_chunks[i] = new Chunk2();
					m_chunks[i]->Create( info.size, info.alignment );
				}
				else
				{
					m_chunks[i] = nullptr;
				}
				

			}
		}
	};

	//================================
	//================================
	struct EcsWorld2
	{
		std::unordered_map< Signature2, Archetype >	 m_archetypes;
		std::vector< ComponentInfo2 >				 m_componentsInfo;
		std::vector< Entity2 >						 m_entities;
		std::unordered_map<uint32_t, ComponentIndex2> m_typeToIndex;

		Archetype* FindArchetype( const Signature2 _signature )
		{
			auto it = m_archetypes.find( _signature );
			return it == m_archetypes.end() ? nullptr : &it->second;
		}

		Archetype& CreateArchetype( const Signature2 _signature )
		{
			assert( FindArchetype( _signature ) == nullptr );
			m_archetypes.emplace( _signature, Archetype() );
			Archetype& archetype = m_archetypes[_signature];
			archetype.Create( m_componentsInfo, _signature );
			return archetype;
		}

		template <typename _ComponentType > 
		void AddComponentType()
		{
			const ComponentIndex2 index = static_cast<uint32_t>( m_componentsInfo.size() );

			ComponentInfo2 info;
			info.size		= _ComponentType::Info::s_size;
			info.alignment	= _ComponentType::Info::s_alignment;
			info.name		= _ComponentType::Info::s_name;
			info.type		= _ComponentType::Info::s_type;
			m_componentsInfo.push_back( info );

			m_typeToIndex[_ComponentType::Info::s_type] = index;
		}

		Component2&  AddComponent( const EntityID2 _entityID, const uint32_t s_type )
		{
			const uint32_t index = m_typeToIndex[ s_type ];
			Entity2& entity = m_entities[ _entityID ];
			assert( !entity.signature[index] ); // entity doesn't already have this component

			// Get signatures
			const Signature2 oldSignature = entity.signature;
			const Signature2 newSignature = entity.signature | Signature2(1) << index;
			entity.signature = newSignature;

			// Get archetypes
			Archetype* oldArchetype = FindArchetype( oldSignature );
			Archetype* newArchetype = FindArchetype( newSignature );
			if( newArchetype == nullptr )
			{
				newArchetype = &CreateArchetype( entity.signature );
			}

			// copy old components from the old archetype to the new archetype
			if( oldArchetype != nullptr )
			{
				oldArchetype->m_size--;
				for( int i = 0; i < m_componentsInfo.size(); i++ )
				{
					if( oldSignature[i] )
					{
						newArchetype->m_chunks[i]->PushBack( oldArchetype->m_chunks[i]->At( entity.index ) );
						oldArchetype->m_chunks[i]->Remove( entity.index );						
					}
				}
			}
			newArchetype->m_size++;

			// create new component
			Component2* component = static_cast<Component2*>( newArchetype->m_chunks[index]->Append() );
			
			// @todo remove this test
			for( const Chunk2* chunk : newArchetype->m_chunks )
			{
				if( chunk != nullptr )
				{
					assert( chunk->m_size == newArchetype->m_size );
				}
			}
			return *component;
		}

		EntityID2 CreateEntity()
		{
			m_entities.push_back( {Signature2( 1 ) << aliveBit2} );
			return (EntityID2)m_entities.size() - 1;
		}
	};
}