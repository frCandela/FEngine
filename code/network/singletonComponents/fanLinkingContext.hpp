#pragma once

#include <unordered_map>

#include "ecs/fanSingletonComponent.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	//================================================================================================================================
	// Links net ID's to entities handles
	//================================================================================================================================	
	struct LinkingContext : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void OnGui( EcsWorld&, SingletonComponent& _component );

		std::unordered_map<NetID, EntityHandle > netIDToEntityHandle;
		std::unordered_map<EntityHandle, NetID > entityHandleToNetID;
		NetID nextNetID;

		void AddEntity( const EntityHandle _entityHandle, const NetID _netID );
		void RemoveEntity( const EntityHandle _entityHandle );
	};
}