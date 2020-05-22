#pragma once

#include "ecs/fanEcsWorld.hpp"

namespace fan
{

	void EcsWorld::Create() //@todo delete this method and do it incrementally
	{
		m_transitionArchetype.Create( m_componentsInfo, ~EcsSignature( 0 ) );
	}
	void EcsWorld::ApplyTransitions()
	{
		// Applies structural transition to entities ( add/remove components/tags, add/remove entities
		assert( m_transitionArchetype.Size() == m_transitions.size() );
		for( int transitionIndex = m_transitionArchetype.Size() - 1; transitionIndex >= 0; --transitionIndex )
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
				EcsEntityData& dstEntity = *dstArchetype->m_entities.rbegin();
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
	void EcsWorld::Clear()
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

	EcsHandle	EcsWorld::AddHandle( const EcsEntity _entity )
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
	void		EcsWorld::RemoveHandle( const EcsEntity _entity )
	{
		EcsEntityData& entity = _entity.archetype->m_entities[_entity.index];
		if( entity.handle != 0 )
		{
			m_handles.erase( entity.handle );
			entity.handle = 0;
		}
	}
	const EcsSingletonInfo* EcsWorld::SafeGetSingletonInfo( const uint32_t _type ) const
	{
		const auto& it = m_singletonInfos.find( _type );
		return it == m_singletonInfos.end() ? nullptr : &it->second;
	}
	std::vector< EcsSingletonInfo > EcsWorld::GetVectorSingletonInfo() const
	{
		std::vector< EcsSingletonInfo > infos;
		for( const std::pair<uint32_t, EcsSingletonInfo>& info : m_singletonInfos )
		{
			infos.push_back( info.second );
		}
		return infos;
	}

	void EcsWorld::AddTag( const EcsEntity _entity, const uint32_t _type )
	{
		const int tagIndex = GetIndex( _type );
		EcsTransition& transition = FindOrCreateTransition( _entity );

		// entity doesn't already have this tag
		assert( _entity.archetype == &m_transitionArchetype || !_entity.archetype->m_signature[tagIndex] );

		// Update transition
		transition.signatureAdd[tagIndex] = 1;
	}
	void EcsWorld::RemoveTag( const EcsEntity _entity, const uint32_t _type )
	{
		const int tagIndex = GetIndex( _type );
		EcsTransition& transition = FindOrCreateTransition( _entity );

		// entity must have this tag
		assert( _entity.archetype == &m_transitionArchetype || _entity.archetype->m_signature[tagIndex] );

		// Update transition
		transition.signatureRemove[tagIndex] = 1;
	}
	bool EcsWorld::HasTag( const EcsEntity _entity, const uint32_t _type ) const
	{
		const int tagIndex = GetIndex( _type );
		return _entity.archetype->m_signature[tagIndex];
	}
	void EcsWorld::AddTagsFromSignature( const EcsEntity _entity, const EcsSignature& _signature )
	{
		EcsTransition& transition = FindOrCreateTransition( _entity );
		transition.signatureAdd |= _signature & m_tagsMask;
	}

	EcsComponent& EcsWorld::AddComponent( const EcsEntity _entity, const uint32_t _type )
	{
		const int componentIndex = GetIndex( _type );
		EcsTransition& transition = FindOrCreateTransition( _entity );
		EcsEntityData& entityData = GetEntityData( _entity );

		// entity doesn't already have this component
		assert( _entity.archetype == &m_transitionArchetype || !_entity.archetype->m_signature[componentIndex] );

		// Update transition
		transition.signatureAdd[componentIndex] = 1;

		return *static_cast<EcsComponent*>( m_transitionArchetype.m_chunks[componentIndex].At( entityData.transitionIndex ) );
	}
	void		  EcsWorld::RemoveComponent( const EcsEntity _entity, const uint32_t _type )
	{
		const int componentIndex = GetIndex( _type );
		EcsTransition& transition = FindOrCreateTransition( _entity );

		// entity must have this component
		assert( _entity.archetype == &m_transitionArchetype || _entity.archetype->m_signature[componentIndex] );

		// Update transition
		transition.signatureRemove[componentIndex] = 1;
	}
	EcsComponent& EcsWorld::GetComponent( const EcsEntity _entity, const uint32_t _type )
	{
		const int index = m_typeToIndex[_type];
		return *static_cast<EcsComponent*>( _entity.archetype->m_chunks[index].At( _entity.index ) );
	}

	EcsEntity EcsWorld::CreateEntity()
	{
		EcsEntity entityID;
		entityID.archetype = &m_transitionArchetype;
		entityID.index = m_transitionArchetype.Size();

		EcsEntityData entity;
		entity.transitionIndex = int( m_transitions.size() );

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
	void EcsWorld::Kill( const EcsEntity _entity )
	{
		EcsTransition& transition = FindOrCreateTransition( _entity );
		transition.isDead = true;
	}
	bool EcsWorld::IsAlive( const EcsEntity _entity ) const
	{
		const EcsEntityData& entityData = GetEntityData( _entity );
		return entityData.transitionIndex < 0 || !m_transitions[entityData.transitionIndex].isDead;
	}
	EcsView EcsWorld::Match( const EcsSignature _signature ) const
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

	EcsArchetype* EcsWorld::FindArchetype( const EcsSignature _signature )
	{
		auto it = m_archetypes.find( _signature );
		return it == m_archetypes.end() ? nullptr : it->second;
	}
	EcsArchetype& EcsWorld::CreateArchetype( const EcsSignature _signature )
	{
		assert( FindArchetype( _signature ) == nullptr );
		EcsArchetype* newArchetype = new EcsArchetype();
		newArchetype->Create( m_componentsInfo, _signature );
		m_archetypes[_signature] = newArchetype;
		return *newArchetype;
	}
	EcsTransition& EcsWorld::FindOrCreateTransition( const EcsEntity _entity )
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
}