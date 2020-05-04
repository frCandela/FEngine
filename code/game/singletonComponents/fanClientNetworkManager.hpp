#pragma once

#include "ecs/fanSingletonComponent.hpp"

#include "network/fanPacket.hpp"
#include <queue>

namespace fan
{
	class EcsWorld;
	struct RPCManager;
	struct DeliveryNotificationManager;
	struct ClientConnectionManager;
	struct ClientReplicationManager;
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
		static void Save( const SingletonComponent& _component, Json& _json );
		static void Load( SingletonComponent& _component, const Json& _json );

		FrameIndex spaceshipSpawnFrameIndex;	// the frame index on which the spaceship is spawned
		NetID	 spaceshipNetID;
		EntityHandle spaceshipHandle;

		 std::queue< PacketInput > inputs;
		 bool synced;

		// pre-get singletons
		DeliveryNotificationManager*	deliveryNotificationManager;
		ClientReplicationManager*		replicationManager;
		ClientConnectionManager*		connection;
		LinkingContext*					linkingContext;
		RPCManager*						rpcManager;
		Game*							game;

		void Start( EcsWorld& _world );
		void Stop( EcsWorld& _world );
		void Update( EcsWorld& _world );
		void NetworkSend();
		void NetworkReceive();

		// RPC callbacks
		void ShiftFrameIndex( const int _framesDelta );
		void SpawnShip( NetID _spaceshipID, FrameIndex _frameIndex );
	};
}
