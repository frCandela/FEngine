#pragma once

#include "core/resources/fanResourcePtr.hpp"
#include "scene/fanPrefab.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
    struct EcsComponent;

    //========================================================================================================
    // holds a pointer to a component of a specific  scene
    // must be initialized with an EcsWorld
    // can be initialized with scene node/component directly or with a sceneNode ID  to be resolved later
    // ( loading, copy/paste )
    //========================================================================================================
    struct ComponentPtrBase
    {
        ComponentPtrBase( uint32_t _type ) : type( _type ) {}
        void Init( EcsWorld& _world ) { world = &_world; }
        void Create( EcsHandle _handle );
        void CreateUnresolved( EcsHandle _handle );
        void Clear();
        bool IsValid() const{ return type != 0 && handle != 0; }
        EcsComponent& operator*() const { return world->GetComponent( world->GetEntity( handle ), type ); }
        EcsComponent* operator->() const { return &( **this ); /* use operator* */ }

        EcsWorld* world;
        const uint32_t type = 0;
        EcsHandle      handle = 0;
    };

    //========================================================================================================
    // Same functionality than ComponentPtrBase but strongly typed
    //========================================================================================================
    template< typename _componentType >
    class ComponentPtr : public ComponentPtrBase
    {
    public:
        ComponentPtr() : ComponentPtrBase( _componentType::Info::sType ) {}
        _componentType& operator*() const
        {
            return static_cast<_componentType&>( world->GetComponent( world->GetEntity( handle ), type ));
        }
        _componentType* operator->() const { return &( **this ); /* use operator* */ }
        bool operator!=( const ComponentPtr<_componentType>& _other ) const { return !( *this == _other ); }
        bool operator==( const ComponentPtr<_componentType>& _other ) const
        {
            return _other.handle == handle;
        }
        bool operator!=( _componentType* _component ) const
        {
            if( _component == nullptr )
            {
                return handle != 0;
            }
            else if( handle == 0 )
            {
                return true;
            }
            return &( *( *this ) ) != _component;
        }
        bool operator==( _componentType* _component ) const
        {
            if( _component == nullptr )
            {
                return handle == 0;
            }
            else if( handle == 0 )
            {
                return false;
            }
            return &( *( *this ) ) == _component;
        }
    };

    //========================================================================================================
    // holds a pointer to a prefab of the prefab resource manager
    // can be initialized with a prefab path to be resolved later
    //========================================================================================================
    class PrefabPtr : public ResourcePtr<Prefab>
    {
    public:
        PrefabPtr( Prefab* _prefab = nullptr ) : ResourcePtr<Prefab>( _prefab ) {}
        void Init( const std::string _path ) { m_path = _path; }
        const std::string& GetPath() const { return m_path; }

        ResourcePtr& Set( Prefab* _resource )
        {
            SetResource( _resource );
            return *this;
        }
    private:
        std::string m_path;
    };
}

//============================================================================================================
// ImGui bindings
//============================================================================================================
namespace ImGui
{
    bool FanPrefab( const char* _label, fan::PrefabPtr& _ptr );
    bool FanComponentBase( const char* _label, fan::ComponentPtrBase& _ptr );
    template< typename _componentType >
    bool FanComponent( const char* _label, fan::ComponentPtr<_componentType>& _ptr )
    {
        static_assert( ( std::is_base_of<fan::EcsComponent, _componentType>::value ) );
        return FanComponentBase( _label, _ptr );
    }
}
