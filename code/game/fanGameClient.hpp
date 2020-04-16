#pragma once

#include "scene/fanScenePrecompiled.hpp"
#include "ecs/fanEcsWorld.hpp"

#include "network/fanUdpSocket.hpp"

namespace fan
{
	//================================================================================================================================
	// 
	//================================================================================================================================
	struct GameClient
	{
		GameClient( const std::string _name );

		void Start();
		void Stop();
		void Pause();
		void Resume();
		void Step( const float _delta );

		enum RandomFlags { MUST_PING_SERVER = 1 << 0, MUST_ACK_START = 1 << 1 };

		EcsWorld		world;

	private:
		void	NetworkSend();
		void	NetworkReceive();

		void OnTestFailure( HostID _client );
		void OnTestSuccess( HostID _client );
	};
}
