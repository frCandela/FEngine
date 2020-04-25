#pragma once

#include "ecs/fanSingletonComponent.hpp"

#include "network/fanPacket.hpp"

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

		// pre-get singletons
		DeliveryNotificationManager*	deliveryNotificationManager;
		ClientReplicationManager*		replicationManager;
		ClientConnectionManager*		connection;
		LinkingContext*					linkingContext;
		RPCManager*						rpcManager;
		Game*							game;

		void Start( EcsWorld& _world );
		void Stop( EcsWorld& _world );
		void NetworkSend( EcsWorld& _world );
		void NetworkReceive( EcsWorld& _world );

		void ShiftFrameIndex( const int64_t _framesDelta );
	};
}
