#pragma once

#include "ecs/fanEcsComponent.hpp"

namespace fan
{
    class EcsWorld;

    //==================================================================================================================================================================================================
    // All entities that have an EntityReplication component will be saved
    // in a packet using NetSave / NetLoad methods.
    // Then, data will be replicated on other hosts
    //==================================================================================================================================================================================================
    struct EntityReplication : public EcsComponent
    {
    ECS_COMPONENT( EntityReplication )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );

        std::vector<uint32_t> mComponentTypes;
        EcsHandle             mExclude; // host handle to exclude for replication
    };
}