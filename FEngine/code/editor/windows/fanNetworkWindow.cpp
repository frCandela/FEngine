#include "fanGlobalIncludes.h"
#include "editor/windows/fanNetworkWindow.h"

#include "scene/fanScene.h"
#include "game/network/fanGameServer.h"
#include "scene/fanGameobject.h"

namespace fan {

	//================================================================================================================================
	//================================================================================================================================
	NetworkWindow::NetworkWindow( Scene * _clientScene, Scene * _serverScene) 
		: EditorWindow("network", ImGui::IconType::JOYSTICK16 )
		, m_clientScene( _clientScene)
		, m_serverScene( _serverScene )
	{
		
	}

	//================================================================================================================================
	//================================================================================================================================
	void NetworkWindow::OnGui() 
	{



		// Game server
		GameServer * gameServer = m_serverScene->FindComponentOfType<GameServer>();
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
		}	
	}
}