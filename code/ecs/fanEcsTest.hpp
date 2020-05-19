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
	using EntityHandle2 = uint32_t;
	using ComponentIndex2 = uint16_t;
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
	// Small aligned memory buffer
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
	// Dynamic array of fixed size chunks
	// Can be indexed like a continuous array	
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

	class Archetype;

	//================================
	// Allows indexing of entities
	//================================
	struct EntityID2
	{
		Archetype* archetype = nullptr;
		uint32_t	index = 0;	// index in the archetype
	};

	//================================
	// Small data relative to a specific entity
	//================================
	struct Entity2
	{
		int	 transitionIndex = -1;	// index of the corresponding transition if it exists
		EntityHandle2 handle = 0;
	};

	//================================
	// A transition is created when structural change happen on an entity ( creating / removing components )
	//================================
	struct Transition
	{
		EntityID2	entityID;
		Signature2  signatureAdd = Signature2( 0 );		// bit to 1 means add componenent
		Signature2  signatureRemove = Signature2( 0 );	// bit to 1 means remove componenent
		bool isDead = false;
	};

	static const size_t trrtrtrt = sizeof( Entity2 );

	//================================
	// An archetype is a collection of entities with the same signature
	// All component of the same entity are at the same index
	// components are put in arrays of small buffers called chunks
	//================================
	class Archetype
	{
	public:
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

		// Returns true if the last entity was swapped with the removed one
		bool RemoveEntity( const int _entityIndex )
		{
			// pop the last element
			if( _entityIndex == Size() - 1 )
			{
				m_entities.pop_back();
				return false;
			}
			
			// else swap with the last
			m_entities[_entityIndex] = *m_entities.rbegin();
			m_entities.pop_back();
			return true;
		}

		int		Size() const  { return int(m_entities.size());  }
		bool	Empty() const { return m_entities.empty(); }

		Signature2					m_signature;
		std::vector< ChunkVector >	m_chunks;		// one index per component type
		std::vector<Entity2>		m_entities;	
	};

	//================================
	//================================
	struct EcsWorld2
	{
		Archetype m_transitionArchetype;
		std::unordered_map< Signature2, Archetype* >	m_archetypes;
		std::vector< ComponentInfo2 >					m_componentsInfo;
		std::unordered_map<uint32_t, ComponentIndex2>	m_typeToIndex;
		std::unordered_map< EntityHandle2, EntityID2 >	m_handles;
		EntityHandle2									m_nextHandle = 1; // 0 is a null handle
		std::vector< Transition >						m_transitions;

		void Create()
		{
			m_transitionArchetype.Create( m_componentsInfo, ~Signature2( 0 ) );
		}

		int NumComponents() const 
		{
			return int(m_componentsInfo.size());
		}

		void EndFrame()
		{
			// Applies structural transition to entities
			assert( m_transitionArchetype.Size() == m_transitions.size() );
			for (int transitionIndex = m_transitionArchetype.Size() - 1; transitionIndex >= 0; --transitionIndex)
			{
				const Transition& transition = m_transitions[transitionIndex];
				Archetype& srcArchetype = *transition.entityID.archetype;
				const uint32_t srcIndex = transition.entityID.index;

				Entity2 srcEntityCpy = srcArchetype.m_entities[srcIndex];
				assert( srcEntityCpy.transitionIndex == transitionIndex );
				srcEntityCpy.transitionIndex = -1;

				// Create new signature
				Signature2 targetSignature = Signature2( 0 );
				if( &srcArchetype != &m_transitionArchetype )
				{
					targetSignature |= srcArchetype.m_signature;
				}
				targetSignature |= transition.signatureAdd;
				targetSignature &= ~transition.signatureRemove;

				// Entity is dead
				if( transition.isDead || targetSignature == Signature2( 0 ) )
				{
					// remove handle
					if( srcEntityCpy.handle != 0 )
					{
						m_handles.erase( srcEntityCpy.handle );
					}

					// remove all components from the src archetype
					if( &srcArchetype != &m_transitionArchetype )
					{
						for( int componentIndex = 0; componentIndex < NumComponents(); componentIndex++ )
						{
							if( srcArchetype.m_signature[componentIndex] )
							{
								srcArchetype.m_chunks[componentIndex].Remove( srcIndex );
							}
						}
						if( srcArchetype.RemoveEntity( srcIndex ) )
						{
							// update the handle of the element if it was moved
							Entity2 movedEntity = srcArchetype.m_entities[srcIndex];
							if( movedEntity.handle != 0 )
							{
								m_handles[movedEntity.handle].index = srcIndex;
							}
						}
					}

					// clears the transition archetype
					const Signature2 transitionMoveSignature = transition.signatureAdd & ~transition.signatureRemove;
					for( int i = 0; i < NumComponents(); i++ )
					{
						m_transitionArchetype.m_chunks[i].Remove( transitionIndex );						
					}
					m_transitionArchetype.RemoveEntity( transitionIndex );
				}
				else
				{
					// Get new archetype
					Archetype* dstArchetype = FindArchetype( targetSignature );
					if( dstArchetype == nullptr )
					{
						dstArchetype = &CreateArchetype( targetSignature );
					}

					// Push new entity
					const uint32_t dstIndex = dstArchetype->Size();
					dstArchetype->m_entities.push_back( srcArchetype.m_entities[srcIndex] );
					Entity2& dstEntity = * dstArchetype->m_entities.rbegin();
					dstEntity = srcEntityCpy;
					dstEntity.transitionIndex = -1;
					if( dstEntity.handle != 0 )
					{
						m_handles[dstEntity.handle] = { dstArchetype, dstIndex };
					}

					// Moves components from the source archetype to the new archetype				
					if( &srcArchetype != &m_transitionArchetype )
					{
						for( int i = 0; i < NumComponents(); i++ )
						{
							if( srcArchetype.m_signature[i] )
							{
								if( targetSignature[i] )
								{
									dstArchetype->m_chunks[i].PushBack( srcArchetype.m_chunks[i].At( srcIndex ) );
								}
								srcArchetype.m_chunks[i].Remove( srcIndex );
							}
						}
						if( srcArchetype.RemoveEntity( srcIndex ) )
						{
							Entity2 swappedEntity = srcArchetype.m_entities[srcIndex];
							if( swappedEntity.handle != 0 )
							{
								m_handles[swappedEntity.handle].index = srcIndex;
							}
						}
					}

					// copy all components from the transition archetype to the new archetype
					const Signature2 transitionMoveSignature = transition.signatureAdd & targetSignature;
					for( int i = 0; i < NumComponents(); i++ )
					{
						if( transitionMoveSignature[i] )
						{
							dstArchetype->m_chunks[i].PushBack( m_transitionArchetype.m_chunks[i].At( transitionIndex ) );
						}
						m_transitionArchetype.m_chunks[i].Remove( transitionIndex );
					}
					m_transitionArchetype.RemoveEntity( transitionIndex );
				}
			}
			m_transitions.clear();
		}

		void Kill( const EntityID2 _entityID )
		{
			Transition& transition = FindOrCreateTransition( _entityID );
			transition.isDead = true;
		}

		EntityID2 GetEntityID( const EntityHandle2 _handle )
		{
			return m_handles[ _handle ];
		}

		EntityHandle CreateHandle( const EntityID2 _entityID )
		{
			Entity2& entity = _entityID.archetype->m_entities[_entityID.index];
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

		void RemoveHandle( const EntityID2 _entityID )
		{
			Entity2& entity = _entityID.archetype->m_entities[_entityID.index];
			if( entity.handle != 0 )
			{
				m_handles.erase( entity.handle );
				entity.handle = 0;
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

		Transition& FindOrCreateTransition( const EntityID2 _entityID )
		{
			// Get/register transition
			Entity2& entity = _entityID.archetype->m_entities[_entityID.index];
			Transition* transition = nullptr;
			if( entity.transitionIndex < 0 )
			{
				entity.transitionIndex = m_transitionArchetype.Size();
				m_transitions.emplace_back();
				transition = &( *m_transitions.rbegin() );
				transition->entityID = _entityID;

				for( int i = 0; i < NumComponents(); i++ )
				{
					m_transitionArchetype.m_chunks[i].EmplaceBack();
				}
				m_transitionArchetype.m_entities.emplace_back();
			}
			else
			{
				// transition already exists
				transition = &m_transitions[entity.transitionIndex];
			}
			return *transition;
		}

		Entity2& GetEntity( const EntityID2 _entityID )
		{
			return _entityID.archetype->m_entities[_entityID.index];
		}
		void RemoveComponent( const EntityID2 _entityID, const uint32_t s_type )
		{
			const uint32_t componentIndex = m_typeToIndex[s_type];
			Transition& transition = FindOrCreateTransition( _entityID );

			// entity must have this component
			assert( _entityID.archetype == &m_transitionArchetype || _entityID.archetype->m_signature[componentIndex] );

			// Update transition
			transition.signatureRemove |= Signature2( 1 ) << componentIndex;
		}

		Component2& AddComponent( const EntityID2 _entityID, const uint32_t s_type )
		{			
			const uint32_t componentIndex = m_typeToIndex[s_type];
			Transition& transition = FindOrCreateTransition( _entityID );
			Entity2& entity = GetEntity( _entityID ); 			

			// entity doesn't already have this component
			assert( _entityID.archetype == &m_transitionArchetype || !_entityID.archetype->m_signature[componentIndex] );

			// Update transition
			transition.signatureAdd |=  Signature2( 1 ) << componentIndex;

			return *static_cast<Component2*>( m_transitionArchetype.m_chunks[componentIndex].At( entity.transitionIndex ) );
		}

		EntityID2 CreateEntity()
		{			
			EntityID2 entityID; 
			entityID.archetype = &m_transitionArchetype;
			entityID.index = m_transitionArchetype.Size();

			Entity2 entity;
			entity.transitionIndex = int(m_transitions.size());

			Transition transition;
			transition.entityID = entityID;

			m_transitions.push_back( transition );
			m_transitionArchetype.m_entities.push_back( entity );
			for( int i = 0; i < NumComponents(); i++ )
			{
				m_transitionArchetype.m_chunks[i].EmplaceBack();
			}			

			return entityID;
		}
	};

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
			m_world.Kill( { &( *_iterator.m_currentArchetype ), index } );
		}
	};
}