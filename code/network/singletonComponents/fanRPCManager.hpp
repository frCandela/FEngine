#pragma once

#include <unordered_map>

#include "ecs/fanSingletonComponent.hpp"
#include "network/fanPacket.hpp"
#include "core/fanSignal.hpp"


namespace fan
{
	//================================================================================================================================
	// Manages remote procedure calls
	// A remote procedure call is the act of one host causing a	procedure to execute on one or more remote hosts
	//================================================================================================================================	
	struct RPCManager : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void OnGui( EcsWorld&, SingletonComponent& _component );

		using RpcUnwrapFunc = void ( RPCManager::* )( sf::Packet& );
		using RpcId = sf::Uint32;

		void RegisterRPCs( );
		void RegisterUnwrapFunction( const RpcId _id, const RpcUnwrapFunc _rpcUnwrapFunc );
		void TriggerRPC( sf::Packet& _packet );

		std::unordered_map<RpcId , RpcUnwrapFunc > nameToRPCTable;

		// List of available rpc
		void					 UnwrapSynchClientFrame( sf::Packet& _packet );
		static PacketReplication RPCSynchClientFrame( const uint64_t _frameIndex, const float _rtt );
	};
}