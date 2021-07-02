#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "core/resources/fanResourcePtr.hpp"
#include "engine/resources/fanPrefab.hpp"
#include "engine/ui/fanUITransform.hpp"
#include "engine/physics/fanTransform.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct PrefabInstance : public EcsComponent
    {
    ECS_COMPONENT( PrefabInstance )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
        static void Save( const EcsComponent& _component, Json& _json );
        static void Load( EcsComponent& _component, const Json& _json );

        static void CreateOverride( PrefabInstance& _prefabInstance, EcsWorld& _world, EcsEntity _entity );
        static void ApplyOverride( const PrefabInstance& _prefabInstance, EcsWorld& _world, EcsEntity _entity );

        enum OverrideType : int
        {
            HasNone, HasTransform, HasUITransform
        };
        union Override
        {
            Transform   mTransform;
            UITransform mUiTransform;
            Override() {}
            Override( const Override& _other ) { *this = _other; }
        }                   mOverride;
        OverrideType        mOverrideType;
        ResourcePtr<Prefab> mPrefab;
    };
}