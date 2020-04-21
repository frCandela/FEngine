#include "network/singletonComponents/fanServerReplicationManager.hpp"

#include "scene/fanSceneSerializable.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "game/singletonComponents/fanGame.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( ServerReplicationManager );

	//================================================================================================================================
	//================================================================================================================================
	void ServerReplicationManager::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::NONE;
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
		replicationManager.world = &_world;
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
	//================================================================================================================================
	void ServerReplicationManager::ReplicateSingleton( const uint32_t _staticID, const ReplicationFlags _flags )
	{
		// generates replicated data
		const SingletonComponentInfo& info = world->GetSingletonComponentInfo( _staticID );
		const SingletonComponent& component = world->GetSingletonComponent( _staticID );	
		PacketReplicationSingletonComponents packet;
		packet.Generate( info, component );

		for ( HostData& hostData : hostDatas )
		{
			if( !hostData.isNull )
			{
				hostData.nextReplication.emplace_back();
				SingletonReplicationData& replicationData = hostData.nextReplication[hostData.nextReplication.size() - 1];
				replicationData.staticID = _staticID;
				replicationData.flags = _flags;
				replicationData.packet = packet;
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerReplicationManager::Send( Packet& _packet, const HostID _hostID )
	{
		HostData& hostData = hostDatas[_hostID];
		for( SingletonReplicationData& data : hostData.nextReplication )
		{
			data.packet.Write( _packet );
			if( data.flags & ReplicationFlags::EnsureReplicated )
			{
				hostData.pendingReplication.insert( { _packet.tag , data } );
				_packet.onSuccess.Connect( &ServerReplicationManager::OnReplicationSuccess, this );
				_packet.onFail.Connect( &ServerReplicationManager::OnReplicationFail, this );
				Debug::Log() << "rep send: " << _packet.tag << Debug::Endl();
			}
		}
		hostData.nextReplication.clear();
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void ServerReplicationManager::OnReplicationSuccess( const HostID _hostID, const PacketTag _packetTag )
	{
		HostData& hostData = hostDatas[_hostID];
		hostData.pendingReplication.erase( _packetTag );
		Debug::Highlight() << "success: " << _packetTag << Debug::Endl();
	}
	
	//================================================================================================================================
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

		Debug::Warning() << "fail: " << _packetTag << Debug::Endl();
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
		{
			for (int i = 0; i < replicationManager.hostDatas.size(); i++)
			{
				HostData& data = replicationManager.hostDatas[i];
				if( !data.isNull )
				{
					ImGui::Text( "client %d", i );
					ImGui::Text( "next replication:     %d", data.nextReplication.size() );
					ImGui::Text( "pending replication:  %d", data.pendingReplication.size() );
					ImGui::Spacing(); ImGui::Spacing();
				}
			}
		}
		ImGui::Unindent(); ImGui::Unindent();
	}
}