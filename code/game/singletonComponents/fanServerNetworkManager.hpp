#pragma once

#include "ecs/fanSingletonComponent.hpp"

#include "network/fanPacket.hpp"
#include <queue>

namespace fan
{
	class EcsWorld;
	struct ServerConnectionManager;
	struct DeliveryNotificationManager;
	struct ServerReplicationManager;
	struct LinkingContext;
	struct RPCManager;
	struct Game;

	//================================================================================================================================
	//================================================================================================================================
	struct ServerNetworkManager : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void OnGui( EcsWorld&, SingletonComponent& _component );
		static void Save( const SingletonComponent& _component, Json& _json );
		static void Load( SingletonComponent& _component, const Json& _json );

		//================================================================
		//================================================================
		struct HostData
		{
			bool   isNull = false;		// client was deleted
			NetID  spaceshipID = 0;
			EntityHandle spaceshipHandle = 0;
			std::queue<PacketInput> inputs;
		};

		std::vector<HostData> hostDatas;

		// pre-get singletons
		ServerConnectionManager*	 connection;
		DeliveryNotificationManager* deliveryNotification;
		ServerReplicationManager*	 replication;
		LinkingContext*				 linkingContext;
		RPCManager*					 rpcManager;
		Game*						 game;

		void Start( EcsWorld& _world );
		void Stop( EcsWorld& _world );
		void Update( EcsWorld& _world );
		void NetworkSend();
		void NetworkReceive();

		void CreateHost( const HostID _hostID );
		void DeleteHost( const HostID _hostID );

		void OnSyncSuccess( HostID _hostID );
	};
}
