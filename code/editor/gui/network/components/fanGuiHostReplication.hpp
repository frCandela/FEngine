#prama once

#include "network/components/fanHostReplication.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void HostReplication::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon  = ImGui::IconType::Network16;
		_info.mGroup = EngineGroups::Network;
		_info.onGui  = &HostReplication::OnGui;
		_info.mName  = "host replication";
	}

	//========================================================================================================
	//========================================================================================================
	void HostReplication::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		HostReplication& hostReplication = static_cast<HostReplication&>( _component );
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
		{
			ImGui::Text( "next replication:    %d", hostReplication.mNextReplication.size() );
			ImGui::Text( "pending replication: %d", hostReplication.mPendingReplication.size() );
		} 
		ImGui::PopItemWidth();
	}
}