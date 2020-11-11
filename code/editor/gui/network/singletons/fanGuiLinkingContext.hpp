#prama once

#include "network/singletons/fanLinkingContext.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void LinkingContext::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mIcon  = ImGui::Link16;
		_info.mGroup = EngineGroups::Network;
		_info.onGui  = &LinkingContext::OnGui;
		_info.mName  = "linking context";
	}

	//========================================================================================================
	//========================================================================================================
	void LinkingContext::OnGui( EcsWorld&, EcsSingleton& _component )
	{
		LinkingContext& linkingContext = static_cast<LinkingContext&>( _component );
		ImGui::Columns( 2 );

		ImGui::Text( "net ID" );		ImGui::NextColumn();
		ImGui::Text( "entity handle" ); ImGui::NextColumn();
		for( std::pair<NetID, EcsHandle> pair : linkingContext.mNetIDToEcsHandle )
		{
			ImGui::Text( "%d", pair.first );
			ImGui::NextColumn();
			ImGui::Text( "%d", pair.second );
			ImGui::NextColumn();
		}
		ImGui::Columns( 1 );
	}
}