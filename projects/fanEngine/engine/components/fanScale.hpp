#pragma once

#include "ecs/fanEcsComponent.hpp"
#include "core/math/fanVector3.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // scale for objects
    // used in combination with a Transform
    //==================================================================================================================================================================================================
    struct Scale : public EcsComponent
    {
    ECS_COMPONENT( Scale )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
        static void Save( const EcsComponent& _component, Json& _json );
        static void Load( EcsComponent& _component, const Json& _json );

        Vector3 mScale;
    };
}