#prama once

#include "network/components/fanClientReplication.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void ClientReplication::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon  = ImGui::Network16;
		_info.mGroup = EngineGroups::Network;
		_info.onGui  = &ClientReplication::OnGui;
		_info.mName  = "client replication";
	}

	//========================================================================================================
	//========================================================================================================
	void ClientReplication::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		ClientReplication& replicationManager = static_cast<ClientReplication&>( _component );
		ImGui::Text( "singletons: %d", replicationManager.mReplicationListSingletons.size() );
		ImGui::Text( "rpc:        %d", replicationManager.mReplicationListRPC.size() );
	}
}