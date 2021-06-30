#pragma once

// remove old macro to remove warning with std::numeric_limits>>::max()
#ifdef max
#undef max
#endif

#include <limits>
#include <type_traits>
#include "SFML/System.hpp"
#include "SFML/Network.hpp"
#include "ecs/fanEcsTypes.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // defines types used in for net code
    //==================================================================================================================================================================================================
    using NetID = sf::Uint32;
    using IpAddress = sf::IpAddress;
    using Port = unsigned short;
    using PacketTag = sf::Uint32;        // may change to uint16 on release
    using PacketTypeInt = sf::Uint8;    // the sf integer type encoding the PacketType enum
    using FrameIndex = sf::Uint32;
    using PlayerID = sf::Uint32;
    using SpawnID = sf::Uint32;

    static_assert( sizeof( PlayerID ) == sizeof( EcsHandle ) );

    // all the types of packets of the network engine
    enum class PacketType
    {
          Ping = 0            // server calculates the RTT &sends it to the client
        , Ack                // packet reception acknowledgment
        , Hello                // first presentation of the client to the server for logging in
        , Disconnect        // packet sent/received when the player disconnects from the server
        , LoggedIn            // server informs client that login was successful
        , Replication        // replication of data on the client's world
        , PlayerInput        // client input ring buffer sent to the server
        , PlayerGameState    // the game state of one player at a specific frame
        , COUNT
    };
    static_assert( (int)PacketType::COUNT < std::numeric_limits<PacketTypeInt>::max() );
}