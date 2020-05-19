#include <bitset>
#include <unordered_map>
#include <memory>
#include <cassert>
#include "core/fanHash.hpp"

namespace fan
{
	//================================
	//================================
#define DECLARE_COMPONENT2( _ComponentType)									\
	public:																	\
	template <class T> struct ComponentInfoImpl	{							\
		static constexpr uint32_t	 s_size		{ sizeof( T )			  };\
		static constexpr uint32_t	 s_alignment{ alignof( T )			  };\
		static constexpr const char* s_name		{ #_ComponentType		  };\
		static constexpr uint32_t	 s_type		{ SSID( #_ComponentType ) };\
	};																		\
	using Info = ComponentInfoImpl< _ComponentType >;


	//================================
	//================================
#define DECLARE_TAG2( _TagType )										\
	public:																\
		static constexpr const char* s_name		{ #_TagType		  };	\
		static constexpr uint32_t	 s_type		{ SSID( #_TagType ) };																		

	//================================
	//================================
	static constexpr uint32_t ecsSignatureLength = 8;
	using EcsSignature = std::bitset<ecsSignatureLength>;
	using EcsHandle = uint32_t;
	static constexpr size_t ecsChunkMaxSize = 65536;

	//================================
	//================================
	struct EcsComponent {};
	struct EcsTag {};

	//================================
	//================================
	struct TagBoundsOutdated : EcsTag { DECLARE_TAG2 ( TagBoundsOutdated ) };

	//================================
	//================================
	struct Position2 : EcsComponent
	{
		DECLARE_COMPONENT2( Position2 );

		float position[2];
	};

	//================================
	//================================
	struct Speed2 : EcsComponent
	{
		DECLARE_COMPONENT2( Speed2 );
		float speed[2];
	};

	//================================
	//================================
	struct Expiration2 : EcsComponent
	{
		DECLARE_COMPONENT2( Expiration2 );
		float timeLeft;
	};

	//================================
	//================================
	struct EcsComponentInfo
	{
		uint32_t	size;
		uint32_t	alignment;
		std::string name;
		uint32_t	type;
	};

	//================================
	// Small aligned memory buffer
	//================================
	class EcsChunk
	{
	public:
		void Create( const int _componentSize, const int _alignment )
		{
			m_componentSize = _componentSize;
			m_capacity =  ecsChunkMaxSize / _componentSize;
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
	class EcsChunkVector
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

			EcsChunk& chunk = m_chunks[chunkIndex];

			// Last chunk ? just remove the element & back swap locally in the chunk
			if( chunkIndex == m_chunks.size() - 1 )
			{
				chunk.Remove( elementIndex );
			}
			else
			{
				// back swap the removed element with the last element of the last chunk
				EcsChunk& lastChunk = *m_chunks.rbegin();
				chunk.Set( elementIndex, lastChunk.At( lastChunk.Size() - 1 ) );
				lastChunk.PopBack();
			}

			EcsChunk& lastChunk = *m_chunks.rbegin();
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
				m_chunks.push_back( EcsChunk() );
				m_chunks.rbegin()->Create( m_componentSize, m_alignment );
			}

			// Push to the last chunk
			EcsChunk& chunk = *m_chunks.rbegin();
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
				m_chunks.push_back( EcsChunk() );
				m_chunks.rbegin()->Create( m_componentSize, m_alignment );
			}

			// Emplace to the last chunk
			EcsChunk& chunk = *m_chunks.rbegin();
			const int chunkIndex = int( m_chunks.size() - 1 );
			const int elementIndex = chunk.Size();
			chunk.EmplaceBack();
			return chunkIndex * m_chunkCapacity + elementIndex;
		}
		
		EcsChunk&	GetChunk( const int _index ) { return m_chunks[_index]; }
		int			NumChunk() const { return int( m_chunks.size() ); }
	private:
		std::vector<EcsChunk> m_chunks;
		int m_componentSize;
		int m_alignment;
		int m_chunkCapacity;
	};

	class EcsArchetype;

	//================================
	// Allows indexing of entities
	//================================
	struct EcsEntity
	{
		EcsArchetype* archetype = nullptr;
		uint32_t	index = 0;	// index in the archetype
	};

	//================================
	// Small data relative to a specific entity
	//================================
	struct EcsEntityData
	{
		int	 transitionIndex = -1;	// index of the corresponding transition if it exists
		EcsHandle handle = 0;
	};

	//================================
	// A transition is created when structural change happen on an entity ( creating / removing components )
	//================================
	struct EcsTransition
	{
		EcsEntity	entityID;
		EcsSignature  signatureAdd = EcsSignature( 0 );		// bit to 1 means add component
		EcsSignature  signatureRemove = EcsSignature( 0 );	// bit to 1 means remove component
		bool isDead = false;
	};

	//================================
	// An archetype is a collection of entities with the same signature
	// All component of the same entity are at the same index
	// components are put in arrays of small buffers called chunks
	//================================
	class EcsArchetype
	{
	public:
		void Create( const std::vector< EcsComponentInfo >& _componentsInfo, const EcsSignature& _signature )
		{
			const size_t numComponents = _componentsInfo.size();

			m_signature = _signature;
			m_chunks.resize( numComponents );

			for( int i = 0; i < numComponents; i++ )
			{
				if( m_signature[i] )
				{
					const EcsComponentInfo& info = _componentsInfo[i];
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

		EcsSignature					m_signature;
		std::vector< EcsChunkVector >	m_chunks;		// one index per component type
		std::vector<EcsEntityData>		m_entities;	
	};

	//================================
	//================================
	struct EcsWorld2
	{
		EcsArchetype m_transitionArchetype;
		std::unordered_map< EcsSignature, EcsArchetype* >	m_archetypes;
		std::vector< EcsComponentInfo >						m_componentsInfo;
		std::unordered_map<uint32_t, int >					m_typeToIndex;
		std::unordered_map< EcsHandle, EcsEntity >			m_handles;
		EcsHandle											m_nextHandle = 1; // 0 is a null handle
		std::vector< EcsTransition >						m_transitions;
		int													m_nextTagIndex = ecsSignatureLength - 1;
		EcsSignature										m_tagsMask; // signature of all tag types combined

		void Create()
		{
			m_transitionArchetype.Create( m_componentsInfo, ~EcsSignature( 0 ) );
		}

		int NumComponents() const 
		{
			return int(m_componentsInfo.size());
		}

		void ApplyTransitions()
		{
			// Applies structural transition to entities ( add/remove components/tags, add/remove entities
			assert( m_transitionArchetype.Size() == m_transitions.size() );
			for (int transitionIndex = m_transitionArchetype.Size() - 1; transitionIndex >= 0; --transitionIndex)
			{
				const EcsTransition& transition = m_transitions[transitionIndex];
				EcsArchetype& srcArchetype = *transition.entityID.archetype;
				const uint32_t srcIndex = transition.entityID.index;

				EcsEntityData srcEntityCpy = srcArchetype.m_entities[srcIndex];
				assert( srcEntityCpy.transitionIndex == transitionIndex );
				srcEntityCpy.transitionIndex = -1;

				// Create new signature
				EcsSignature targetSignature = EcsSignature( 0 );
				if( &srcArchetype != &m_transitionArchetype )
				{
					targetSignature |= srcArchetype.m_signature;
				}
				targetSignature |= transition.signatureAdd;
				targetSignature &= ~transition.signatureRemove;

				// Entity is dead
				if( transition.isDead || targetSignature == EcsSignature( 0 ) )
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
							EcsEntityData movedEntity = srcArchetype.m_entities[srcIndex];
							if( movedEntity.handle != 0 )
							{
								m_handles[movedEntity.handle].index = srcIndex;
							}
						}
					}

					// clears the transition archetype
					const EcsSignature transitionMoveSignature = transition.signatureAdd & ~transition.signatureRemove;
					for( int i = 0; i < NumComponents(); i++ )
					{
						m_transitionArchetype.m_chunks[i].Remove( transitionIndex );						
					}
					m_transitionArchetype.RemoveEntity( transitionIndex );
				}
				else
				{
					// Get new archetype
					EcsArchetype* dstArchetype = FindArchetype( targetSignature );
					if( dstArchetype == nullptr )
					{
						dstArchetype = &CreateArchetype( targetSignature );
					}

					// Push new entity
					const uint32_t dstIndex = dstArchetype->Size();
					dstArchetype->m_entities.push_back( srcArchetype.m_entities[srcIndex] );
					EcsEntityData& dstEntity = * dstArchetype->m_entities.rbegin();
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
							EcsEntityData swappedEntity = srcArchetype.m_entities[srcIndex];
							if( swappedEntity.handle != 0 )
							{
								m_handles[swappedEntity.handle].index = srcIndex;
							}
						}
					}

					// copy all components from the transition archetype to the new archetype
					const EcsSignature transitionMoveSignature = transition.signatureAdd & targetSignature;
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

		EcsTransition& FindOrCreateTransition( const EcsEntity _entityID )
		{
			// Get/register transition
			EcsEntityData& entity = _entityID.archetype->m_entities[_entityID.index];
			EcsTransition* transition = nullptr;
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

		EcsEntity GetEntityID( const EcsHandle _handle )
		{
			return m_handles[ _handle ];
		}

		EntityHandle AddHandle( const EcsEntity _entityID )
		{
			EcsEntityData& entity = _entityID.archetype->m_entities[_entityID.index];
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

		void RemoveHandle( const EcsEntity _entityID )
		{
			EcsEntityData& entity = _entityID.archetype->m_entities[_entityID.index];
			if( entity.handle != 0 )
			{
				m_handles.erase( entity.handle );
				entity.handle = 0;
			}
		}

		EcsArchetype* FindArchetype( const EcsSignature _signature )
		{
			auto it = m_archetypes.find( _signature );
			return it == m_archetypes.end() ? nullptr : it->second;
		}

		EcsArchetype& CreateArchetype( const EcsSignature _signature )
		{
			assert( FindArchetype( _signature ) == nullptr );
			EcsArchetype* newArchetype = new EcsArchetype();
			newArchetype->Create( m_componentsInfo, _signature );
			m_archetypes[_signature] = newArchetype;
			return *newArchetype;
		}

		EcsEntityData& GetEntity( const EcsEntity _entityID )
		{
			return _entityID.archetype->m_entities[_entityID.index];
		}

		void Kill( const EcsEntity _entityID )
		{
			EcsTransition& transition = FindOrCreateTransition( _entityID );
			transition.isDead = true;
		}

		template <typename _TagType > 
		void AddTagType()
		{
			static_assert( std::is_base_of< EcsTag, _TagType>::value );
 			assert( m_nextTagIndex >= NumComponents() );
 			const ComponentIndex newTagIndex = m_nextTagIndex--;
			m_typeToIndex[_TagType::s_type] = newTagIndex;
 			m_tagsMask[newTagIndex] = 1;
		}

		void AddTag( const EcsEntity _entityID, const uint32_t s_type )
		{
			const uint32_t tagIndex = m_typeToIndex[s_type];
			EcsTransition& transition = FindOrCreateTransition( _entityID );

			// entity doesn't already have this tag
			assert( _entityID.archetype == &m_transitionArchetype || !_entityID.archetype->m_signature[tagIndex] );

			// Update transition
			transition.signatureAdd[tagIndex] = 1;
		}

		void RemoveTag( const EcsEntity _entityID, const uint32_t s_type )
		{
			const uint32_t tagIndex = m_typeToIndex[s_type];
			EcsTransition& transition = FindOrCreateTransition( _entityID );

			// entity must have this tag
			assert( _entityID.archetype == &m_transitionArchetype || _entityID.archetype->m_signature[tagIndex] );

			// Update transition
			transition.signatureRemove[tagIndex] = 1;
		}

		template <typename _ComponentType >
		void AddComponentType()
		{
			const int index = NumComponents();

			EcsComponentInfo info;
			info.size = _ComponentType::Info::s_size;
			info.alignment = _ComponentType::Info::s_alignment;
			info.name = _ComponentType::Info::s_name;
			info.type = _ComponentType::Info::s_type;
			m_componentsInfo.push_back( info );

			m_typeToIndex[_ComponentType::Info::s_type] = index;
		}

		EcsComponent& AddComponent( const EcsEntity _entityID, const uint32_t s_type )
		{
			const uint32_t componentIndex = m_typeToIndex[s_type];
			EcsTransition& transition = FindOrCreateTransition( _entityID );
			EcsEntityData& entity = GetEntity( _entityID );

			// entity doesn't already have this component
			assert( _entityID.archetype == &m_transitionArchetype || !_entityID.archetype->m_signature[componentIndex] );

			// Update transition
			transition.signatureAdd[componentIndex] = 1;

			return *static_cast<EcsComponent*>( m_transitionArchetype.m_chunks[componentIndex].At( entity.transitionIndex ) );
		}

		void RemoveComponent( const EcsEntity _entityID, const uint32_t s_type )
		{
			const uint32_t componentIndex = m_typeToIndex[s_type];
			EcsTransition& transition = FindOrCreateTransition( _entityID );

			// entity must have this component
			assert( _entityID.archetype == &m_transitionArchetype || _entityID.archetype->m_signature[componentIndex] );

			// Update transition
			transition.signatureRemove[componentIndex] = 1;
		}

		EcsEntity CreateEntity()
		{			
			EcsEntity entityID; 
			entityID.archetype = &m_transitionArchetype;
			entityID.index = m_transitionArchetype.Size();

			EcsEntityData entity;
			entity.transitionIndex = int(m_transitions.size());

			EcsTransition transition;
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
	struct EcsSystemView
	{
		EcsSystemView( EcsWorld2& _world ) : m_world( _world )
		{
		}

		EcsWorld2& m_world;
		std::vector<EcsArchetype*> m_archetypes;


		//================================
		//================================
		template < typename _ComponentType >
		struct Iterator
		{
			Iterator( const int _componentIndex, EcsSystemView& _view )
			{
				m_archetypes = _view.m_archetypes;
				m_componentIndex = _componentIndex;
				m_archetypeIndex = 0;
				m_chunkIndex = 0;
				m_elementIndex = 0;
				m_currentArchetype = m_archetypes[m_archetypeIndex];
				m_currentChunk = &m_currentArchetype->m_chunks[_componentIndex].GetChunk( m_chunkIndex );
			}

			std::vector< EcsArchetype* >	m_archetypes;
			int							m_componentIndex;
			uint16_t					m_archetypeIndex;
			uint16_t					m_chunkIndex;
			uint16_t					m_elementIndex;
			EcsArchetype*					m_currentArchetype;
			EcsChunk*						m_currentChunk;			

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
			const int index = m_world.m_typeToIndex[_ComponentType::Info::s_type];
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