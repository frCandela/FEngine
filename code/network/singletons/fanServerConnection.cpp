#include "network/singletons/fanServerConnection.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void ServerConnection::SetInfo( EcsSingletonInfo& _info )
	{
		_info.icon = ImGui::CONNECTION16;
		_info.group = EngineGroups::Network;
		_info.onGui = &ServerConnection::OnGui;
		_info.name = "server connection";
	}

	//========================================================================================================
	//========================================================================================================
	void ServerConnection::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
		ServerConnection& connection = static_cast<ServerConnection&>( _component );
		connection.serverPort = 53000;
	}

	//========================================================================================================
	//========================================================================================================
	void ServerConnection::OnGui( EcsWorld&, EcsSingleton& _component )
	{
		ServerConnection& connection = static_cast<ServerConnection&>( _component );
		ImGui::Text( "Server" );
		ImGui::Spacing();
		ImGui::Text( "port: %u", connection.serverPort );
	}
}