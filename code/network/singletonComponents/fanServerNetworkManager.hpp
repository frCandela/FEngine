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
			bool isNull = false; // client was deleted
		};

		std::vector<HostData> hostDatas;

		void CreateHost( const HostID _hostID );
		void DeleteHost( const HostID _hostID );
	};
}
