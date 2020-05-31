#include "editor/windows/fanNetworkWindow.hpp"

#include "game/fanGameServer.hpp"
#include "game/fanGameClient.hpp"
#include "game/singletonComponents/fanGame.hpp"
#include "core/time/fanTime.hpp"
#include "network/singletonComponents/fanServerConnection.hpp"

namespace fan
{

	//================================================================================================================================
	//================================================================================================================================
	NetworkWindow::NetworkWindow( EcsWorld& _world )
		: EditorWindow( "network", ImGui::IconType::NETWORK16 )
		, m_world( &_world )
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void NetworkWindow::OnGui()
	{
		Game& game = m_world->GetSingleton<Game>();
		if( game.gameClient != nullptr )
		{
			OnGuiClient( *game.gameClient ); 
		}
		else
		{
			OnGuiServer( *game.gameServer );
		}
	}	

	//================================================================================================================================
	//================================================================================================================================
	void  NetworkWindow::OnGuiServer( GameServer& /*_gameServer*/ )
	{
	}

	//================================================================================================================================
	//================================================================================================================================
	void  NetworkWindow::OnGuiClient( GameClient& /*_game*/ )
	{
	}
}