#include <bitset>
#include <unordered_map>
#include <memory>
#include <cassert>
#include "core/fanHash.hpp"

namespace fan
{
	//================================
	//================================
#define DECLARE_COMPONENT2( _ComponentType)															\
	public:																							\
	template <class T> struct ComponentInfoImpl	{													\
		static constexpr uint32_t	 s_size		{ sizeof( T )			  };						\
		static constexpr uint32_t	 s_alignment{ alignof( T )			  };						\
		static constexpr const char* s_name		{ #_ComponentType		  };						\
		static constexpr uint32_t	 s_type		{ SSID( #_ComponentType ) };						\
	};																								\
	using Info = ComponentInfoImpl< _ComponentType >;												\

	//================================
	//================================
#define DECLARE_TAG2( _TagType )										\
	public:																\
		static constexpr const char* s_name		{ #_TagType		  };	\
		static constexpr uint32_t	 s_type		{ SSID( #_TagType ) };																		

	//================================
	//================================
#define DECLARE_SINGLETON2( _SingletonType )									\
	public:																		\
		static constexpr const char* s_name		{ #_SingletonType		  };	\
		static constexpr uint32_t	 s_type		{ SSID( #_SingletonType ) };	

	//================================
	//================================
	static constexpr uint32_t ecsSignatureLength = 8;
	using EcsSignature = std::bitset<ecsSignatureLength>;
	using EcsHandle = uint32_t;
	static constexpr size_t ecsChunkMaxSize = 65536;

	//================================
	//================================
	struct EcsSingleton {};
	struct EcsComponent {};
	struct EcsTag {};

	class EcsWorld2;
	class EcsArchetype;

	//================================
	// Allows indexing of individual entities
	//================================
	struct EcsEntity
	{
		EcsArchetype* archetype = nullptr;
		uint32_t	index = 0;	// index in the archetype
	};

	//================================
	//================================
	struct EcsComponentInfo
	{
		std::string name;
		ImGui::IconType icon = ImGui::IconType::NONE;	// editor icon
		uint32_t	type;
		int			index;
		uint32_t	size;
		uint32_t	alignment;
		const char* editorPath = "";					// editor path ( for the addComponent ) popup of the inspector

		void ( *init )( EcsWorld2&, EcsComponent& ) = nullptr;						// (mandatory) called at the creation of the component
		void ( *destroy )( EcsWorld2&, EcsComponent& ) = nullptr;					// called at the destruction of the component
		void ( *onGui )( EcsWorld2&, EcsEntity, EcsComponent& ) = nullptr;			// called by the editor for gui display
		void ( *save )( const EcsComponent&, Json& ) = nullptr;						// called when the scene is saved
		void ( *load )( EcsComponent&, const Json& ) = nullptr;						// called when the scene is loaded ( after the init )
		void ( *netSave ) ( const EcsComponent&, sf::Packet& _packet ) = nullptr;
		void ( *netLoad ) ( EcsComponent&, sf::Packet& _packet ) = nullptr;
	};

	//==============================================================================================================================================================
	// EcsSingletonInfo is runtime type information for singleton components
	// Function pointers :
	// init			: clears the component value and registers it when necessary (mandatory)
	// onGui		: draws ui associated with the component (optional)
	// save			: serializes the component to json (optional)
	// load			: deserializes the component from json (optional)
	// netSave		: serializes the component into a packet for replication (optional)
	// netLoad		: deserializes the component from a packet for replication (optional)
	//==============================================================================================================================================================
	struct EcsSingletonInfo
	{
		std::string		name;
		uint32_t		type;
		ImGui::IconType icon = ImGui::IconType::NONE;
		void ( *init ) ( EcsWorld2&, EcsSingleton& ) = nullptr;
		void ( *onGui ) ( EcsWorld2&, EcsSingleton& ) = nullptr;
		void ( *save ) ( const EcsSingleton&, Json& ) = nullptr;
		void ( *load ) ( EcsSingleton&, const Json& ) = nullptr;
		void ( *netSave ) ( const EcsSingleton&, sf::Packet& _packet ) = nullptr;
		void ( *netLoad ) ( EcsSingleton&, sf::Packet& _packet ) = nullptr;
	};

	//================================
	//================================
	struct TagBoundsOutdated : EcsTag { DECLARE_TAG2 ( TagBoundsOutdated ) };

	//================================
	//================================
	struct SingletonTest : EcsSingleton
	{
		DECLARE_SINGLETON2( SingletonTest )
		static void SetInfo( EcsSingletonInfo& /*_info*/ ){}
		static void Init( EcsWorld2& /*_world*/, EcsSingleton& /*_singleton*/ ) {}

		float number;
	};

	//================================
	//================================
	struct Position2 : EcsComponent
	{
		DECLARE_COMPONENT2( Position2 );
		static void SetInfo( EcsComponentInfo& /*_info*/ ){}
		static void Init( EcsWorld2& /*_world*/, EcsComponent& /*_component*/ ) {}

		float position[2];
	};

	//================================
	//================================
	struct Speed2 : EcsComponent
	{
		DECLARE_COMPONENT2( Speed2 );
		static void SetInfo( EcsComponentInfo& /*_info*/ ) {}
		static void Init( EcsWorld2& /*_world*/, EcsComponent& /*_component*/ ) {}

		float speed[2];
	};

	//================================
	//================================
	struct Expiration2 : EcsComponent
	{
		DECLARE_COMPONENT2( Expiration2 );
		static void SetInfo( EcsComponentInfo& /*_info*/ ) {}
		static void Init( EcsWorld2& /*_world*/, EcsComponent& /*_component*/ ) {}

		float timeLeft;
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

		void Clear()
		{
			m_size = 0;
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
		
		void Clear()
		{
			for (int i = NumChunk() - 1; i > 0 ; i--)
			{
				m_chunks[i].Destroy();
				m_chunks.pop_back();
			}			
			m_chunks[0].Clear();
			assert( m_chunks.size() == 1 );
			assert( m_chunks[0].Empty() );
		}
		
		const EcsChunk& GetChunk( const int _index ) const  { return m_chunks[_index];		 }
		EcsChunk&		GetChunk( const int _index )		{ return m_chunks[_index];		 }
		int				NumChunk() const					{ return int( m_chunks.size() ); }
	private:
		std::vector<EcsChunk> m_chunks;
		int m_componentSize;
		int m_alignment;
		int m_chunkCapacity;
	};

	//================================
	// Data relative to a specific entity
	//================================
	struct EcsEntityData
	{
		int	 transitionIndex = -1;	// index of the corresponding transition if it exists
		EcsHandle handle = 0;
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

		void Clear()
		{			
			for ( EcsChunkVector& chunkVector : m_chunks )
			{
				chunkVector.Clear();
			}
			m_entities.clear();
		}

		int		Size() const  { return int(m_entities.size());  }
		bool	Empty() const { return m_entities.empty(); }

		EcsSignature					m_signature;
		std::vector< EcsChunkVector >	m_chunks;		// one index per component type
		std::vector<EcsEntityData>		m_entities;	
	};

	//================================
	// A transition is created when structural change happen on an entity ( creating / removing components, changing archetype etc )
	//================================
	struct EcsTransition
	{
		EcsEntity	entityID;
		EcsSignature  signatureAdd = EcsSignature( 0 );		// bit to 1 means add component
		EcsSignature  signatureRemove = EcsSignature( 0 );	// bit to 1 means remove component
		bool isDead = false;
	};

	//================================	
	//================================
	class EcsWorld2
	{
	public:
		// Global
		void Create()
		{
			m_transitionArchetype.Create( m_componentsInfo, ~EcsSignature( 0 ) );
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
		void Clear()
		{
			ApplyTransitions();
			// Remove all entities
			for( auto it = m_archetypes.begin(); it != m_archetypes.end(); ++it )
			{
				it->second->Clear();
			}
			m_transitionArchetype.Clear();

			m_handles.clear();
			m_nextHandle = 1;

			// clears singleton components
			for( std::pair< uint32_t, EcsSingleton*> pair : m_singletons )
			{
				const EcsSingletonInfo& info = GetSingletonInfo( pair.first );
				info.init( *this, *pair.second );
			}

		}
		int  NumComponents() const
		{
			return int( m_componentsInfo.size() );
		}
		int  GetIndex( const uint32_t  _type ) const { return m_typeToIndex.at( _type ); }

		// Handles
		EcsEntity	 GetEntity( const EcsHandle _handle )
		{
			return m_handles[ _handle ];
		}
		EntityHandle AddHandle( const EcsEntity _entity )
		{
			EcsEntityData& entity = _entity.archetype->m_entities[_entity.index];
			if( entity.handle != 0 )
			{
				return entity.handle;
			}
			else
			{
				entity.handle = m_nextHandle++;
				m_handles[entity.handle] = _entity;
				return entity.handle;
			}
		}
		void		 RemoveHandle( const EcsEntity _entity )
		{
			EcsEntityData& entity = _entity.archetype->m_entities[_entity.index];
			if( entity.handle != 0 )
			{
				m_handles.erase( entity.handle );
				entity.handle = 0;
			}
		}

		// Singletons
		template <typename _SingletonType >	void			AddSingletonType()
		{
			static_assert( std::is_base_of< EcsSingleton, _SingletonType>::value );
			assert( m_singletons.find( _SingletonType::s_type ) == m_singletons.end() );

			// Creates the singleton component
			_SingletonType* singleton = new _SingletonType();
			m_singletons[_SingletonType::s_type] = singleton;

			// Registers singleton info
			EcsSingletonInfo info;
			_SingletonType::SetInfo( info );
			info.init = &_SingletonType::Init;			
			info.type = _SingletonType::s_type;
			m_singletonInfos[_SingletonType::s_type] = info;

			// init singleton
			info.init( *this, *singleton );
		}
		template< typename _SingletonType > _SingletonType& GetSingleton()
		{ 
			static_assert( std::is_base_of< EcsSingleton, _SingletonType >::value );
			return static_cast<_SingletonType&>(  GetSingleton( _SingletonType::s_type  ) );
		}
		EcsSingleton&			GetSingleton( const uint32_t _type ) { return  *m_singletons[_type]; }
		const EcsSingletonInfo& GetSingletonInfo( const uint32_t _type ) const { return  m_singletonInfos.at( _type ); }
		const EcsSingletonInfo* SafeGetSingletonInfo( const uint32_t _type ) const
		{
			const auto& it = m_singletonInfos.find( _type );
			return it == m_singletonInfos.end() ? nullptr : &it->second;
		}

		// Tags
		template <typename _TagType > void AddTagType()
		{
			static_assert( std::is_base_of< EcsTag, _TagType>::value );
 			assert( m_nextTagIndex >= NumComponents() );
 			const ComponentIndex newTagIndex = m_nextTagIndex--;
			m_typeToIndex[_TagType::s_type] = newTagIndex;

 			m_tagsMask[newTagIndex] = 1;
		}
		template <typename _TagType > void AddTag( const EcsEntity _entity ) 
		{ 
			static_assert( std::is_base_of< EcsTag, _TagType>::value ); 
			AddTag( _TagType::s_type );
		}
		template <typename _TagType > void RemoveTag( const EcsEntity _entity )
		{
			static_assert( std::is_base_of< EcsTag, _TagType>::value ); 
			RemoveTag( _TagType::s_type );
		}
		void AddTag( const EcsEntity _entity, const uint32_t _type )
		{
			const int tagIndex = GetIndex(_type);
			EcsTransition& transition = FindOrCreateTransition( _entity );

			// entity doesn't already have this tag
			assert( _entity.archetype == &m_transitionArchetype || !_entity.archetype->m_signature[tagIndex] );

			// Update transition
			transition.signatureAdd[tagIndex] = 1;
		}
		void RemoveTag( const EcsEntity _entity, const uint32_t _type )
		{
			const int tagIndex = GetIndex( _type );
			EcsTransition& transition = FindOrCreateTransition( _entity );

			// entity must have this tag
			assert( _entity.archetype == &m_transitionArchetype || _entity.archetype->m_signature[tagIndex] );

			// Update transition
			transition.signatureRemove[tagIndex] = 1;
		}
		bool HasTag( const EcsEntity _entity, const uint32_t _type ) const 
		{ 
			const int tagIndex = GetIndex( _type );
			return _entity.archetype->m_signature[tagIndex];
		}
		void AddTagsFromSignature( const EcsEntity _entity, const EcsSignature& _signature )
		{
			EcsTransition& transition = FindOrCreateTransition( _entity );
			transition.signatureAdd |= _signature & m_tagsMask;
		}

		// Components
		template <typename _ComponentType >	void			AddComponentType()
		{
			static_assert( std::is_base_of< EcsComponent, _ComponentType>::value );
			
			const int nextTypeIndex = NumComponents();
			assert( m_nextTagIndex >= nextTypeIndex );

			// Set component info
			EcsComponentInfo info;
			info.name		= _ComponentType::Info::s_name;
			info.init		= &_ComponentType::Init;			
			info.size		= _ComponentType::Info::s_size;
			info.alignment	= _ComponentType::Info::s_alignment;			
			info.type		= _ComponentType::Info::s_type;
			info.index		= nextTypeIndex;
			_ComponentType::SetInfo( info );
			m_componentsInfo.push_back( info );

			m_typeToIndex[_ComponentType::Info::s_type] = nextTypeIndex;
		}
		template <typename _ComponentType > _ComponentType& AddComponent( const EcsEntity _entity )
		{
			static_assert( std::is_base_of< EcsComponent, _ComponentType>::value );
			return static_cast<_ComponentType&>( AddComponent( _ComponentType::s_type ) );
		}
		template <typename _ComponentType > void			RemoveComponent( const EcsEntity _entity )
		{
			static_assert( std::is_base_of< EcsComponent, _ComponentType>::value );
			RemoveComponent( _ComponentType::s_type );
		}
		template <typename _ComponentType > bool			HasComponent( const EcsEntity _entity )
		{
			static_assert( std::is_base_of< EcsComponent, _ComponentType>::value );
			return HasComponent( _entity, _ComponentType::Info::s_type );
		}
		EcsComponent& AddComponent( const EcsEntity _entity, const uint32_t _type )
		{
			const int componentIndex = GetIndex(_type);
			EcsTransition& transition = FindOrCreateTransition( _entity );
			EcsEntityData& entityData = GetEntityData( _entity );

			// entity doesn't already have this component
			assert( _entity.archetype == &m_transitionArchetype || !_entity.archetype->m_signature[componentIndex] );

			// Update transition
			transition.signatureAdd[componentIndex] = 1;

			return *static_cast<EcsComponent*>( m_transitionArchetype.m_chunks[componentIndex].At( entityData.transitionIndex ) );
		}
		void		  RemoveComponent( const EcsEntity _entity, const uint32_t s_type )
		{
			const int componentIndex = GetIndex(s_type);
			EcsTransition& transition = FindOrCreateTransition( _entity );

			// entity must have this component
			assert( _entity.archetype == &m_transitionArchetype || _entity.archetype->m_signature[componentIndex] );

			// Update transition
			transition.signatureRemove[componentIndex] = 1;
		}
		bool		  HasComponent( const EcsEntity _entity, const uint32_t _type )
		{			
			return _entity.archetype->m_signature[GetIndex( _type )];
		}

		// Entities
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
		void Kill( const EcsEntity _entity )
		{
			EcsTransition& transition = FindOrCreateTransition( _entity );
			transition.isDead = true;
		}
 		bool IsAlive( const EcsEntity _entity ) const 
		{ 
			const EcsEntityData& entityData = GetEntityData( _entity );
			return entityData.transitionIndex < 0 || ! m_transitions[entityData.transitionIndex].isDead;
		}

		// Const accessors
		const std::unordered_map< EcsHandle, EcsEntity >&		 GetHandles() const				{ return m_handles;				}
		const std::unordered_map< EcsSignature, EcsArchetype* >& GetArchetypes() const			{ return m_archetypes;			}
		const EcsArchetype&										 GetTransitionArchetype() const { return m_transitionArchetype; }

	private:
		const EcsEntityData& GetEntityData( const EcsEntity _entity ) const { return _entity.archetype->m_entities[_entity.index]; }
		EcsEntityData&	GetEntityData( const EcsEntity _entity )	{	return _entity.archetype->m_entities[_entity.index]; }
		EcsArchetype*	FindArchetype( const EcsSignature _signature )
		{
			auto it = m_archetypes.find( _signature );
			return it == m_archetypes.end() ? nullptr : it->second;
		}
		EcsArchetype&	CreateArchetype( const EcsSignature _signature )
		{
			assert( FindArchetype( _signature ) == nullptr );
			EcsArchetype* newArchetype = new EcsArchetype();
			newArchetype->Create( m_componentsInfo, _signature );
			m_archetypes[_signature] = newArchetype;
			return *newArchetype;
		}
		EcsTransition&	FindOrCreateTransition( const EcsEntity _entity )
		{
			// Get/register transition
			EcsEntityData& entity = _entity.archetype->m_entities[_entity.index];
			EcsTransition* transition = nullptr;
			if( entity.transitionIndex < 0 )
			{
				entity.transitionIndex = m_transitionArchetype.Size();
				m_transitions.emplace_back();
				transition = &( *m_transitions.rbegin() );
				transition->entityID = _entity;

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

		EcsHandle											m_nextHandle = 1;	// 0 is a null handle
		int													m_nextTagIndex = ecsSignatureLength - 1;
		EcsSignature										m_tagsMask;			// signature of all tag types combined
		EcsArchetype										m_transitionArchetype;
		std::unordered_map< EcsSignature, EcsArchetype* >	m_archetypes;
		std::unordered_map<uint32_t, int >					m_typeToIndex;
		std::unordered_map< EcsHandle, EcsEntity >			m_handles;
		std::unordered_map< uint32_t, EcsSingleton* >		m_singletons;
		std::unordered_map< uint32_t, EcsSingletonInfo >	m_singletonInfos;
		std::vector< EcsComponentInfo >						m_componentsInfo;
		std::vector< EcsTransition >						m_transitions;
	};

	//================================
	//================================
	struct EcsSystemView
	{
		EcsSystemView( EcsWorld2& _world ) : m_world( _world )
		{}

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
			const int index = m_world.GetIndex(_ComponentType::Info::s_type);
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