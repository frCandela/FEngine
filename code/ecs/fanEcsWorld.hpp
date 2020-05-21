#pragma once

#include <unordered_map>
#include <cassert>
#include "core/fanHash.hpp"
#include "ecs/fanEcsEntity.hpp"
#include "ecs/fanEcsComponent.hpp"
#include "ecs/fanEcsSingleton.hpp"
#include "ecs/fanEcsTag.hpp"
#include "ecs/fanEcsArchetype.hpp"
#include "ecs/fanEcsView.hpp"

namespace fan
{
	struct EcsView;

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
	class EcsWorld
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
		EcsHandle AddHandle( const EcsEntity _entity )
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
		EcsSingleton&		GetSingleton( const uint32_t _type )		{ return  *m_singletons[_type];		}
		const EcsSingleton& GetSingleton( const uint32_t _type ) const	{ return  *m_singletons.at(_type);	}
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
 			const int newTagIndex = m_nextTagIndex--;
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
			return static_cast<_ComponentType&>( AddComponent( _entity, _ComponentType::Info::s_type ) );
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
		template< typename _ComponentType >	_ComponentType& GetComponent( const EcsEntity _entity )
		{
			static_assert( std::is_base_of< EcsComponent, _ComponentType>::value );
			return static_cast<_ComponentType&> ( GetComponent( _entity, _ComponentType::Info::s_type ) );
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
		void		  RemoveComponent( const EcsEntity _entity, const uint32_t _type )
		{
			const int componentIndex = GetIndex(_type);
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
		EcsComponent& GetComponent( const EcsEntity _entity, const uint32_t _type )
		{
			const int index = m_typeToIndex[_type];
			return *static_cast<EcsComponent*>( _entity.archetype->m_chunks[index].At( _entity.index ) );
		}
		const EcsComponentInfo& GetComponentInfo( const uint32_t _type ) const { return  m_componentsInfo.at( GetIndex(_type) ); }

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
		EcsView Match( const EcsSignature _signature ) const
		{
			EcsView view( m_typeToIndex, _signature );
			for( auto it = m_archetypes.begin(); it != m_archetypes.end(); ++it )
			{
				if( ( it->first & _signature ) == _signature && !it->second->Empty() )
				{
					view.m_archetypes.push_back( it->second );
				}
			}
			return view;			
		}
		template< typename _tagOrComponentType > EcsSignature	GetSignature() const
		{
			static_assert( std::is_base_of< EcsTag, _tagOrComponentType>::value || std::is_base_of< Component, _tagOrComponentType>::value );
			return EcsSignature( 1 ) << m_typeToIndex.at( _tagOrComponentType::s_type );
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
}