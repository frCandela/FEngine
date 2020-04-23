#pragma once

#include "ecs/fanSingletonComponent.hpp"

#include "network/fanPacket.hpp"

namespace fan
{
	class EcsWorld;

	//================================================================================================================================
	// [Server]
	//================================================================================================================================
	class ServerNetworkManager : public SingletonComponent
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
			bool isNull = false;		// client was deleted
			double lastSync = -100.f; // last time the client frame was synced with the server
		};

		std::vector<HostData> hostDatas;
		double				  syncInterval; // the delay between two clients sync 

		void CreateHost( const HostID _hostID );
		void DeleteHost( const HostID _hostID );

		void Update( EcsWorld& _world, const HostID _hostID );
	};
}
