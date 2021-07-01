#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "core/resources/fanResourcePtr.hpp"
#include "engine/resources/fanPrefab.hpp"

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

        ResourcePtr<Prefab> mPrefab;
    };
}