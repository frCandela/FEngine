#pragma once

#include "ecs/fanSingletonComponent.hpp"

#include "network/fanPacket.hpp"

namespace fan
{
	class EcsWorld;

	//================================================================================================================================
	// [Client] processes server packets to replicates objects / run events
	//================================================================================================================================
	class ClientReplicationManager : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void OnGui( EcsWorld&, SingletonComponent& _component );
		static void Save( const SingletonComponent& _component, Json& _json );
		static void Load( SingletonComponent& _component, const Json& _json );

		void ProcessPacket( PacketReplicationSingletonComponents& _packet );

		EcsWorld* world = nullptr;
	};
}
