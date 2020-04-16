#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanSingletonComponent.hpp"
#include "network/fanUdpSocket.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	class EcsWorld;	

	//================================================================================================================================
	//================================================================================================================================
	struct Client
	{
		enum class ClientState { 
			Disconnected,		// Requires a hello packet from the client to start connection process
			NeedingApprouval,	// Client hello is received, a Login packet must be sent back
			PendingApprouval,	// Login packet was sent, waiting for ack
			Connected			// Login packet was received, client is connected
		};

		HostID		clientId;
		IpAddress	ip;
		Port		port;
		std::string	name = "";
		ClientState	state = ClientState::Disconnected;
		float		roundTripDelay = 0.f;
		double		lastResponse = 0.f;
	};

	//================================================================================================================================
	// Manages clients connections
	//================================================================================================================================	
	struct ServerConnectionManager : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void OnGui( SingletonComponent& _component );

		UdpSocket			socket;
		Port				serverPort;
		float				pingDuration;		// clients are pinged every X seconds
		float				timeoutDuration;	// clients are disconnected after X seconds
		std::vector<Client> clients;

		HostID	FindClient( const sf::IpAddress _ip, const unsigned short _port );
		HostID	CreateClient( const sf::IpAddress _ip, const unsigned short _port );
		void	Send( Packet& _packet, const HostID _client );
		void	ProcessPacket( const HostID _client, const PacketHello& _packetHello );

		void OnLoginFail( const HostID _clientID );
		void OnLoginSuccess( const HostID _clientID );
	};
}