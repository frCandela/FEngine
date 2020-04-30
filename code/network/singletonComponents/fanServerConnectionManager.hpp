#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanSingletonComponent.hpp"
#include "network/fanUdpSocket.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	class EcsWorld;	

	//================================================================
	//================================================================
	struct Client
	{
		enum class State
		{
			Null,				// empty client slot
			Disconnected,		// Requires a hello packet from the client to start connection process
			NeedingApprouval,	// Client hello is received, a Login packet must be sent back
			PendingApprouval,	// Login packet was sent, waiting for ack
			Connected			// Login packet was received, client is connected
		};

		HostID		hostId;
		IpAddress	ip;
		Port		port;
		std::string	name = "";
		State		state = State::Null;
		double		lastResponseTime = 0.f;		// last time the client answered back
		double		lastPingTime = 0.f;			// last time the client was sent a ping
		float		rtt = -1.f;

		// client frame index synchronization
		bool				synced = false;		// true if the client has been synced 
		double				lastSync = 0.f;		// client frame index value is correct
		std::array<int, 5>  framesDelta;		// server-client frame index delta in the N previous frames
		int					nextDeltaIndex = 0; // next delta to update in the array
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
		void	Send( Packet& _packet, const HostID _clientID, const FrameIndex _frameIndex );

		void	ProcessPacket( const HostID _clientID, const PacketHello& _packetHello );
		void	ProcessPacket( const HostID _clientID, const PacketPing& _packetPing, const FrameIndex _frameIndex, const float _logicDelta );
		void	DetectClientTimout();

		void OnLoginFail( const HostID _clientID, const PacketTag _packetTag );
		void OnLoginSuccess( const HostID _clientID, const PacketTag _packetTag );
	};
}