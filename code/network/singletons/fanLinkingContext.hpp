#pragma once

#include <unordered_map>

#include "ecs/fanEcsSingleton.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	//================================================================================================================================
	// Links net ID's to entities handles
	//================================================================================================================================	
	struct LinkingContext : public EcsSingleton
	{
		ECS_SINGLETON( LinkingContext )
	public:
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );
		static void OnGui( EcsWorld&, EcsSingleton& _component );

		std::unordered_map<NetID, EcsHandle > netIDToEcsHandle;
		std::unordered_map<EcsHandle, NetID > EcsHandleToNetID;
		NetID nextNetID;

		void AddEntity( const EcsHandle _handle, const NetID _netID );
		void RemoveEntity( const EcsHandle _handle );
	};
}