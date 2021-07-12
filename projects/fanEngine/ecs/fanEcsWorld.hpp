#pragma once

#include <unordered_map>
#include <functional>
#include "core/fanHash.hpp"
#include "core/fanAssert.hpp"
#include "ecs/fanEcsEntity.hpp"
#include "ecs/fanEcsComponent.hpp"
#include "ecs/fanEcsSingleton.hpp"
#include "ecs/fanEcsSystem.hpp"
#include "ecs/fanEcsTag.hpp"
#include "ecs/fanEcsArchetype.hpp"
#include "ecs/fanEcsView.hpp"

namespace fan
{
    struct EcsView;

    //==================================================================================================================================================================================================

    // A transition is created when structural change happen on an entity
    // ( creating / removing components, changing archetype etc )
    //==================================================================================================================================================================================================

    struct EcsTransition
    {
        EcsEntity    mEntity;
        EcsSignature mSignatureAdd    = EcsSignature( 0 );        // bit to 1 means add component
        EcsSignature mSignatureRemove = EcsSignature( 0 );    // bit to 1 means remove component
        bool         mIsDead          = false;
    };

    //==================================================================================================================================================================================================
    // Contains a component and its destructor,
    // destructor is called at the end of the frame before ApplyTransitions
    //==================================================================================================================================================================================================
    struct DestroyedComponent
    {
        EcsEntity mEntity;
        EcsComponent& mComponent;
        void ( * destroy )( EcsWorld&, EcsEntity, EcsComponent& );
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    class EcsWorld
    {
    public:
        EcsWorld() {}
        EcsWorld( EcsWorld const& ) = delete;
        EcsWorld& operator=( EcsWorld const& ) = delete;

        // Global
        void ApplyTransitions();
        void Clear();
        void InitSingletons();
        void PostInitSingletons( const bool _force = false );
        int NumComponents() const { return int( mComponentsInfo.size() ); }
        int NumTags() const { return ecsSignatureLength - mNextTagIndex - 1; }
        int GetFistTagIndex() const { return mNextTagIndex + 1; }

        int GetIndex( const uint32_t _type ) const;
        void ReloadInfos();

        // Handles
        EcsEntity GetEntity( const EcsHandle _handle ) { return mHandles.at( _handle ); }
        EcsHandle AddHandle( const EcsEntity _entity );
        void SetHandle( const EcsEntity _entity, EcsHandle _handle );
        EcsHandle GetHandle( const EcsEntity _entity ) const;
        void RemoveHandle( const EcsEntity _entity );
        EcsHandle GetNextHandle() const { return mNextHandle; }
        void SetNextHandle( const EcsHandle _handle ) { mNextHandle = _handle; }
        bool HandleExists( const EcsHandle _handle ) { return mHandles.find( _handle ) != mHandles.end(); }

        // Singletons
        template< typename _SingletonType >
        void AddSingletonType();
        template< typename _SingletonType >
        _SingletonType& GetSingleton();
        EcsSingleton& GetSingleton( const uint32_t _type ) { return *mSingletons.at( _type ); }
        const EcsSingleton& GetSingleton( const uint32_t _type ) const { return *mSingletons.at( _type ); }
        const EcsSingletonInfo& GetSingletonInfo( const uint32_t _type ) const
        {
            return mSingletonInfos.at( _type );
        }
        const EcsSingletonInfo* SafeGetSingletonInfo( const uint32_t _type ) const;
        std::vector<EcsSingletonInfo> GetVectorSingletonInfo() const;

        // Tags
        template< typename _TagType >
        void AddTagType();
        template< typename _TagType >
        void AddTag( const EcsEntity _entity );
        template< typename _TagType >
        void RemoveTag( const EcsEntity _entity );
        template< typename _TagType >
        bool HasTag( const EcsEntity _entity );
        void AddTag( const EcsEntity _entity, const uint32_t _type );
        void RemoveTag( const EcsEntity _entity, const uint32_t _type );
        bool HasTag( const EcsEntity _entity, const uint32_t _type ) const;
        bool IndexedHasTag( const EcsEntity _entity, const int _tagIndex ) const;
        void AddTagsFromSignature( const EcsEntity _entity, const EcsSignature& _signature );
        const EcsTagInfo& IndexedGetTagInfo( const int _tagIndex ) const;

        // Components
        template< typename _ComponentType >
        void AddComponentType();
        template< typename _ComponentType >
        _ComponentType& AddComponent( const EcsEntity _entity );
        template< typename _ComponentType >
        void RemoveComponent( const EcsEntity _entity );
        template< typename _ComponentType >
        bool HasComponent( const EcsEntity _entity );
        template< typename _ComponentType >
        _ComponentType& GetComponent( const EcsEntity _entity );
        template< typename _ComponentType >
        _ComponentType* SafeGetComponent( const EcsEntity _entity );
        EcsComponent& AddComponent( const EcsEntity _entity, const uint32_t _type );
        void RemoveComponent( const EcsEntity _entity, const uint32_t _type );
        bool HasComponent( const EcsEntity _entity, const uint32_t _type );
        bool IndexedHasComponent( const EcsEntity _entity, const int _componentindex );
        EcsComponent& GetComponent( const EcsEntity _entity, const uint32_t _type );
        EcsComponent* SafeGetComponent( const EcsEntity _entity, const uint32_t _type );
        EcsComponent& IndexedGetComponent( const EcsEntity _entity, const int _componentIndex );
        const EcsComponentInfo& IndexedGetComponentInfo( const int _componentIndex ) const;
        const EcsComponentInfo& GetComponentInfo( const uint32_t _type ) const;
        const EcsComponentInfo* SafeGetComponentInfo( const uint32_t _type ) const;
        const EcsTagInfo* SafeGetTagInfo( const uint32_t _type ) const;
        const std::vector<EcsComponentInfo>& GetComponentInfos() const { return mComponentsInfo; }
        const std::vector<EcsTagInfo>& GetTagsInfos() const { return mTagsInfo; }

        // Entities
        EcsEntity CreateEntity();
        void Kill( const EcsEntity _entity );
        bool IsAlive( const EcsEntity _entity ) const;
        template< class FirstComponent, class... NextComponents >
        EcsSignature GetSignature() const;

        template< typename _SystemType, typename... _Args >
        void Run( _Args&& ... _args );

        template< typename _SystemType, typename... _Args >
        void ForceRun( _Args&& ... _args );

        template< typename... ComponentsOrTags >
        void RunLambda( std::function<void( EcsWorld&, const EcsView& )> _lambda, const EcsSignature& _signatureExclude = EcsSignature( 0 ) );

        template< typename... ComponentsOrTags >
        void ForceRunLambda( std::function<void( EcsWorld&, const EcsView& )> _lambda, const EcsSignature& _signatureExclude = EcsSignature( 0 ) );

        template< typename _SystemType >
        EcsView Match() const;
        EcsView Match( const EcsSignature& _include, const EcsSignature& _exclude = EcsSignature( 0 ) ) const;

        // Const accessors
        const std::unordered_map<EcsHandle, EcsEntity>& GetHandles() const { return mHandles; }
        const std::unordered_map<EcsSignature, EcsArchetype*>& GetArchetypes() const { return mArchetypes; }
        const EcsArchetype& GetTransitionArchetype() const { return mTransitionArchetype; }

    private:
        const EcsEntityData& GetEntityData( const EcsEntity _entity ) const
        {
            return _entity.mArchetype->GetEntityData( _entity.mIndex );
        }
        EcsEntityData& GetEntityData( const EcsEntity _entity )
        {
            return _entity.mArchetype->GetEntityData( _entity.mIndex );
        }
        EcsArchetype* FindArchetype( const EcsSignature _signature );
        EcsArchetype& CreateArchetype( const EcsSignature _signature );
        EcsTransition& FindOrCreateTransition( const EcsEntity _entity );

        EcsHandle                                       mNextHandle   = 1;    // 0 is a null handle
        int                                             mNextTagIndex = ecsSignatureLength - 1;
        EcsSignature                                    mTagsMask;// signature of all tag types combined
        EcsArchetype                                    mTransitionArchetype;
        std::unordered_map<EcsSignature, EcsArchetype*> mArchetypes;
        std::unordered_map<uint32_t, int>               mTypeToIndex;
        std::unordered_map<EcsHandle, EcsEntity>        mHandles;
        std::unordered_map<uint32_t, EcsSingleton*>     mSingletons;
        std::unordered_map<uint32_t, EcsSingletonInfo>  mSingletonInfos;
        std::vector<EcsComponentInfo>                   mComponentsInfo;
        std::vector<EcsTagInfo>                         mTagsInfo;
        std::vector<EcsTransition>                      mTransitions;
        std::vector<DestroyedComponent>                 mDestroyedComponents;
    };
}

#include "ecs/fanEcsWorld.inl"