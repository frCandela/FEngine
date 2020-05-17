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
	static constexpr uint32_t signatureLength2 = 4;//256;
	using Signature2 = std::bitset<signatureLength2>;
	using EntityID2 = uint32_t;
	using ComponentIndex2 = uint16_t;
	static constexpr uint32_t aliveBit2 = signatureLength2 - 1;
	static constexpr size_t chunkMaxSize = 65536;

	//================================
	//================================
	struct Component2 {};

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
	class Chunk
	{
	public:
		void Create( const size_t _componentSize, const size_t _alignment )
		{
			m_componentSize = _componentSize;
			m_capacity = uint16_t( chunkMaxSize / _componentSize );
			m_size = 0;

			size_t chunckSize = m_capacity * _componentSize;
			m_buffer = new uint8_t[chunckSize + _alignment];

			size_t space = chunckSize + _alignment;
			m_alignedBuffer = m_buffer;
			std::align( _alignment, _componentSize, m_alignedBuffer, space );
		}

		void Destroy()
		{
			delete m_buffer;
			m_buffer = nullptr;
			m_alignedBuffer = nullptr;
		}

		bool		Empty() const { return m_size == 0; }
		bool		Full() const { return m_size == m_capacity; }
		uint16_t	Size() const { return m_size; }
		uint16_t	Capacity() const { return m_capacity; }

		void* At( const size_t _index )
		{
			assert( _index < m_size );
			uint8_t* buffer = static_cast<uint8_t*>( m_alignedBuffer );
			return &buffer[_index * m_componentSize];
		}

		void Set( const uint16_t _index, void* _data )
		{
			assert( _index < m_size );
			std::memcpy( At( _index ), _data, m_componentSize );
		}

		void Remove( const uint16_t _index )
		{
			assert( _index < m_size );
			// back swap
			if( m_size != 1 && _index != m_size - 1 )
			{
				Set( _index, At( m_size - 1 ) );
			}
			m_size--;
		}

		void PushBack( void* _data )
		{
			assert( m_size < m_capacity );
			const uint16_t index = m_size;
			m_size++;
			Set( index, _data );
		}

		void PopBack()
		{
			assert( m_size > 0 );
			m_size--;
		}

		void EmplaceBack()
		{
			assert( m_size < m_capacity );
			m_size++;
		}

	private:
		uint16_t m_capacity = 0;
		uint16_t m_size;
		size_t m_componentSize;
		void* m_buffer;
		void* m_alignedBuffer;
	};

	//================================
	//================================
	class ChunkVector
	{
	public:
		//================================
		//================================
		struct Index
		{
			uint16_t chunkIndex;	// index of the chunk
			uint16_t elementIndex;	// index of the data in the chunk
		};

		void Create( const size_t _componentSize, const size_t _alignment )
		{
			m_componentSize = _componentSize;
			m_alignment = _alignment;
			m_chunks.emplace_back();
			m_chunks.rbegin()->Create( m_componentSize, m_alignment );
		}

		void Remove( const Index& _index )
		{
			assert( _index.chunkIndex < m_chunks.size() );
			assert( _index.elementIndex < m_chunks[_index.chunkIndex].Size() );
			Chunk& chunk = m_chunks[_index.chunkIndex];

			// Last chunk ? just remove the element & back swap locally in the chunk
			if( _index.chunkIndex == m_chunks.size() - 1 )
			{
				chunk.Remove( _index.elementIndex );
			}
			else
			{
				// back swap the removed element with the last element of the last chunk
				Chunk& lastChunk = *m_chunks.rbegin();
				chunk.Set( _index.elementIndex, lastChunk.At( lastChunk.Size() - 1 ) );
				lastChunk.PopBack();
			}

			Chunk& lastChunk = *m_chunks.rbegin();
			if( lastChunk.Empty() && m_chunks.size() > 1 )
			{
				lastChunk.Destroy();
				m_chunks.pop_back();
			}
		}

		void* At( const Index& _index )
		{
			return m_chunks[_index.chunkIndex].At( _index.elementIndex );
		}

		Index PushBack( void* _data )
		{
			// Create a new chunk if necessary
			if( m_chunks.rbegin()->Full() )
			{
				m_chunks.push_back( Chunk() );
				m_chunks.rbegin()->Create( m_componentSize, m_alignment );
			}

			// Push to the last chunk
			Chunk& chunk = *m_chunks.rbegin();
			Index index;
			index.chunkIndex = uint16_t( m_chunks.size() - 1 );
			index.elementIndex = chunk.Size();
			chunk.PushBack( _data );
			return index;
		}

		Index EmplaceBack()
		{
			// Create a new chunk if necessary
			if( m_chunks.rbegin()->Full() )
			{
				m_chunks.push_back( Chunk() );
				m_chunks.rbegin()->Create( m_componentSize, m_alignment );
			}

			// Emplace to the last chunk
			Chunk& chunk = *m_chunks.rbegin();
			Index index;
			index.chunkIndex = uint16_t( m_chunks.size() - 1 );
			index.elementIndex = chunk.Size();
			chunk.EmplaceBack();
			return index;
		}

		Chunk& GetChunk( const size_t _index ) { return m_chunks[_index]; }
		uint16_t NumChunk() const { return uint16_t( m_chunks.size() ); }
	private:
		std::vector<Chunk> m_chunks;
		size_t m_componentSize;
		size_t m_alignment;
	};

	//================================
	//================================
	class Archetype
	{
	public:
		void Create( const std::vector< ComponentInfo2 >& _componentsInfo, const Signature2& _signature )
		{
			const size_t numComponents = _componentsInfo.size();

			m_size = 0;
			m_signature = _signature;
			m_chunks.resize( numComponents );

			for( int i = 0; i < numComponents; i++ )
			{
				if( m_signature[i] )
				{
					const ComponentInfo2& info = _componentsInfo[i];
					m_chunks[i].Create( info.size, info.alignment );
				}
			}
		}

		bool Empty() const { return m_size == 0; }

		Signature2					m_signature;
		std::vector< ChunkVector >	m_chunks;		// components are contained in a list of small buffers
		uint32_t					m_size;			// number of entities in the archetype
	};

	//================================
	//================================
	struct Entity2
	{
		Signature2	signature;	// signature of all components & tags
		Archetype* archetype;
		ChunkVector::Index index;
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
			info.size = _ComponentType::Info::s_size;
			info.alignment = _ComponentType::Info::s_alignment;
			info.name = _ComponentType::Info::s_name;
			info.type = _ComponentType::Info::s_type;
			m_componentsInfo.push_back( info );

			m_typeToIndex[_ComponentType::Info::s_type] = index;
		}

		Component2& AddComponent( const EntityID2 _entityID, const uint32_t s_type )
		{
			const uint32_t index = m_typeToIndex[s_type];
			Entity2& entity = m_entities[_entityID];
			assert( !entity.signature[index] ); // entity doesn't already have this component

			// Get signatures
			const Signature2 oldSignature = entity.signature;
			const Signature2 newSignature = entity.signature | Signature2( 1 ) << index;
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
				const size_t numComponents = m_componentsInfo.size();
				for( int i = 0; i < numComponents; i++ )
				{
					if( oldArchetype->m_signature[i] )
					{
						newArchetype->m_chunks[i].PushBack( oldArchetype->m_chunks[i].At( entity.index ) );
						oldArchetype->m_chunks[i].Remove( entity.index );
					}
				}
				oldArchetype->m_size--;
			}
			newArchetype->m_size++;

			// create new component
			entity.index = newArchetype->m_chunks[index].EmplaceBack();

			return *static_cast<Component2*>( newArchetype->m_chunks[index].At( entity.index ) );
		}

		EntityID2 CreateEntity()
		{
			m_entities.push_back( { Signature2( 1 ) << aliveBit2 } );
			return (EntityID2)m_entities.size() - 1;
		}
	};

	static const size_t sdfsdfsdf = sizeof( EcsWorld2 ) / 1000 / 1000;

	//================================
	//================================
	struct MatchComponents
	{
		MatchComponents( EcsWorld2& _world ) : m_world( _world )
		{
		}

		EcsWorld2& m_world;
		std::vector<Archetype*> m_archetypes;


		//================================
		//================================
		template < typename _ComponentType >
		struct Iterator
		{
			Iterator( const ComponentIndex2 _componentIndex, MatchComponents& _matchComponents )
			{
				for( Archetype* archetype : _matchComponents.m_archetypes )
				{
					ChunkVector& chunks = archetype->m_chunks[_componentIndex];
					for( int i = 0; i < chunks.NumChunk(); i++ )
					{
						m_chunks.push_back( &chunks.GetChunk( i ) );
					}
				}

				chunkIndex = 0;
				elementIndex = 0;
			}

			std::vector< Chunk* > m_chunks;
			uint16_t chunkIndex;
			uint16_t elementIndex;

			void operator++() // prefix ++
			{
				++elementIndex;
				if( elementIndex >= m_chunks[chunkIndex]->Size() )
				{
					elementIndex = 0;
					++chunkIndex;
				}
			}

			bool End()
			{
				return chunkIndex >= m_chunks.size();
			}

			inline _ComponentType& operator*()
			{
				return *static_cast<_ComponentType*>( m_chunks[chunkIndex]->At( elementIndex ) );
			}
		}; static constexpr size_t itSize = sizeof( Iterator<Position2> );

		template < typename _ComponentType >
		Iterator<_ComponentType> Begin()
		{
			const ComponentIndex2 index = m_world.m_typeToIndex[_ComponentType::Info::s_type];

			return Iterator<_ComponentType>( index, *this );
		}
	};
}