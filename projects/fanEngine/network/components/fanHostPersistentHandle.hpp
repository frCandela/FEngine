#pragma once

#include "core/ecs/fanEcsComponent.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // Contains the handle of the player network persistent entity
    // Is placed on the player spaceship
    //==================================================================================================================================================================================================
    struct HostPersistentHandle : public EcsComponent
    {
    ECS_COMPONENT( HostPersistentHandle )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );

        EcsHandle mHandle;
    };
}