#pragma once

#include "ecs/fanEcsSingleton.hpp"
#include "network/fanUdpSocket.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
    class EcsWorld;

    //==================================================================================================================================================================================================
    // Manages server udp socket & some timings
    //==================================================================================================================================================================================================
    struct ServerConnection : public EcsSingleton
    {
    ECS_SINGLETON( ServerConnection )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _component );

        UdpSocket mSocket;
        Port      mServerPort;
    };
}