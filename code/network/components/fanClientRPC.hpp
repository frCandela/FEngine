#pragma once

#include <unordered_map>
#include "ecs/fanEcsComponent.hpp"
#include "network/fanPacket.hpp"
#include "ecs/fanSignal.hpp"

namespace fan
{
	struct SpawnInfo;

	//================================================================================================================================
	// Manages remote procedure calls
	// A remote procedure call is the act of one host causing a	procedure to execute on one or more remote hosts
	// Can generate replication packets to send using the server replication
	//================================================================================================================================	
	struct ClientRPC : public EcsComponent
	{
		ECS_COMPONENT( ClientRPC )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );

		using RpcID = sf::Uint32;
		using RpcUnwrapMethod = void ( ClientRPC::* )( sf::Packet& );
		
		void RegisterUnwrapMethod( const RpcID _rpcID, const RpcUnwrapMethod _unwrapMethod );
		void RegisterRPCs( );
		void TriggerRPC( sf::Packet& _packet );

		std::unordered_map<RpcID , RpcUnwrapMethod > nameToRPCTable;

		// List of available rpc
		Signal < int >			 onShiftFrameIndex;
		static const RpcID		 s_rpcIdShiftFrame = SSID( "RPCShiftFrame" );
		void					 UnwrapShiftClientFrame( sf::Packet& _packet );
		static PacketReplication RPCShiftClientFrame( const int _framesDelta );

		Signal< FrameIndex, sf::Packet >	 onSpawn;
		static const RpcID		 s_rpcIdSpawn = SSID( "RPCSpawn" );
		void					 UnwrapSpawn( sf::Packet& _packet );
		static PacketReplication RPCSpawn( const SpawnInfo& spawnInfo );
	};
}