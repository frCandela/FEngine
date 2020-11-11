#prama once

#include "network/singletons/fanHostManager.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void HostManager::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mIcon  = ImGui::Network16;
		_info.mGroup = EngineGroups::Network;
		_info.onGui  = &HostManager::OnGui;
		_info.mName  = "host manager";
	}

	//========================================================================================================
	//========================================================================================================
	void HostManager::OnGui( EcsWorld&, EcsSingleton& _component )
	{
        HostManager& hostManager = static_cast<HostManager&>( _component );

        ImGui::Columns( 3 );

        ImGui::Text( "name" );
        ImGui::NextColumn();
        ImGui::Text( "ip" );
        ImGui::NextColumn();
        ImGui::Text( "port" );
        ImGui::NextColumn();
        for( const std::pair<IPPort, EcsHandle>& pair : hostManager.mHostHandles )
        {
            ImGui::Text( "host%d", pair.second );
            ImGui::NextColumn();
            ImGui::Text( "%s", pair.first.mAdress.toString().c_str() );
            ImGui::NextColumn();
            ImGui::Text( "%d", pair.first.mPort );
            ImGui::NextColumn();
        }
        ImGui::Columns( 1 );
	}
}