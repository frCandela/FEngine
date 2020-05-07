#pragma once

#include "ecs/fanSingletonComponent.hpp"
#include "network/fanUdpSocket.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	class EcsWorld;

	//================================================================================================================================
	// Manages the connection of the client with the server
	//================================================================================================================================	
	struct ClientConnection : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void OnGui( EcsWorld&, SingletonComponent& _component );

		enum class ClientState { 
			Disconnected,		// Client needs to send a Hello packet to the server to login
			PendingConnection,	// A Hello packet was sent, waiting for LoginSuccess packet from the server
			Connected			// Client received a LoginSuccess, client is connected
		};
		
		UdpSocket		socket;
		Port			clientPort;
		sf::IpAddress	serverIP;
		Port			serverPort;
		ClientState		state;
		float			rtt;
		float			timeoutTime;			// disconnects from server after X seconds without a response
		float			bandwidth;				// in Ko/s
		double			serverLastResponse;

		PacketPing		lastPacketPing;
		bool			mustSendBackPacketPing;

		void Write( Packet& _packet );
		void OnLoginFail( const PacketTag _packetTag );
		void DetectServerTimout();
		void DisconnectFromServer();

		void ProcessPacket( const PacketLoginSuccess& _packetLogin );
		void ProcessPacket( const PacketPing& _packetPing, const FrameIndex _frameIndex );
	};
}