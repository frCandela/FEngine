#pragma once

#include "ecs/fanSingletonComponent.hpp"
#include "network/fanUdpSocket.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	class EcsWorld;	

	//================================================================================================================================
	// Manages server udp socket & some timings
	//================================================================================================================================	
	struct ServerConnectionManager : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void OnGui( EcsWorld&, SingletonComponent& _component );

		UdpSocket			socket;
		Port				serverPort;
		float				pingDelay;		// send a ping to clients every X seconds
		float				timeoutTime;	// disconnects clients after X seconds without a response

		void	Write( EcsWorld& _world, Packet& _packet, const HostID _clientID );
		void	DetectClientTimout( EcsWorld& _world );
	};
}