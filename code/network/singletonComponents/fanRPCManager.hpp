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

		//================================================================
		//================================================================
		using RPCUnwrapFunc = void ( RPCManager::* )( sf::Packet& );

		void RegisterRPCs();
		void RegisterUnwrapFunction( const uint32_t _id, const RPCUnwrapFunc _RPCUnwrapFunc );

		std::unordered_map<uint32_t, RPCUnwrapFunc > nameToRPCTable;

		Signal<uint64_t, float> OnSynchClientFrame;
		void RPCSynchClientFrame( sf::Packet& _packet, const uint64_t _frameIndex, const float _RTT );
		void UnwrapSynchClientFrame( sf::Packet& _packet );
	};
}