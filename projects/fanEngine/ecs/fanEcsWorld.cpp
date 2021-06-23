#include "ecs/fanEcsWorld.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/fanBits.hpp"
#include "core/fanDebug.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SortedTransition
    {
        int      transitionIndex;
        uint32_t entityIndex;
        bool operator<( const SortedTransition& _other ) const
        {
            return ( entityIndex < _other.entityIndex );
        }
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EcsWorld::ApplyTransitions()
    {
        SCOPED_PROFILE( apply_transitions );

        // calls destructors
        while( !mDestroyedComponents.empty() )
        {
            const DestroyedComponent destroyedComponent = *mDestroyedComponents.rbegin();
            mDestroyedComponents.pop_back();
            destroyedComponent.destroy( *this, destroyedComponent.mEntity, destroyedComponent.mComponent );
        }

        fanAssert( mTransitionArchetype.Size() == (int)mTransitions.size() );
        if( mTransitions.size() == 0 ){ return; }

        // Sort transitions by ascending entity index
        std::vector<SortedTransition> sortedTransitions;
        sortedTransitions.reserve( mTransitions.size() );
        for( int transitionIndex = 0; transitionIndex < (int)mTransitions.size(); transitionIndex++ )
        {
            const EcsTransition& transition = mTransitions[transitionIndex];
            sortedTransitions.push_back( { transitionIndex, transition.mEntity.mIndex } );
        }
        std::sort( sortedTransitions.begin(), sortedTransitions.end() );

        // Applies structural transition to entities ( add/remove components/tags, add/remove entities
        for( int transitionIdx = int( sortedTransitions.size() ) - 1; transitionIdx >= 0; transitionIdx-- )
        {
            const SortedTransition& sortedTransition = sortedTransitions[transitionIdx];
            const int transitionIndex = sortedTransition.transitionIndex;
            const EcsTransition& transition = mTransitions[transitionIndex];
            const EcsEntity    & srcEntity  = transition.mEntity;
            const uint32_t srcIndex = srcEntity.mIndex;
            EcsArchetype& srcArchetype = *srcEntity.mArchetype;
            const bool srcArchetypeIsTransitionArchetype = ( &srcArchetype == &mTransitionArchetype );
            const EcsEntityData& srcEntityData = srcArchetype.GetEntityData( srcIndex );
            fanAssert( srcEntityData.mTransitionIndex == transitionIndex );
            fanAssert( !( srcArchetypeIsTransitionArchetype &&
                          transition.mSignatureRemove != EcsSignature( 0 ) ) );

            // Create destination signature
            EcsSignature targetSignature = EcsSignature( 0 );
            if( &srcArchetype != &mTransitionArchetype )
            {
                targetSignature |= srcArchetype.GetSignature();
            }
            targetSignature |= transition.mSignatureAdd;
            targetSignature &= ~transition.mSignatureRemove;

            // if we didn't change anything, continue after erasing the transition
            if( !transition.mIsDead && targetSignature == srcArchetype.GetSignature() )
            {
                srcArchetype.GetEntityData( srcIndex ).mTransitionIndex = -1;
                continue;
            }

            // entity is dead, no need to consider a target archetype
            if( transition.mIsDead || targetSignature == EcsSignature( 0 ) )
            {
                fanAssert( !srcArchetypeIsTransitionArchetype );

                // destroy all components from the src archetype
                if( !srcArchetypeIsTransitionArchetype )
                {
                    for( int componentIndex = 0; componentIndex < NumComponents(); componentIndex++ )
                    {
                        if( srcArchetype.GetSignature()[componentIndex] )
                        {
                            srcArchetype.GetChunkVector( componentIndex ).Remove( srcIndex );
                        }
                    }
                }

                // remove handle
                if( srcEntityData.mHandle != 0 )
                {
                    mHandles.erase( srcEntityData.mHandle );
                }

                // if another entity was moved, update its handle
                if( !srcArchetypeIsTransitionArchetype )
                {
                    if( srcIndex != uint32_t( srcArchetype.Size() - 1 ) )
                    {
                        EcsEntityData movedEntity = srcArchetype.GetEntityData( srcArchetype.Size() - 1 );
                        if( movedEntity.mHandle != 0 )
                        {
                            mHandles[movedEntity.mHandle].mIndex = srcIndex;
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
                dstArchetype->PushBackEntityData( srcArchetype.GetEntityData( srcIndex ) );
                EcsEntityData& dstEntityData = dstArchetype->GetEntityData( dstArchetype->Size() - 1 );
                dstEntityData = srcEntityData;
                dstEntityData.mTransitionIndex = -1;
                if( srcEntityData.mHandle != 0 )
                {
                    mHandles[srcEntityData.mHandle] = { dstArchetype, dstIndex };
                }

                // moves components from the source archetype to the target archetype
                if( !srcArchetypeIsTransitionArchetype )
                {
                    for( int i = 0; i < NumComponents(); i++ )
                    {
                        if( srcArchetype.GetSignature()[i] )
                        {
                            if( !transition.mSignatureRemove[i] ) // copy component to the target archetype
                            {
                                dstArchetype->GetChunkVector( i ).PushBack(
                                        srcArchetype.GetChunkVector( i ).At( srcIndex ) );
                            }
                            srcArchetype.GetChunkVector( i ).Remove( srcIndex );
                        }
                    }

                    // if another entity was moved, update its handle
                    if( !srcArchetypeIsTransitionArchetype )
                    {
                        if( srcIndex != uint32_t( srcArchetype.Size() - 1 ) )
                        {
                            EcsEntityData movedEntity = srcArchetype.GetEntityData( srcArchetype.Size() - 1 );
                            if( movedEntity.mHandle != 0 )
                            {
                                mHandles[movedEntity.mHandle].mIndex = srcIndex;
                            }
                        }
                        srcArchetype.RemoveEntity( srcIndex );
                    }
                }

                // moves components from the transition archetype to the target archetype
                for( int i = 0; i < NumComponents(); i++ )
                {
                    if( transition.mSignatureAdd[i] )
                    {
                        dstArchetype->GetChunkVector( i ).PushBack(
                                mTransitionArchetype.GetChunkVector( i ).At( transitionIndex ) );
                    }
                }
            }
        }
        // we must keep the transitions intact during the apply
        fanAssert( mTransitions.size() == sortedTransitions.size() );

        // we must keep the transition archetype intact during the apply
        fanAssert( mTransitionArchetype.Size() == (int)sortedTransitions.size() );

        mTransitions.clear();
        mTransitionArchetype.Clear();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    int EcsWorld::GetIndex( const uint32_t _type ) const
    {
        return mTypeToIndex.at( _type );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EcsWorld::Clear()
    {
        // clears singleton components
        for( std::pair<uint32_t, EcsSingleton*> pair : mSingletons )
        {
            const EcsSingletonInfo& info = GetSingletonInfo( pair.first );
            if( info.destroy )
            {
                info.destroy( *this, *pair.second );
            }
        }

        ApplyTransitions();
        // Remove all entities
        for( auto it = mArchetypes.begin(); it != mArchetypes.end(); ++it )
        {
            it->second->Clear();
        }
        mTransitionArchetype.Clear();

        mHandles.clear();
        mNextHandle = 1;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EcsWorld::InitSingletons()
    {
        for( std::pair<uint32_t, EcsSingleton*> pair : mSingletons )
        {
            const EcsSingletonInfo& info = GetSingletonInfo( pair.first );
            if( !BITMASK_TRUE_ANY( info.mFlags, EcsSingletonFlags::InitOnce ) )
            {
                info.init( *this, *pair.second );
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EcsWorld::PostInitSingletons( const bool _force )
    {
        // clears singleton components
        for( std::pair<uint32_t, EcsSingleton*> pair : mSingletons )
        {
            const EcsSingletonInfo& info = GetSingletonInfo( pair.first );
            if( info.postInit != nullptr )
            {
                if( !BITMASK_TRUE_ANY( info.mFlags, EcsSingletonFlags::InitOnce ) || _force )
                {
                    info.postInit( *this, *pair.second );
                }
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EcsWorld::ReloadInfos()
    {
        for( auto it = mSingletonInfos.begin(); it != mSingletonInfos.end(); it++ )
        {
            EcsSingletonInfo& info = it->second;
            ( *info.setInfo )( info );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    EcsHandle EcsWorld::AddHandle( const EcsEntity _entity )
    {
        EcsEntityData& entity = _entity.mArchetype->GetEntityData( _entity.mIndex );
        fanAssert( entity.mHandle == 0 );
        entity.mHandle = mNextHandle++;
        mHandles[entity.mHandle] = _entity;
        return entity.mHandle;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    EcsHandle EcsWorld::GetHandle( const EcsEntity _entity ) const
    {
        EcsEntityData& entity = _entity.mArchetype->GetEntityData( _entity.mIndex );
        return entity.mHandle;
    }

    //==================================================================================================================================================================================================
    // Assigns a specific handle to an entity without incrementing the m_nextHandle counter
    // useful during scene loading & prefab instantiation
    //==================================================================================================================================================================================================
    void EcsWorld::SetHandle( const EcsEntity _entity, EcsHandle _handle )
    {
        fanAssert( _handle != 0 );
        fanAssert( _handle >= mNextHandle );

        EcsEntityData& entity = _entity.mArchetype->GetEntityData( _entity.mIndex );
        fanAssert( entity.mHandle == 0 );
        entity.mHandle = _handle;

        fanAssert( mHandles.find( entity.mHandle ) == mHandles.end() );
        mHandles[entity.mHandle] = _entity;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EcsWorld::RemoveHandle( const EcsEntity _entity )
    {
        EcsEntityData& entity = _entity.mArchetype->GetEntityData( _entity.mIndex );
        if( entity.mHandle != 0 )
        {
            mHandles.erase( entity.mHandle );
            entity.mHandle = 0;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    const EcsSingletonInfo* EcsWorld::SafeGetSingletonInfo( const uint32_t _type ) const
    {
        const auto& it = mSingletonInfos.find( _type );
        return it == mSingletonInfos.end() ? nullptr : &it->second;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::vector<EcsSingletonInfo> EcsWorld::GetVectorSingletonInfo() const
    {
        std::vector<EcsSingletonInfo> infos;
        for( const std::pair<uint32_t, EcsSingletonInfo>& info : mSingletonInfos )
        {
            infos.push_back( info.second );
        }
        return infos;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EcsWorld::AddTag( const EcsEntity _entity, const uint32_t _type )
    {
        const int tagIndex = GetIndex( _type );
        const EcsEntityData& entityData = _entity.mArchetype->GetEntityData( _entity.mIndex );
        if( entityData.mTransitionIndex != -1 || !_entity.mArchetype->GetSignature()[tagIndex] )
        {
            EcsTransition& transition = FindOrCreateTransition( _entity );
            transition.mSignatureAdd[tagIndex]    = 1;
            transition.mSignatureRemove[tagIndex] = 0;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EcsWorld::RemoveTag( const EcsEntity _entity, const uint32_t _type )
    {
        const int tagIndex = GetIndex( _type );
        const EcsEntityData& entityData = _entity.mArchetype->GetEntityData( _entity.mIndex );
        if( _entity.mArchetype == &mTransitionArchetype )
        {
            fanAssert( entityData.mTransitionIndex != -1 );
            EcsTransition& transition = FindOrCreateTransition( _entity );
            fanAssert( transition.mSignatureRemove[tagIndex] == 0 );
            transition.mSignatureAdd[tagIndex] = 0;
        }
        else if( entityData.mTransitionIndex != -1 || _entity.mArchetype->GetSignature()[tagIndex] )
        {
            EcsTransition& transition = FindOrCreateTransition( _entity );
            transition.mSignatureRemove[tagIndex] = 1;
            transition.mSignatureAdd[tagIndex]    = 0;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool EcsWorld::HasTag( const EcsEntity _entity, const uint32_t _type ) const
    {
        const int tagIndex = GetIndex( _type );
        return IndexedHasTag( _entity, tagIndex );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool EcsWorld::IndexedHasTag( const EcsEntity _entity, const int _tagIndex ) const
    {
        fanAssert( _tagIndex < (int)ecsSignatureLength );
        fanAssert( _tagIndex >= GetFistTagIndex() );
        const EcsEntityData& entityData = _entity.mArchetype->GetEntityData( _entity.mIndex );
        if( entityData.mTransitionIndex != -1 )
        {
            const EcsTransition& transition = mTransitions[entityData.mTransitionIndex];
            if( transition.mSignatureAdd[_tagIndex] )
            {
                return true;
            }
            else if( transition.mSignatureRemove[_tagIndex] )
            {
                return false;
            }
        }
        return _entity.mArchetype->GetSignature()[_tagIndex];
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    const EcsTagInfo& EcsWorld::IndexedGetTagInfo( const int _tagIndex ) const
    {
        fanAssert( _tagIndex < (int)ecsSignatureLength );
        fanAssert( _tagIndex >= GetFistTagIndex() );
        int reverseIndex = int( ecsSignatureLength ) - 1 - _tagIndex;
        return mTagsInfo[reverseIndex];
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EcsWorld::AddTagsFromSignature( const EcsEntity _entity, const EcsSignature& _signature )
    {
        EcsTransition& transition = FindOrCreateTransition( _entity );
        transition.mSignatureAdd |= _signature & mTagsMask;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    EcsComponent& EcsWorld::AddComponent( const EcsEntity _entity, const uint32_t _type )
    {
        if( HasComponent( _entity, _type ) )
        {
            return GetComponent( _entity, _type );
        }

        const int componentIndex = GetIndex( _type );
        EcsEntityData& entityData = GetEntityData( _entity );

        // Update transition
        EcsTransition& transition = FindOrCreateTransition( _entity );
        fanAssert( !transition.mSignatureRemove[componentIndex] );
        transition.mSignatureAdd[componentIndex] = 1;

        EcsComponent& component = *static_cast<EcsComponent*>(
                mTransitionArchetype.GetChunkVector( componentIndex ).At( entityData.mTransitionIndex ) );
        EcsComponentInfo info = mComponentsInfo[componentIndex];
        info.construct( &component );
        info.init( *this, _entity, component );

        return component;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EcsWorld::RemoveComponent( const EcsEntity _entity, const uint32_t _type )
    {
        const int componentIndex = GetIndex( _type );

        fanAssert( _entity.mArchetype != &mTransitionArchetype );
        fanAssert( _entity.mArchetype->GetSignature()[componentIndex] );// entity must have this component

        // Update transition
        EcsTransition& transition = FindOrCreateTransition( _entity );
        fanAssert( !transition.mSignatureAdd[componentIndex] );

        // calls destructor on the component
        if( !transition.mSignatureRemove[componentIndex] )
        {
            transition.mSignatureRemove[componentIndex] = 1;
            const EcsComponentInfo& info = mComponentsInfo[componentIndex];
            if( info.destroy != nullptr )
            {
                EcsComponent& component = *static_cast<EcsComponent*>(
                        _entity.mArchetype->GetChunkVector( componentIndex ).At( _entity.mIndex ) );
                mDestroyedComponents.push_back( { _entity, component, info.destroy } );
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool EcsWorld::HasComponent( const EcsEntity _entity, const uint32_t _type )
    {
        const int componentIndex = GetIndex( _type );
        return IndexedHasComponent( _entity, componentIndex );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool EcsWorld::IndexedHasComponent( const EcsEntity _entity, const int _componentindex )
    {
        if( _entity.mArchetype == &mTransitionArchetype )
        {
            const EcsEntityData& entityData = _entity.mArchetype->GetEntityData( _entity.mIndex );
            fanAssert( entityData.mTransitionIndex != -1 );
            const EcsTransition& transition = mTransitions[entityData.mTransitionIndex];
            return transition.mSignatureAdd[_componentindex];
        }
        else
        {
            if( _entity.mArchetype->GetSignature()[_componentindex] )
            {
                return true;
            }
            else
            {
                const EcsEntityData& entityData = _entity.mArchetype->GetEntityData( _entity.mIndex );
                if( entityData.mTransitionIndex != -1 )
                {
                    const EcsTransition& transition = mTransitions[entityData.mTransitionIndex];
                    return transition.mSignatureAdd[_componentindex];
                }
                else
                {
                    return false;
                }
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    EcsComponent& EcsWorld::GetComponent( const EcsEntity _entity, const uint32_t _type )
    {
        fanAssert( HasComponent( _entity, _type ) );
        return IndexedGetComponent( _entity, GetIndex( _type ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    EcsComponent* EcsWorld::SafeGetComponent( const EcsEntity _entity, const uint32_t _type )
    {
        const int componentIndex = GetIndex( _type );
        if( IndexedHasComponent( _entity, componentIndex ) )
        {
            return &IndexedGetComponent( _entity, componentIndex );
        }
        return nullptr;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    EcsComponent& EcsWorld::IndexedGetComponent( const EcsEntity _entity, const int _componentIndex )
    {
        fanAssert( _componentIndex < NumComponents() );
        if( _entity.mArchetype == &mTransitionArchetype )
        {
            const EcsEntityData& entityData = _entity.mArchetype->GetEntityData( _entity.mIndex );
            fanAssert( entityData.mTransitionIndex != -1 );
            return *static_cast<EcsComponent*>( mTransitionArchetype.
                    GetChunkVector( _componentIndex ).At( entityData.mTransitionIndex ) );
        }
        else
        {
            if( _entity.mArchetype->GetSignature()[_componentIndex] )
            {
                return *static_cast<EcsComponent*>(
                        _entity.mArchetype->GetChunkVector( _componentIndex ).At( _entity.mIndex ) );
            }
            else
            {
                const EcsEntityData& entityData = _entity.mArchetype->GetEntityData( _entity.mIndex );
                return *static_cast<EcsComponent*>( mTransitionArchetype.
                        GetChunkVector( _componentIndex ).At( entityData.mTransitionIndex ) );
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    const EcsComponentInfo& EcsWorld::IndexedGetComponentInfo( const int _componentIndex ) const
    {
        fanAssert( _componentIndex < NumComponents() );
        return mComponentsInfo.at( _componentIndex );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    const EcsComponentInfo& EcsWorld::GetComponentInfo( const uint32_t _type ) const
    {
        return mComponentsInfo.at( GetIndex( _type ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    const EcsComponentInfo* EcsWorld::SafeGetComponentInfo( const uint32_t _type ) const
    {
        auto it = mTypeToIndex.find( _type );
        if( it != mTypeToIndex.end() )
        {
            const int index = it->second;
            return &mComponentsInfo.at( index );
        }
        return nullptr;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    EcsEntity EcsWorld::CreateEntity()
    {
        EcsEntity entityID;
        entityID.mArchetype = &mTransitionArchetype;
        entityID.mIndex     = mTransitionArchetype.Size();

        EcsEntityData entity;
        entity.mTransitionIndex = int( mTransitions.size() );

        EcsTransition transition;
        transition.mEntity = entityID;

        mTransitions.push_back( transition );
        mTransitionArchetype.PushBackEntityData( entity );
        for( int i = 0; i < NumComponents(); i++ )
        {
            mTransitionArchetype.GetChunkVector( i ).EmplaceBack();
        }

        return entityID;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EcsWorld::Kill( const EcsEntity _entity )
    {
        EcsTransition& transition = FindOrCreateTransition( _entity );

        // do not delete twice
        if( !transition.mIsDead )
        {
            transition.mIsDead = true;

            // calls destructor on all components
            for( int i = 0; i < NumComponents(); i++ )
            {
                const EcsSignature& signature = _entity.mArchetype->GetSignature();
                if( signature[i] && !transition.mSignatureRemove[i] )
                {
                    const EcsComponentInfo& info = mComponentsInfo[i];
                    if( info.destroy != nullptr )
                    {
                        EcsComponent& component = *static_cast<EcsComponent*>(
                                _entity.mArchetype->GetChunkVector( i ).At( _entity.mIndex ) );
                        mDestroyedComponents.push_back( { _entity, component, info.destroy } );
                    }
                }
            }
        }
    }

    //==================================================================================================================================================================================================
    // finds all archetypes that match the _include signature
    // archetypes containing types from the _exclude signature are excluded
    //==================================================================================================================================================================================================
    EcsView EcsWorld::Match( const EcsSignature& _include, const EcsSignature& _exclude ) const
    {
        fanAssert( ( _include & _exclude ) == EcsSignature( 0 ) );
        EcsView   view( mTypeToIndex, _include );
        for( auto it = mArchetypes.begin(); it != mArchetypes.end(); ++it )
        {
            const EcsSignature& archetypeSignature = it->first;
            EcsArchetype      * archetype          = it->second;
            if( ( archetypeSignature & _include ) == _include && ( archetypeSignature & _exclude ) == EcsSignature( 0 ) && !archetype->Empty() )
            {
                view.mArchetypes.push_back( archetype );
            }
        }
        return view;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool EcsWorld::IsAlive( const EcsEntity _entity ) const
    {
        const EcsEntityData& entityData = GetEntityData( _entity );
        return entityData.mTransitionIndex < 0 || !mTransitions[entityData.mTransitionIndex].mIsDead;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    EcsArchetype* EcsWorld::FindArchetype( const EcsSignature _signature )
    {
        auto it = mArchetypes.find( _signature );
        return it == mArchetypes.end() ? nullptr : it->second;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    EcsArchetype& EcsWorld::CreateArchetype( const EcsSignature _signature )
    {
        fanAssert( FindArchetype( _signature ) == nullptr );
        EcsArchetype* newArchetype = new EcsArchetype();
        newArchetype->Create( mComponentsInfo, _signature );
        mArchetypes[_signature] = newArchetype;
        return *newArchetype;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    EcsTransition& EcsWorld::FindOrCreateTransition( const EcsEntity _entity )
    {
        // Get/register transition
        EcsEntityData& entity     = _entity.mArchetype->GetEntityData( _entity.mIndex );
        EcsTransition* transition = nullptr;
        if( entity.mTransitionIndex < 0 )
        {
            entity.mTransitionIndex = mTransitionArchetype.Size();
            mTransitions.emplace_back();
            transition = &( *mTransitions.rbegin() );
            transition->mEntity = _entity;

            for( int i = 0; i < NumComponents(); i++ )
            {
                mTransitionArchetype.GetChunkVector( i ).EmplaceBack();
            }
            mTransitionArchetype.PushBackEntityData( {} );
        }
        else
        {
            // transition already exists
            transition = &mTransitions[entity.mTransitionIndex];
        }
        return *transition;
    }
}