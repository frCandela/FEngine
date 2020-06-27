#pragma once

#include "ecs/fanEcsSystem.hpp"
#include "network/singletons/fanHostManager.hpp"
#include "network/components/fanHostReplication.hpp"
#include "network/components/fanEntityReplication.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// Sends a replication packet on all hosts,
	// one host can be excluded using the _excludeHandle parameter
	//==============================================================================================================================================================
	struct S_ReplicateOnAllHosts : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return
				_world.GetSignature<HostReplication>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view, const PacketReplication& _packet, const HostReplication::ReplicationFlags _flags, const EcsHandle _excludeHandle = 0 )
		{
			auto hostReplicationIt = _view.begin<HostReplication>();
			for( ; hostReplicationIt != _view.end<HostReplication>(); ++hostReplicationIt )
			{
				const EcsHandle handle = _world.GetHandle( hostReplicationIt.GetEntity() );
				if( handle != _excludeHandle )
				{
					HostReplication& hostReplication = *hostReplicationIt;
					hostReplication.Replicate( _packet, _flags );
				}
			}
		}
	};

	//==============================================================================================================================================================
	// Replicates all entities that have an EntityReplication component on all hosts
	//==============================================================================================================================================================
	struct S_UpdateReplication : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<EntityReplication>();
		}

		// local stucture used to store an EcsHandle and an HostReplication component
		struct HostManagerHandlePair
		{
			const EcsHandle handle;
			HostReplication& hostReplication;
		};

		static void Run( EcsWorld& _world, const EcsView& _view )
		{
			// Get all HostManager/EcsHandle pairs
			HostManager& hostManager = _world.GetSingleton<HostManager>();
			std::vector< HostManagerHandlePair > hostReplications;
			hostReplications.reserve( hostManager.hostHandles.size() );
			for( const std::pair<HostManager::IPPort, EcsHandle>& pair : hostManager.hostHandles )
			{
				const EcsHandle handle = pair.second;
				HostReplication& hostReplication = _world.GetComponent<HostReplication>( _world.GetEntity( handle ) );
				hostReplications.push_back( { handle, hostReplication }  );
			}

			// Replicates entities on all hosts
			auto replicationIt = _view.begin<EntityReplication>();
			for( ; replicationIt != _view.end<EntityReplication>(); ++replicationIt )
			{
				const EcsEntity entity = replicationIt.GetEntity();
				const EcsHandle handle = _world.GetHandle( entity );
				const EntityReplication& entityReplication = *replicationIt;
				assert( handle != 0 );
				const PacketReplication packet = HostReplication::BuildEntityPacket( _world, handle, entityReplication.componentTypes );
				for( HostManagerHandlePair& pair : hostReplications )
				{
					if( pair.handle != entityReplication.exclude ) // do not replicate on this host
					{
						pair.hostReplication.Replicate( packet, HostReplication::None );
					}
				}
			}
		}
	};
}