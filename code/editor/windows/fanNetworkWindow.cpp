#include "editor/windows/fanNetworkWindow.hpp"

#include "game/fanGameServer.hpp"
#include "game/fanGameClient.hpp"
#include "game/singletonComponents/fanGame.hpp"
#include "core/time/fanTime.hpp"

namespace fan
{

	//================================================================================================================================
	//================================================================================================================================
	NetworkWindow::NetworkWindow( EcsWorld& _world )
		: EditorWindow( "network", ImGui::IconType::JOYSTICK16 )
		, m_world( &_world )
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void NetworkWindow::OnGui()
	{
		Game& game = m_world->GetSingletonComponent<Game>();
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
	void  NetworkWindow::OnGuiServer( GameServer& _gameServer )
	{
		double currentTime = Time::Get().ElapsedSinceStartup();

		Game& game = m_world->GetSingletonComponent<Game>();

		ImGui::Text( "Server" );
		ImGui::Separator();
		ImGui::Text( "port: %u", _gameServer.serverPort );
		ImGui::DragFloat( "ping duration", &_gameServer.pingDuration, 0.1f, 0.f, 10.f );
		ImGui::DragFloat( "timeout duration", &_gameServer.timeoutDuration, 0.1f, 0.f, 10.f );
		if( ImGui::Button( "Start" ) && _gameServer.state == GameServer::WAITING_FOR_PLAYERS )
		{ 
			_gameServer.state = GameServer::STARTING;
			game.frameStart = game.frameIndex + 180; 
		}

		// draw all clients
		if( ImGui::CollapsingHeader( "clients" ) )
		{
			for( int i = (int)_gameServer.clients.size() - 1; i >= 0; i-- )
			{
				Client& client = _gameServer.clients[i];

				ImGui::Text( "name           %s", client.name.c_str() );
				ImGui::Text( "adress         %s::%u", client.ip.toString().c_str(), client.port );
				ImGui::Text( "state          %s", StateToString( client.state ).c_str() );
				ImGui::Text( "ping           %.01f", .5f * 1000.f * client.roundTripDelay );
				ImGui::Text( "last response  %.1f", currentTime - client.lastResponse );

				ImGui::Spacing();
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  NetworkWindow::OnGuiClient( GameClient& _game )
	{
		double currentTime = Time::Get().ElapsedSinceStartup();

		ImGui::Text( "Client" );
		ImGui::Separator();
		ImGui::DragFloat( "timeout duration",		&_game.timeoutDuration, 0.1f, 0.f, 10.f );
		ImGui::Text( "client port           %u",	_game.clientPort );
		ImGui::Text( "server adress         %s::%u", _game.serverIP.toString().c_str(), _game.serverPort );
		ImGui::Text( "state                 %s",	StateToString( _game.state ).c_str() );
		ImGui::Text( "server last response  %.1f",	currentTime - _game.serverLastResponse );
		ImGui::Text( "ping                  %.01f", .5f * 1000.f * _game.roundTripDelay );
	}

	//================================================================================================================================
	// helper to draw the game server's client state to string
	//================================================================================================================================
	std::string NetworkWindow::StateToString( const Client::State _state )
	{
		switch( _state )
		{
		case Client::CONNECTING:		  return "CONNECTING";
		case Client::CONNECTED_NEED_ACK:  return "CONNECTED_NEED_ACK";
		case Client::CONNECTED:			  return "CONNECTED";
		case Client::STARTING:			  return "STARTING";
		case Client::PLAYING:			  return "PLAYING";
		default: return "<ERROR>";
		}
	}

	//================================================================================================================================
	// helper to draw the game client status to string
	//================================================================================================================================
	std::string NetworkWindow::StateToString( const GameClient::State _status )
	{
		switch( _status )
		{
		case GameClient::DISCONNECTED:	return "DISCONNECTED";
		case GameClient::CONNECTED:		return "CONNECTED";
		case GameClient::STARTING:		return "STARTING";			
		default: assert( false ); return "";
		}
	}
}