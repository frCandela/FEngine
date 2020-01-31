#include "editor/windows/fanNetworkWindow.hpp"
#include "game/network/fanGameServer.hpp"
#include "game/network/fanGameClient.hpp"
#include "scene/fanGameobject.hpp"
#include "scene/fanScene.hpp"

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
		if ( ImGui::CollapsingHeader( "Server" ) )
		{
			// Game server
			GameServer * gameServer = m_serverScene->FindComponentOfType<GameServer>();
			if ( gameServer != nullptr )
			{
				//const UDPSocket& socket = gameServer->GetSocket();

				ImGui::Separator();
				const std::vector<GameServer::ClientData >& clientsList = gameServer->GetClients();
				for ( int clientIndex = 0; clientIndex < clientsList.size(); clientIndex++ )
				{
					const GameServer::ClientData& client = clientsList[clientIndex];
					ImGui::Text( client.name.c_str() );
					ImGui::Text( "ping: %d", (int)( 1000.f * client.ping ) );
					ImGui::Text( "last response: %.1f", client.lastResponse );
					ImGui::Separator();
				}
			}
		}

		if ( ImGui::CollapsingHeader( "Client" ) )
		{
			// Game client
			GameClient * gameClient = m_clientScene->FindComponentOfType<GameClient>();
			if ( gameClient != nullptr )
			{
				ImGui::Separator();
				const std::vector<GameClient::NetPlayerData >& netPlayers = gameClient->GetNetPlayers();
				for ( int playerIndex = 0; playerIndex < netPlayers.size(); playerIndex++ )
				{
					const GameClient::NetPlayerData& netPlayer = netPlayers[playerIndex];
					ImGui::Text( netPlayer.playerPersistent->GetName().c_str() );
					ImGui::Text( "state: %d", (int)( netPlayer.state ) );
					ImGui::Separator();
				}
			}
		}
 	}
}