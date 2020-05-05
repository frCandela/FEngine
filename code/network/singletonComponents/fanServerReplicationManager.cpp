#include "network/singletonComponents/fanServerReplicationManager.hpp"

#include "scene/fanSceneSerializable.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( ServerReplicationManager );

	//================================================================================================================================
	//================================================================================================================================
	void ServerReplicationManager::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.init = &ServerReplicationManager::Init;
		_info.onGui = &ServerReplicationManager::OnGui;
		_info.save = &ServerReplicationManager::Save;
		_info.load = &ServerReplicationManager::Load;
		_info.name = "replication manager";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerReplicationManager::Init( EcsWorld& _world, SingletonComponent& _component )
	{
		ServerReplicationManager& replicationManager = static_cast<ServerReplicationManager&>( _component );
		replicationManager.hostDatas.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerReplicationManager::CreateHost( const HostID _hostID )
	{
		if( _hostID >= hostDatas.size() )
		{
			hostDatas.resize( _hostID + 1 );
		}

		HostData& hostData = hostDatas[_hostID];
		hostData = {};
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerReplicationManager::DeleteHost( const HostID _hostID )
	{
		hostDatas[_hostID].isNull = true;
	}

	//================================================================================================================================
	// Replicates data on all connected hosts
	// returns a success signal that the caller can connect to to get notified of the acknowledgments 
	// ( ResendUntilReplicated flag must be on )
	//================================================================================================================================
	Signal<HostID>* ServerReplicationManager::ReplicateOnClient( const HostID _hostID, PacketReplication& _packet, const ReplicationFlags _flags )
	{
		HostData& hostData = hostDatas[_hostID];
		if( !hostData.isNull )
		{
			hostData.nextReplication.emplace_back();
			SingletonReplicationData& replicationData = hostData.nextReplication[hostData.nextReplication.size() - 1];
			replicationData.flags = _flags;
			replicationData.packet = _packet;
			return &replicationData.onSuccess;
		}
		return nullptr;
	}

	//================================================================================================================================
	// Replicates data on all connected hosts
	//================================================================================================================================
	void ServerReplicationManager::ReplicateOnAllClients( PacketReplication& _packet, const ReplicationFlags _flags )
	{
		for ( HostID i = 0; i < hostDatas.size(); i++)
		{
			ReplicateOnClient( i, _packet, _flags );
		}
	}

	//================================================================================================================================
	// Builds & returns a replication packet to replicate an ecs singleton
	//================================================================================================================================
	PacketReplication ServerReplicationManager::BuildSingletonPacket( const EcsWorld& _world, const uint32_t _staticID )
	{
		// generates replicated data for a singleton component
		const SingletonComponentInfo& info = _world.GetSingletonComponentInfo( _staticID );
		const SingletonComponent& component = _world.GetSingletonComponent( _staticID );
		PacketReplication packet;
		packet.replicationType = PacketReplication::ReplicationType::SingletonComponent;
		packet.packetData.clear();
		packet.packetData << sf::Uint32( info.staticIndex );
		info.netSave( component, packet.packetData );

		return packet;
	}

	//================================================================================================================================
	// Builds & returns a replication packet for running RPC 
	//================================================================================================================================
	PacketReplication ServerReplicationManager::BuildRPCPacket( sf::Packet& _dataRPC )
	{
		PacketReplication packet;
		packet.replicationType = PacketReplication::ReplicationType::RPC;
		packet.packetData.clear();
		packet.packetData = _dataRPC;

		return packet;
	}

	//================================================================================================================================
	// Sends all new replication packed
	//================================================================================================================================
	void ServerReplicationManager::Write( Packet& _packet, const HostID _hostID )
	{
		HostData& hostData = hostDatas[_hostID];
		for( SingletonReplicationData& data : hostData.nextReplication )
		{
			data.packet.Write( _packet );
			if( data.flags & ReplicationFlags::ResendUntilReplicated )
			{
				hostData.pendingReplication.insert( { _packet.tag , data } );
				_packet.onSuccess.Connect( &ServerReplicationManager::OnReplicationSuccess, this );
				_packet.onFail.Connect( &ServerReplicationManager::OnReplicationFail, this );
				//Debug::Log() << "rep send: " << _packet.tag << Debug::Endl();
			}
		}
		hostData.nextReplication.clear();
	}
	
	//================================================================================================================================
	// Replication packet has arrived, remove from pending list
	//================================================================================================================================
	void ServerReplicationManager::OnReplicationSuccess( const HostID _hostID, const PacketTag _packetTag )
	{
		HostData& hostData = hostDatas[_hostID];
		using MMapIt = std::multimap< PacketTag, SingletonReplicationData>::iterator;
		std::pair<MMapIt, MMapIt> result = hostData.pendingReplication.equal_range( _packetTag );
		for( MMapIt it = result.first; it != result.second; it++ )
		{
			SingletonReplicationData& data = it->second;
			data.onSuccess.Emmit( _hostID );
		}
		hostData.pendingReplication.erase( _packetTag );
		//Debug::Highlight() << "success: " << _packetTag << Debug::Endl();
	}
	
	//================================================================================================================================
	// Replication packet has dropped, resends it if necessary
	//================================================================================================================================
	void ServerReplicationManager::OnReplicationFail( const HostID _hostID, const PacketTag _packetTag )
	{
		HostData& hostData = hostDatas[_hostID];
		using MMapIt = std::multimap< PacketTag, SingletonReplicationData>::iterator;
		std::pair<MMapIt, MMapIt> result = hostData.pendingReplication.equal_range( _packetTag );
		for( MMapIt it = result.first; it != result.second; it++ )
		{
			SingletonReplicationData& data = it->second;
			hostData.nextReplication.push_back( data );
		}
		hostData.pendingReplication.erase( _packetTag );

		//Debug::Warning() << "fail: " << _packetTag << Debug::Endl();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerReplicationManager::Save( const SingletonComponent& _component, Json& _json )
	{
		const ServerReplicationManager& replicationManager = static_cast<const ServerReplicationManager&>( _component );

	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerReplicationManager::Load( SingletonComponent& _component, const Json& _json )
	{
		ServerReplicationManager& replicationManager = static_cast<ServerReplicationManager&>( _component );

	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerReplicationManager::OnGui( EcsWorld&, SingletonComponent& _component )
	{
		ServerReplicationManager& replicationManager = static_cast<ServerReplicationManager&>( _component );

		ImGui::Indent(); ImGui::Indent();
		ImGui::Columns( 3 );

		ImGui::Text( "id" );					ImGui::NextColumn();
		ImGui::Text( "next replication" );		ImGui::NextColumn();
		ImGui::Text( "pending replication" );	ImGui::NextColumn();

		{
			for (int i = 0; i < replicationManager.hostDatas.size(); i++)
			{
				HostData& data = replicationManager.hostDatas[i];
				if( !data.isNull )
				{
					ImGui::Text( "%d", i ); 
					ImGui::NextColumn();

					ImGui::Text( "%d", data.nextReplication.size() );
					ImGui::NextColumn();

					ImGui::Text( "%d", data.pendingReplication.size() );
					ImGui::NextColumn();

				}
			}
		}

		ImGui::Columns( 1 );
		ImGui::Unindent(); ImGui::Unindent();
	}
}