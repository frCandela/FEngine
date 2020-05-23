#pragma once

#include "ecs/fanEcsComponent.hpp"

#include "network/fanUdpSocket.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	class EcsWorld;

	//================================================================================================================================
	// Manages the connection of the client with the server
	//================================================================================================================================	
	struct ClientConnection : public EcsComponent
	{
		ECS_COMPONENT( ClientConnection )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsComponent& _component );
		static void Destroy ( EcsWorld& _world, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		 



		enum class ClientState { 
			Disconnected,		// Client needs to send a Hello packet to the server to login
			PendingConnection,	// A Hello packet was sent, waiting for LoginSuccess packet from the server
			Connected,			// Client received a LoginSuccess, client is connected
			Stopping			// Client is being stopped, a disconnect packet must be sent to the server
		};
		
		UdpSocket*		socket;
		Port			clientPort;
		sf::IpAddress	serverIP;
		Port			serverPort;
		ClientState		state;
		float			rtt;
		float			timeoutDelay;			// disconnects from server after X seconds without a response
		float			bandwidth;				// in Ko/s
		double			serverLastResponse;

		PacketPing		lastPacketPing;
		bool			mustSendBackPacketPing;

		void Write( Packet& _packet );
		void OnLoginFail( const PacketTag _packetTag );

		void ProcessPacket( const PacketLoginSuccess& _packetLogin );
		void ProcessPacket( const PacketDisconnect& _packetDisconnect );
		void ProcessPacket( const PacketPing& _packetPing, const FrameIndex _frameIndex );
	};
}