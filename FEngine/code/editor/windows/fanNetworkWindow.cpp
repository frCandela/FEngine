#include "fanGlobalIncludes.h"
#include "editor/windows/fanNetworkWindow.h"

#include "scene/fanScene.h"
#include "game/network/fanGameServer.h"
#include "scene/fanGameobject.h"

namespace fan {

	//================================================================================================================================
	//================================================================================================================================
	NetworkWindow::NetworkWindow() 
		: EditorWindow("network", ImGui::IconType::JOYSTICK16 )
	{
		
	}

	//================================================================================================================================
	//================================================================================================================================
	void NetworkWindow::OnGui() 
	{
		GameServer * gameServer = m_scene->FindComponentOfType<GameServer>();
		if ( gameServer != nullptr )
		{
			const Server& server = gameServer->GetServer();

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