#pragma once

#include "core/resources/fanResourcePtr.hpp"
#include "core/ecs/fanEcsWorld.hpp"
#include "fanPrefab.hpp"

namespace fan
{
    struct EcsComponent;

    //==================================================================================================================================================================================================
    // holds a pointer to a component of a specific  scene
    // must be initialized with an EcsWorld
    // can be initialized with scene node/component directly or with a sceneNode ID  to be resolved later
    // ( loading, copy/paste )
    //==================================================================================================================================================================================================
    struct ComponentPtrBase
    {
        ComponentPtrBase( uint32_t _type ) : mType( _type ) {}
        void Init( EcsWorld& _world )
        {
            mWorld  = &_world;
            mHandle = 0;
        }
        void Create( EcsHandle _handle );
        void CreateUnresolved( EcsHandle _handle );
        void Clear();
        bool IsValid() const { return mType != 0 && mHandle != 0; }
        EcsComponent& operator*() const { return mWorld->GetComponent( mWorld->GetEntity( mHandle ), mType ); }
        EcsComponent* operator->() const { return &( **this ); /* use operator* */ }

        EcsWorld* mWorld;
        const uint32_t mType   = 0;
        EcsHandle      mHandle = 0;
    };

    //==================================================================================================================================================================================================
    // Same functionality than ComponentPtrBase but strongly typed
    //==================================================================================================================================================================================================
    template< typename _componentType >
    class ComponentPtr : public ComponentPtrBase
    {
    public:
        ComponentPtr() : ComponentPtrBase( _componentType::Info::sType ) {}
        _componentType& operator*() const
        {
            return static_cast<_componentType&>( mWorld->GetComponent( mWorld->GetEntity( mHandle ), mType ));
        }
        _componentType* operator->() const { return &( **this ); /* use operator* */ }
        bool operator!=( const ComponentPtr<_componentType>& _other ) const { return !( *this == _other ); }
        bool operator==( const ComponentPtr<_componentType>& _other ) const
        {
            return _other.mHandle == mHandle;
        }
        bool operator!=( _componentType* _component ) const
        {
            if( _component == nullptr )
            {
                return mHandle != 0;
            }
            else if( mHandle == 0 )
            {
                return true;
            }
            return &( *( *this ) ) != _component;
        }
        bool operator==( _componentType* _component ) const
        {
            if( _component == nullptr )
            {
                return mHandle == 0;
            }
            else if( mHandle == 0 )
            {
                return false;
            }
            return &( *( *this ) ) == _component;
        }
    };
}



