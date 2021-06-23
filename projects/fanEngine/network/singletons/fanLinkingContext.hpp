#pragma once

#include <unordered_map>

#include "ecs/fanEcsSingleton.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // Links net ID's to entities handles
    //==================================================================================================================================================================================================
    struct LinkingContext : public EcsSingleton
    {
    ECS_SINGLETON( LinkingContext )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _component );

        std::unordered_map<NetID, EcsHandle> mNetIDToEcsHandle;
        std::unordered_map<EcsHandle, NetID> mEcsHandleToNetID;
        NetID                                mNextNetID;

        void AddEntity( const EcsHandle _handle, const NetID _netID );
        void RemoveEntity( const EcsHandle _handle );
    };
}