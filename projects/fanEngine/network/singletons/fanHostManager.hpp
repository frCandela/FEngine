#pragma once

#include "core/ecs/fanEcsSingleton.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
    struct SceneNode;

    //==================================================================================================================================================================================================
    // [SERVER] Manages host creation/destruction & holds references to the hosts data
    //==================================================================================================================================================================================================
    struct HostManager : public EcsSingleton
    {
    ECS_SINGLETON( HostManager )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _component );

        //================================================================
        // Hold an ip & port + defines some operators for storing in a unordered_map
        //================================================================
        struct IPPort
        {
            IpAddress mAdress;
            Port      mPort;
            bool operator==( const IPPort& _ipPort ) const
            {
                return mAdress == _ipPort.mAdress && mPort == _ipPort.mPort;
            }
            size_t operator()( const IPPort& _ipPort ) const
            {
                return size_t( _ipPort.mAdress.toInteger() ) | ( size_t( _ipPort.mPort ) >> 31 );
            }
        };

        std::unordered_map<IPPort, EcsHandle, IPPort> mHostHandles;        // links host ip-port to its entity handle
        EcsHandle                                     mNetRootNodeHandle; // host entity nodes are placed below the net root node

        EcsHandle CreateHost( EcsWorld& _world, const IpAddress _ip, const Port _port );
        void DeleteHost( EcsWorld& _world, const EcsHandle _hostHandle );
        EcsHandle FindHost( const IpAddress _ip, const Port _port );
    };
}