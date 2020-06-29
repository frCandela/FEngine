#include "editor/windows/fanNetworkWindow.hpp"

#include "game/fanGameServer.hpp"
#include "game/fanGameClient.hpp"
#include "game/singletons/fanGame.hpp"
#include "network/singletons/fanTime.hpp"
#include "network/singletons/fanServerConnection.hpp"

namespace fan
{

	//================================================================================================================================
	//================================================================================================================================
	NetworkWindow::NetworkWindow()
		: EditorWindow( "network", ImGui::IconType::NETWORK16 )
	{}

	//================================================================================================================================
	//================================================================================================================================
	void NetworkWindow::OnGui( EcsWorld& _world )
	{
		Game& game = _world.GetSingleton<Game>();
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