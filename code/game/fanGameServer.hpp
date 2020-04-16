#pragma once

#include "game/fanGamePrecompiled.hpp"
#include "ecs/fanEcsWorld.hpp"

#include "network/fanUDPSocket.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	struct GameServer
	{
		GameServer( const std::string _name );

		void Start();
		void Stop();
		void Pause();
		void Resume();
		void Step( const float _delta );

		enum State { WAITING_FOR_PLAYERS = 0, STARTING, PLAYING };

		EcsWorld	world;
		State		state = WAITING_FOR_PLAYERS;

	private:
		void	NetworkSend();
		void	NetworkReceive();

		void OnTestFailure( HostID _client );
		void OnTestSuccess( HostID _client );
	};					  
}
