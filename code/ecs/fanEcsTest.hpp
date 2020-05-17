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
	static constexpr uint32_t signatureLength2 = 8;
	using Signature2 = std::bitset<signatureLength2>;
	using EntityID2 = uint32_t;
	using EntityHandle2 = uint32_t;
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
		void Create( const int _componentSize, const int _alignment )
		{
			m_componentSize = _componentSize;
			m_capacity =  chunkMaxSize / _componentSize;
			m_size = 0;

			int chunckSize = m_capacity * _componentSize;
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

		bool	Empty() const { return m_size == 0; }
		bool	Full() const { return m_size == m_capacity; }
		int		Size() const { return m_size; }
		int		Capacity() const { return m_capacity; }

		void* At( const int _index )
		{
			assert( _index < m_size );
			uint8_t* buffer = static_cast<uint8_t*>( m_alignedBuffer );
			return &buffer[_index * m_componentSize];
		}

		void Set( const int _index, void* _data )
		{
			assert( _index < m_size );
			std::memcpy( At( _index ), _data, m_componentSize );
		}

		void Remove( const int _index )
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
			const int index = m_size;
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
		int m_capacity = 0;
		int m_size;
		int m_componentSize;
		void* m_buffer;
		void* m_alignedBuffer;
	};

	//================================
	//================================
	class ChunkVector
	{
	public:
		void Create( const int _componentSize, const int _alignment )
		{
			m_componentSize = _componentSize;
			m_alignment = _alignment;
			m_chunks.emplace_back();
			m_chunks.rbegin()->Create( m_componentSize, m_alignment );
			m_chunkCapacity = m_chunks.rbegin()->Capacity();
		}

		void Remove( const int& _index )
		{
			const int chunkIndex = _index / m_chunkCapacity;
			const int elementIndex = _index % m_chunkCapacity;
			assert( chunkIndex < m_chunks.size() );
			assert( elementIndex < m_chunks[chunkIndex].Size() );

			Chunk& chunk = m_chunks[chunkIndex];

			// Last chunk ? just remove the element & back swap locally in the chunk
			if( chunkIndex == m_chunks.size() - 1 )
			{
				chunk.Remove( elementIndex );
			}
			else
			{
				// back swap the removed element with the last element of the last chunk
				Chunk& lastChunk = *m_chunks.rbegin();
				chunk.Set( elementIndex, lastChunk.At( lastChunk.Size() - 1 ) );
				lastChunk.PopBack();
			}

			Chunk& lastChunk = *m_chunks.rbegin();
			if( lastChunk.Empty() && m_chunks.size() > 1 )
			{
				lastChunk.Destroy();
				m_chunks.pop_back();
			}
		}

		void* At( const int& _index )
		{
			const int chunkIndex = _index / m_chunkCapacity;
			const int elementIndex = _index % m_chunkCapacity;
			assert( chunkIndex < m_chunks.size() );
			assert( elementIndex < m_chunks[chunkIndex].Size() );
			
			return m_chunks[chunkIndex].At( elementIndex );
		}

		int PushBack( void* _data )
		{
			// Create a new chunk if necessary
			if( m_chunks.rbegin()->Full() )
			{
				m_chunks.push_back( Chunk() );
				m_chunks.rbegin()->Create( m_componentSize, m_alignment );
			}

			// Push to the last chunk
			Chunk& chunk = *m_chunks.rbegin();
			const int chunkIndex = int( m_chunks.size() - 1 );
			const int elementIndex = chunk.Size();
			chunk.PushBack( _data );
			return chunkIndex * m_chunkCapacity + elementIndex;
		}

		int EmplaceBack()
		{
			// Create a new chunk if necessary
			if( m_chunks.rbegin()->Full() )
			{
				m_chunks.push_back( Chunk() );
				m_chunks.rbegin()->Create( m_componentSize, m_alignment );
			}

			// Emplace to the last chunk
			Chunk& chunk = *m_chunks.rbegin();
			const int chunkIndex = int( m_chunks.size() - 1 );
			const int elementIndex = chunk.Size();
			chunk.EmplaceBack();
			return chunkIndex * m_chunkCapacity + elementIndex;
		}

		
		Chunk& GetChunk( const int _index ) { return m_chunks[_index]; }
		int NumChunk() const { return int( m_chunks.size() ); }
	private:
		std::vector<Chunk> m_chunks;
		int m_componentSize;
		int m_alignment;
		int m_chunkCapacity;
	};

	//================================
	//================================
	class Archetype
	{
	public:
		struct EntityData
		{
			bool isDead  = false;
		};

		void Create( const std::vector< ComponentInfo2 >& _componentsInfo, const Signature2& _signature )
		{
			const size_t numComponents = _componentsInfo.size();

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

		void RemoveEntity( const int _entityIndex )
		{
			m_entities[_entityIndex] = *m_entities.rbegin();
			m_entities.pop_back();
		}

		int		Size() const  { return int(m_entities.size());  }
		bool	Empty() const { return m_entities.empty(); }

		Signature2					m_signature;
		std::vector< ChunkVector >	m_chunks;		// one index per component type
		std::vector<EntityData>		m_entities;	
	};

	//================================
	//================================
	struct Entity2
	{		
		Archetype*		archetype = nullptr;
		EntityHandle2	handle	  = 0;
		uint32_t		index = 0;	// index of the entity in the archetype
	};

	//================================
	//================================
	struct EcsWorld2
	{
		std::unordered_map< Signature2, Archetype* >	m_archetypes;
		std::vector< ComponentInfo2 >					m_componentsInfo;
		std::vector< Entity2 >							m_entities;
		std::unordered_map<uint32_t, ComponentIndex2>	m_typeToIndex;
		std::unordered_map< EntityHandle2, EntityID2 >	m_handles;
		EntityHandle2									m_nextHandle = 1; // 0 is a null handle

		int NumComponents() const 
		{
			return int(m_componentsInfo.size());
		}

		void EndFrame()
		{
			for ( auto pair : m_archetypes)
			{
				Archetype& archetype = *pair.second;
				for (int entityIndex = archetype.Size() - 1; entityIndex >= 0 ; entityIndex--)
				{
					if( archetype.m_entities[entityIndex].isDead )
					{
						for( int componentIndex = 0; componentIndex < NumComponents(); componentIndex++ )
						{
							if( archetype.m_signature[componentIndex] )
							{
								archetype.m_chunks[componentIndex].Remove( entityIndex );
							}
						}
						archetype.RemoveEntity( entityIndex );
					}
				}				
			}

			// update handles
			m_entities.clear();
		}

		void Kill( Archetype& _archetype,  const uint32_t _index )
		{
			_archetype.m_entities[_index].isDead = true;
		}

		EntityHandle CreateHandle( const EntityID2 _entityID )
		{
			Entity2& entity = m_entities[_entityID];
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

		Archetype* FindArchetype( const Signature2 _signature )
		{
			auto it = m_archetypes.find( _signature );
			return it == m_archetypes.end() ? nullptr : it->second;
		}

		Archetype& CreateArchetype( const Signature2 _signature )
		{
			assert( FindArchetype( _signature ) == nullptr );
			Archetype* newArchetype = new Archetype();
			newArchetype->Create( m_componentsInfo, _signature );
			m_archetypes[_signature] = newArchetype;
			return *newArchetype;
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
		
		Component2& AddComponent( EntityID2& _entityID, const uint32_t s_type )
		{
			Entity2& entity = m_entities[_entityID];
			const uint32_t index = m_typeToIndex[s_type];
			assert( entity.archetype == nullptr ||  !entity.archetype->m_signature[index] ); // entity doesn't already have this component

			const Signature2 newSignature = entity.archetype != nullptr ?
				( ( entity.archetype->m_signature ) | ( Signature2( 1 ) << index ) ) :
				( ( Signature2( 1 ) << aliveBit2 )  | ( Signature2( 1 ) << index ) );


			Archetype* oldArchetype = entity.archetype;
			Archetype* newArchetype = FindArchetype( newSignature );
			if( newArchetype == nullptr )
			{
				newArchetype = &CreateArchetype( newSignature );
			}
			entity.archetype = newArchetype;

			// copy old components from the old archetype to the new archetype
			if( oldArchetype != nullptr )
			{
				for( int i = 0; i < NumComponents(); i++ )
				{
					if( oldArchetype->m_signature[i] )
					{
						newArchetype->m_chunks[i].PushBack( oldArchetype->m_chunks[i].At( entity.index ) );
						oldArchetype->m_chunks[i].Remove( entity.index );
					}

				}
				newArchetype->m_entities.push_back( oldArchetype->m_entities[entity.index] );
				oldArchetype->RemoveEntity( entity.index );
			}
			else
			{
				newArchetype->m_entities.emplace_back();
			}

			// create new component
			entity.index = newArchetype->m_chunks[index].EmplaceBack();

			return *static_cast<Component2*>( newArchetype->m_chunks[index].At( entity.index ) );
		}

		EntityID2 CreateEntity()
		{
			m_entities.emplace_back();
			return EntityID2(m_entities.size() - 1);
		}
	};

	static const size_t sdfsdfsdf = sizeof( EcsWorld2 ) / 1000 / 1000;

	//================================
	//================================
	struct SystemView
	{
		SystemView( EcsWorld2& _world ) : m_world( _world )
		{
		}

		EcsWorld2& m_world;
		std::vector<Archetype*> m_archetypes;


		//================================
		//================================
		template < typename _ComponentType >
		struct Iterator
		{
			Iterator( const ComponentIndex2 _componentIndex, SystemView& _view )
			{
				m_archetypes = _view.m_archetypes;
				m_componentIndex = _componentIndex;
				m_archetypeIndex = 0;
				m_chunkIndex = 0;
				m_elementIndex = 0;
				m_currentArchetype = m_archetypes[m_archetypeIndex];
				m_currentChunk = &m_currentArchetype->m_chunks[_componentIndex].GetChunk( m_chunkIndex );
			}

			std::vector< Archetype* >	m_archetypes;
			ComponentIndex2				m_componentIndex;
			uint16_t					m_archetypeIndex;
			uint16_t					m_chunkIndex;
			uint16_t					m_elementIndex;
			Archetype*					m_currentArchetype;
			Chunk*						m_currentChunk;			

			void operator++() // prefix ++
			{
				++m_elementIndex;
				if( m_elementIndex >= m_currentChunk->Size() )
				{
					m_elementIndex = 0;
					++m_chunkIndex;

					if( m_chunkIndex < m_currentArchetype->m_chunks[m_componentIndex].NumChunk() )
					{
						m_currentChunk = &m_currentArchetype->m_chunks[m_componentIndex].GetChunk( m_chunkIndex );
					}
					else
					{
						m_chunkIndex = 0;
						++m_archetypeIndex;
						if( m_archetypeIndex < m_archetypes.size() )
						{
							m_currentArchetype = m_archetypes[m_archetypeIndex];
							m_currentChunk = &m_currentArchetype->m_chunks[m_componentIndex].GetChunk( m_chunkIndex );
						}
					}
				}
			}

			bool End()
			{
				return m_archetypeIndex >= m_archetypes.size();
			}

			inline _ComponentType& operator*()
			{
				return *static_cast<_ComponentType*>( m_currentChunk->At( m_elementIndex ) );
			}
		}; static constexpr size_t itSize = sizeof( Iterator<Position2> );

		template < typename _ComponentType >
		Iterator<_ComponentType> Begin()
		{
			const ComponentIndex2 index = m_world.m_typeToIndex[_ComponentType::Info::s_type];
			return Iterator<_ComponentType>( index, *this );
		}

		template < typename _ComponentType >
		void Kill( const Iterator<_ComponentType> _iterator )
		{
			const uint32_t index = _iterator.m_chunkIndex * _iterator.m_currentChunk->Capacity() + _iterator.m_elementIndex;
			m_world.Kill( *_iterator.m_currentArchetype, index );
		}
	};
}