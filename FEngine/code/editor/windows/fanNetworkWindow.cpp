#include "fanGlobalIncludes.h"
#include "editor/windows/fanNetworkWindow.h"

#include "scene/fanScene.h"
#include "game/network/fanGameServer.h"

namespace fan {

	//================================================================================================================================
	//================================================================================================================================
	NetworkWindow::NetworkWindow() 
		: EditorWindow("network", ImGui::IconType::JOYSTICK16 )
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void NetworkWindow::OnSceneLoad( Scene * _scene )
	{
		m_scene = _scene;
		m_gameServer = _scene->FindComponentOfType<GameServer>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void NetworkWindow::OnSceneClear()
	{
		m_scene = nullptr;
		m_gameServer = nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	void NetworkWindow::OnGui() 
	{
		if ( m_gameServer != nullptr )
		{
			const Server& server = m_gameServer->GetServer();

			ImGui::Separator();
			const std::vector<Server::ClientData >& clientsList = server.GetClients();
			for ( int clientIndex = 0; clientIndex < clientsList.size(); clientIndex++ )
			{
				const Server::ClientData& client = clientsList[clientIndex];
				ImGui::Text( client.name.c_str() );
				ImGui::Text( "ping: %d", (int)( 1000.f * client.ping ) );
				ImGui::Text( "last response: %.1f", client.lastResponse );
				ImGui::Separator();
			}
		}	}

	//================================================================================================================================
	//================================================================================================================================
	void NetworkWindow::Update( const float /*_delta*/ )
	{
// 		m_client1.Update(_delta);	
// 		m_server.Update(_delta);	
	}
}