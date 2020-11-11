#prama once

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void ServerNetworkManager::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mIcon  = ImGui::Network16;
		_info.mGroup = EngineGroups::GameNetwork;
		_info.onGui  = &ServerNetworkManager::OnGui;
		_info.mName  = "server network manager";
	}

	//========================================================================================================
	//========================================================================================================
	void ServerNetworkManager::OnGui( EcsWorld& /*_world*/, EcsSingleton& /*_component*/ )
	{
		//ServerNetworkManager& netManager = static_cast<ServerNetworkManager&>( _component );
		ImGui::Text( "Stop looking at me plz" );
	}
}