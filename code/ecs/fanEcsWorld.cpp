#pragma once

#include "ecs/fanEcsWorld.hpp"

namespace fan
{

	void EcsWorld::Create() //@todo delete this method and do it incrementally
	{
		m_transitionArchetype.Create( m_componentsInfo, ~EcsSignature( 0 ) );
	}

	struct SortedTransition
	{
		int transitionIndex;
		uint32_t entityIndex;
		bool operator<( const SortedTransition& _other ) const  { return ( entityIndex < _other.entityIndex ); }
	};

	void EcsWorld::ApplyTransitions()
	{
		assert( m_transitionArchetype.Size() == m_transitions.size() );
		if( m_transitions.size() == 0 ) { return; }

		m_isApplyingTransitions = true;

		// Sort transitions by ascending entity index
		std::vector<SortedTransition> sortedTransitions;
		sortedTransitions.reserve( m_transitions.size() );
		for (int transitionIndex = 0; transitionIndex < m_transitions.size(); transitionIndex++)
		{
			const EcsTransition& transition = m_transitions[transitionIndex];
			sortedTransitions.push_back( { transitionIndex, transition.entity.index } );
		}
		std::sort( sortedTransitions.begin(), sortedTransitions.end() );

		// Applies structural transition to entities ( add/remove components/tags, add/remove entities
		for (int sortedTransitionIdx = int(sortedTransitions.size()) - 1; sortedTransitionIdx >= 0 ; sortedTransitionIdx--)
		{
			const SortedTransition&		sortedTransition = sortedTransitions[sortedTransitionIdx];
			const int					transitionIndex = sortedTransition.transitionIndex;
			const EcsTransition&		transition = m_transitions[ transitionIndex ];
			const EcsEntity&			srcEntity = transition.entity;
			const uint32_t				srcIndex = srcEntity.index;
			EcsArchetype&				srcArchetype = *srcEntity.archetype;
			const bool					srcArchetypeIsTransitionArchetype = &srcArchetype == &m_transitionArchetype;
			const EcsEntityData&		srcEntityData = srcArchetype.m_entities[srcIndex];
			assert( srcEntityData.transitionIndex == transitionIndex );
			assert( ! ( srcArchetypeIsTransitionArchetype && transition.signatureRemove != EcsSignature( 0 ) ) );

			// Create destination signature
			EcsSignature targetSignature = EcsSignature( 0 );
			if( &srcArchetype != &m_transitionArchetype )
			{
				targetSignature |= srcArchetype.m_signature;
			}
			targetSignature |= transition.signatureAdd;
			targetSignature &= ~transition.signatureRemove;

			if( transition.isDead || targetSignature == EcsSignature( 0 ) ) // entity is dead, no need to consider a target archetype
			{
				// destroy all components from the src archetype
				if( !srcArchetypeIsTransitionArchetype )
				{
					for( int componentIndex = 0; componentIndex < NumComponents(); componentIndex++ )
					{
						if( srcArchetype.m_signature[componentIndex] )
						{
							const EcsComponentInfo& info = m_componentsInfo[componentIndex];
							if( info.destroy != nullptr )
							{
								EcsComponent& component = *static_cast<EcsComponent*>( srcArchetype.m_chunks[componentIndex].At( srcIndex ) );
								info.destroy( *this, srcEntity, component );
							}
							srcArchetype.m_chunks[componentIndex].Remove( srcIndex );
						}
					}
				}

				// destroy all components from the transition archetype
				for( int componentIndex = 0; componentIndex < NumComponents(); componentIndex++ )
				{
					if( transition.signatureAdd[componentIndex] )
					{
						const EcsComponentInfo& info = m_componentsInfo[componentIndex];
						if( info.destroy != nullptr )
						{
							EcsComponent& component = *static_cast<EcsComponent*>( m_transitionArchetype.m_chunks[componentIndex].At( srcIndex ) );
							info.destroy( *this, srcEntity, component );
						}
					}
				}

				// remove handle
				if( srcEntityData.handle != 0 )
				{
					m_handles.erase( srcEntityData.handle );
				}

				// if another entity was moved, update its handle	
				if( !srcArchetypeIsTransitionArchetype )
				{					
					if( srcIndex != srcArchetype.Size() - 1 )
					{						
						EcsEntityData movedEntity = *srcArchetype.m_entities.rbegin();
						if( movedEntity.handle != 0 )
						{
							m_handles[movedEntity.handle].index = srcIndex;
						}
					}	
					srcArchetype.RemoveEntity( srcIndex );
				}
				
			}
			else // entity is alive, we need to copy remaining components to the target archetype
			{
				// get dst archetype
				EcsArchetype* dstArchetype = FindArchetype( targetSignature );
				if( dstArchetype == nullptr )
				{
					dstArchetype = &CreateArchetype( targetSignature );
				}

				// push new entity
				const uint32_t dstIndex = dstArchetype->Size();
				dstArchetype->m_entities.push_back( srcArchetype.m_entities[srcIndex] );
				EcsEntityData& dstEntityData = *dstArchetype->m_entities.rbegin();
				dstEntityData = srcEntityData;
				dstEntityData.transitionIndex = -1;
				if( srcEntityData.handle != 0 )
				{
					m_handles[srcEntityData.handle] = { dstArchetype, dstIndex };
				}

				// moves components from the source archetype to the target archetype			
				if( !srcArchetypeIsTransitionArchetype )
				{
					for( int i = 0; i < NumComponents(); i++ )
					{
						if( srcArchetype.m_signature[i] )
						{
							if( transition.signatureRemove[i] ) // if the component was removed, calls destroy
							{
								const EcsComponentInfo& info = m_componentsInfo[i];
								if( info.destroy != nullptr )
								{
									EcsComponent& component = *static_cast<EcsComponent*>( srcArchetype.m_chunks[i].At( srcIndex ) );
									info.destroy( *this, srcEntity, component );
								}
							}
							else // copy component to the target archetype
							{
								dstArchetype->m_chunks[i].PushBack( srcArchetype.m_chunks[i].At( srcIndex ) );
							}
							srcArchetype.m_chunks[i].Remove( srcIndex );							
						}
					}

					// if another entity was moved, update its handle	
					if( !srcArchetypeIsTransitionArchetype )
					{
						if( srcIndex != srcArchetype.Size() - 1 )
						{
							EcsEntityData movedEntity = *srcArchetype.m_entities.rbegin();
							if( movedEntity.handle != 0 )
							{
								m_handles[movedEntity.handle].index = srcIndex;
							}
						}
						srcArchetype.RemoveEntity( srcIndex );
					}
				}

				// moves components from the transition archetype to the target archetype
				for( int i = 0; i < NumComponents(); i++ )
				{
					if( transition.signatureAdd[i] )
					{
						dstArchetype->m_chunks[i].PushBack( m_transitionArchetype.m_chunks[i].At( transitionIndex ) );
					}
				}
			}
		}
		assert( m_transitions.size() == sortedTransitions.size() );			// we must keep the transitions intact during the apply
		assert( m_transitionArchetype.Size() == sortedTransitions.size() );	// we must keep the transition archetype intact during the apply 

		m_transitions.clear();
		m_transitionArchetype.Clear();

		m_isApplyingTransitions = false;
	}

	int  EcsWorld::GetIndex( const uint32_t  _type ) const 
	{ 
		return m_typeToIndex.at( _type ); 
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
		assert( entity.handle == 0 );
		entity.handle = m_nextHandle++;
		m_handles[entity.handle] = _entity;
		return entity.handle;
	}

	EcsHandle	EcsWorld::GetHandle( const EcsEntity _entity ) const
	{
		EcsEntityData& entity = _entity.archetype->m_entities[_entity.index];
		return entity.handle;
	}

	//================================================================================================================================
	// Assigns a specific handle to an entity without incrementing the m_nextHandle counter
	// usefull during scene loading & prefab instanciation
	//================================================================================================================================
	void EcsWorld::SetHandle( const EcsEntity _entity, EcsHandle _handle )
	{
		assert( _handle != 0 );
		assert( _handle >= m_nextHandle );

		EcsEntityData& entity = _entity.archetype->m_entities[_entity.index];
		assert( entity.handle == 0 );
		entity.handle = _handle;

		assert( m_handles.find(entity.handle) == m_handles.end() );
		m_handles[entity.handle] = _entity;
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
	const	    EcsSingletonInfo* EcsWorld::SafeGetSingletonInfo( const uint32_t _type ) const
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

		// Update transition
		transition.signatureAdd[tagIndex] = 1;
	}
	void EcsWorld::RemoveTag( const EcsEntity _entity, const uint32_t _type )
	{
		const int tagIndex = GetIndex( _type );
		EcsTransition& transition = FindOrCreateTransition( _entity );

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
		EcsEntityData& entityData = GetEntityData( _entity );

		// entity doesn't already have this component
		assert( _entity.archetype == &m_transitionArchetype || !_entity.archetype->m_signature[componentIndex] );

		// Update transition
		EcsTransition& transition = FindOrCreateTransition( _entity );
		assert( !transition.signatureRemove[componentIndex] );
		transition.signatureAdd[componentIndex] = 1;

		EcsComponent& component = *static_cast<EcsComponent*>( m_transitionArchetype.m_chunks[componentIndex].At( entityData.transitionIndex ) );
		EcsComponentInfo info = m_componentsInfo[componentIndex];
		info.construct( &component );
		info.init( *this, _entity, component );

		return component;
	}
	void		  EcsWorld::RemoveComponent( const EcsEntity _entity, const uint32_t _type )
	{
		const int componentIndex = GetIndex( _type );

		// entity must have this component
		assert( _entity.archetype == &m_transitionArchetype || _entity.archetype->m_signature[componentIndex] );

		// Update transition
		EcsTransition& transition = FindOrCreateTransition( _entity );
		assert( !transition.signatureAdd[componentIndex] );
		transition.signatureRemove[componentIndex] = 1;
	}
	bool		  EcsWorld::HasComponent( const EcsEntity _entity, const uint32_t _type )
	{
		const int componentIndex = GetIndex( _type );
		if( _entity.archetype == &m_transitionArchetype )
		{
			const EcsEntityData& entityData = _entity.archetype->m_entities[_entity.index];
			assert( entityData.transitionIndex != -1 );
			const EcsTransition& transition = m_transitions[entityData.transitionIndex];
			return transition.signatureAdd[componentIndex];
		}
		else
		{
			if( _entity.archetype->m_signature[componentIndex] )
			{
				return true;
			}
			else
			{
				const EcsEntityData& entityData = _entity.archetype->m_entities[_entity.index];
				if( entityData.transitionIndex != -1 )
				{
					const EcsTransition& transition = m_transitions[entityData.transitionIndex];
					return transition.signatureAdd[componentIndex];
				}
				else
				{
					return false;
				}
			}
		}
	}
	EcsComponent& EcsWorld::GetComponent( const EcsEntity _entity, const uint32_t _type )
	{
		assert( HasComponent( _entity, _type ) );
		const int componentIndex = GetIndex( _type );
		if( _entity.archetype == &m_transitionArchetype )
		{
			const EcsEntityData& entityData = _entity.archetype->m_entities[_entity.index];
			assert( entityData.transitionIndex != -1 );
			const EcsTransition& transition = m_transitions[entityData.transitionIndex];
			return *static_cast<EcsComponent*>( m_transitionArchetype.m_chunks[componentIndex].At( entityData.transitionIndex ) );
		}
		else
		{
			if( _entity.archetype->m_signature[componentIndex] )
			{
				return *static_cast<EcsComponent*>( _entity.archetype->m_chunks[componentIndex].At( _entity.index ) );
			}
			else
			{
				const EcsEntityData& entityData = _entity.archetype->m_entities[_entity.index];
				const EcsTransition& transition = m_transitions[entityData.transitionIndex];
				return *static_cast<EcsComponent*>( m_transitionArchetype.m_chunks[componentIndex].At( entityData.transitionIndex ) );
			}
		}
	}

	EcsEntity EcsWorld::CreateEntity()
	{
		EcsEntity entityID;
		entityID.archetype = &m_transitionArchetype;
		entityID.index = m_transitionArchetype.Size();

		EcsEntityData entity;
		entity.transitionIndex = int( m_transitions.size() );

		EcsTransition transition;
		transition.entity = entityID;

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
		assert( !m_isApplyingTransitions );

		// Get/register transition
		EcsEntityData& entity = _entity.archetype->m_entities[_entity.index];
		EcsTransition* transition = nullptr;
		if( entity.transitionIndex < 0 )
		{
			entity.transitionIndex = m_transitionArchetype.Size();
			m_transitions.emplace_back();
			transition = &( *m_transitions.rbegin() );
			transition->entity = _entity;

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