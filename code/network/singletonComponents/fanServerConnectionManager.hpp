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
		enum class State { 
			Null,				// empty client slot
			Disconnected,		// Requires a hello packet from the client to start connection process
			NeedingApprouval,	// Client hello is received, a Login packet must be sent back
			PendingApprouval,	// Login packet was sent, waiting for ack
			Connected			// Login packet was received, client is connected
		};

		HostID		clientId;
		IpAddress	ip;
		Port		port;
		std::string	name = "";
		State		state = State::Null;
		float		roundTripTime = -1.f;
		double		lastResponseTime = 0.f;
		double		lastPingTime = 0.f;
		bool		pingInFlight = false;

		void Clear()
		{

		}
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
		static void OnGui( EcsWorld&, SingletonComponent& _component );

		Signal<HostID> onClientCreated;
		Signal<HostID> onClientDeleted;

		UdpSocket			socket;
		Port				serverPort;
		float				pingDelay;		// send a ping to clients every X seconds
		float				timeoutTime;	// disconnects clients after X seconds without a response
		std::vector<Client> clients;

		HostID	FindClient( const sf::IpAddress _ip, const unsigned short _port );
		HostID	CreateClient( const sf::IpAddress _ip, const unsigned short _port );
		void	DeleteClient( const HostID _clientID );
		void	Send( Packet& _packet, const HostID _clientID );
		void	ProcessPacket( const HostID _clientID, const PacketHello& _packetHello );
		void	DetectClientTimout();

		void OnLoginFail( const HostID _clientID, const PacketTag _packetTag );
		void OnLoginSuccess( const HostID _clientID, const PacketTag _packetTag );
		void OnPingSuccess( const HostID _clientID, const PacketTag _packetTag );
		void OnPingFail( const HostID _clientID, const PacketTag _packetTag );
	};
}