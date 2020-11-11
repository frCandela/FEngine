#prama once

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void ClientNetworkManager::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mIcon  = ImGui::ClientNet16;
		_info.mGroup = EngineGroups::GameNetwork;
		_info.onGui  = &ClientNetworkManager::OnGui;
		_info.mName  = "client network manager";
	}

	//========================================================================================================
	//========================================================================================================
	void ClientNetworkManager::OnGui( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
		ClientNetworkManager& netManager = static_cast<ClientNetworkManager&>( _component );
		ImGui::Text( "persistent handle : %d", netManager.mPersistentHandle );
	}
}