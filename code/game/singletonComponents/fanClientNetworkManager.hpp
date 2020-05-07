#pragma once

#include "ecs/fanSingletonComponent.hpp"

#include "network/fanPacket.hpp"
#include <queue>

namespace fan
{
	class EcsWorld;
	struct RPCManager;
	struct ReliabilityLayer;
	struct ClientConnection;
	struct ClientReplication;
	struct LinkingContext;
	struct Game;

	//================================================================================================================================
	//================================================================================================================================
	struct ClientNetworkManager : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void OnGui( EcsWorld&, SingletonComponent& _component );

		FrameIndex		spaceshipSpawnFrameIndex;	// the frame index on which the spaceship is spawned
		NetID			spaceshipNetID;
		EntityHandle	spaceshipHandle;
		EntityHandle	persistentHandle;
		std::queue< PacketInput >			inputs;
		std::queue< PacketPlayerGameState > previousStates;
		bool synced;

		void Start( EcsWorld& _world );
		void Stop( EcsWorld& _world );
		void Update( EcsWorld& _world );
		void NetworkSend( EcsWorld& _world );
		void NetworkReceive( EcsWorld& _world );
		void ProcessPacket( const PacketPlayerGameState& _packet );

		// RPC callbacks
		void OnShiftFrameIndex( const int _framesDelta );
		void OnSpawnShip( NetID _spaceshipID, FrameIndex _frameIndex );
	};
}
