#pragma once

#include "core/ecs/fanEcsComponent.hpp"

#include <deque>
#include "network/fanPacket.hpp"

namespace fan
{
    class EcsWorld;

    //==================================================================================================================================================================================================
    // Save states of the entity for rolling back the simulation
    //==================================================================================================================================================================================================
    struct ClientRollback : public EcsComponent
    {
    ECS_COMPONENT( ClientRollback )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
        static void Save( const EcsComponent& _component, Json& _json );
        static void Load( EcsComponent& _component, const Json& _json );

        //================================================================
        // Holds the rollback data of one component at a secific frame
        //================================================================
        struct RollbackData
        {
            FrameIndex mFrameIndex;
            int        mComponentIndex;
            sf::Packet mData;
        };
        std::deque<RollbackData> mRollbackDatas;
        static const int         sRollbackDatasMaxSize = 100;
    };
}