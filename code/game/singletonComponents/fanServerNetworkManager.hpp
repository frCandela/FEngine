#pragma once

#include "ecs/fanSingletonComponent.hpp"

#include "network/fanPacket.hpp"
#include <queue>

namespace fan
{
	class EcsWorld;
	struct SceneNode;
	struct ServerConnectionManager;
	struct LinkingContext;
	struct RPCManager;
	struct Game;
	struct HostConnection;

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

		// pre-get singletons
		ServerConnectionManager*	 connection;
		LinkingContext*				 linkingContext;
		RPCManager*					 rpcManager;
		Game*						 game;

		void Start( EcsWorld& _world );
		void Stop( EcsWorld& _world );
		void Update( EcsWorld& _world );
		void NetworkSend( EcsWorld& _world );
		void NetworkReceive( EcsWorld& _world );
	};
}
