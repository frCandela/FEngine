#pragma once

#include "ecs/fanEcsSystem.hpp"
#include "network/components/fanHostReplication.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// Sends a replication packet on all hosts
	//==============================================================================================================================================================
	struct S_ReplicateOnAllHosts : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return
				_world.GetSignature<HostReplication>();
		}

		static void Run( EcsWorld& /*_world*/, const EcsView& _view, const PacketReplication& _packet, const HostReplication::ReplicationFlags _flags )
		{
			auto hostReplicationIt = _view.begin<HostReplication>();
			for( ; hostReplicationIt != _view.end<HostReplication>(); ++hostReplicationIt )
			{
				HostReplication& hostReplication = *hostReplicationIt;
				hostReplication.Replicate( _packet, _flags );
			}
		}
	};
}