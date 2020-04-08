#include "game/network/fanOldGameClient.hpp"

#include "network/packets/fanIPacket.hpp"
#include "network/packets/fanPacketLogin.hpp"
#include "core/input/fanInputManager.hpp"
#include "core/input/fanInput.hpp"
#include "core/input/fanMouse.hpp"
#include "core/input/fanJoystick.hpp"

namespace fan
{

		//================================================================================================================================
		//================================================================================================================================
		void OldGameClient::OnAttach()
	{
		
		//m_socket.Create( "[unknown]", 53001 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void OldGameClient::OnDetach()
	{

		//m_socket.UnBind();
	}

	//================================================================================================================================
	//================================================================================================================================
	void OldGameClient::Start()
	{
// 		// Remove this if we are on a server scene HACK?
// 		if ( GetScene().IsServer() )
// 		{
// 			GetScene().DeleteComponent( this );
// 			return;
// 		}
// 
// 		REQUIRE_COMPONENT( PlayersManager, m_playersManager );
// 		REQUIRE_COMPONENT( GameManager, m_gameManager );
// 
// 		// Bind & Log to server
// 		while ( !m_socket.Bind() )
// 		{
// 			m_socket.SetPort( m_socket.GetPort() + 1 );
// 		}
// 		ConnectToServer( 53000, "127.0.0.1" );
// 
// 		// Log players & register new players callbacks
// 		if ( m_playersManager != nullptr )
// 		{
// 			m_playersManager->onAddPlayer.Connect( &GameClient::OnAddPlayer, this );
// 			std::vector< Gameobject* > players = m_playersManager->GetPlayers();
// 			for ( int playerIndex = 0; playerIndex < players.size(); playerIndex++ )
// 			{
// 				OnAddPlayer( players[ playerIndex ] );
// 			}
// 		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void OldGameClient::Stop()
	{
// 		m_socket.UnBind();
// 		m_netPlayers.clear();
// 
// 		m_playersManager->onAddPlayer.Disconnect( &GameClient::OnAddPlayer, this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void OldGameClient::Update( const float _delta )
	{
// 		m_timer -= _delta;
// 
// 		Receive();
// 
// 		switch ( m_state )
// 		{
// 		case ClientState::CLIENT_NONE:
// 			break;
// 		case ClientState::CLIENT_CONNECTING:
// 			if ( m_timer < 0.f )
// 			{
// 				Debug::Log() << m_socket.GetName() << " attempting connection to server" << Debug::Endl();
// 				PacketLogin packet( m_socket.GetName() );
// 				m_socket.Send( packet.ToPacket(), m_serverIp, m_serverPort );
// 				m_timer = 0.5f;
// 			}
// 			break;
// 		case ClientState::CLIENT_CONNECTED:
// 			break;
// 
// 		default:
// 			break;
// 		}

	}

	//================================================================================================================================
	//================================================================================================================================
	void OldGameClient::LateUpdate( const float /*_delta*/ ) {}

// 	//================================================================================================================================
// 	//================================================================================================================================
// 	void GameClient::OnAddPlayer( Gameobject* _playerPersistent )
// 	{
// 		assert( FindNetPlayer( _playerPersistent ) == nullptr );
// 		NetPlayerData playerData;
// 		playerData.playerPersistent = _playerPersistent;
// 		playerData.state = PLAYER_CONNECTING;
// 		m_netPlayers.push_back( playerData );
//	}

// 	//================================================================================================================================
// 	//================================================================================================================================
// 	void GameClient::Receive()
// 	{
// 		bool disconnected = false;
// 		sf::Packet packet;
// 		while ( m_socket.Receive( packet, disconnected ) )
// 		{
// 			// Process packet
// 			sf::Uint16 intType;
// 			packet >> intType;
// 			const PacketType type = PacketType( intType );
// 
// 			switch ( type )
// 			{
// 			case PacketType::ACK_LOGIN:
// 				Debug::Log() << m_socket.GetName() << " connected !" << Debug::Endl();
// 				m_state = ClientState::CLIENT_CONNECTED;
// 				break;
// 			case PacketType::PING:
// 				m_socket.Send( packet, m_serverIp, m_serverPort );
// 				break;
// 			case PacketType::START_GAME:
// 				Debug::Log() << m_socket.GetName() << " start game " << Debug::Endl();
// 				m_playersManager->SpawnSpaceShips();
// 				break;
// 			default:
// 				Debug::Warning() << m_socket.GetName() << " strange packet received with id: " << intType << Debug::Endl();
// 				break;
// 			}
// 		}
// 	}

// 	//================================================================================================================================
// 	//================================================================================================================================
// 	void GameClient::ConnectToServer( const Port _serverPort, const sf::IpAddress _serverIp )
// 	{
// 		m_socket.OnlyReceiveFrom( _serverIp, _serverPort );
// 
// 		m_serverPort = _serverPort;
// 		m_serverIp = _serverIp;
// 
// 		m_state = CLIENT_CONNECTING;
// 		m_timer = 0.5f;
// 	}
// 
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	GameClient::NetPlayerData* GameClient::FindNetPlayer( const Gameobject* _gameobject )
// 	{
// 		for ( int playerIndex = 0; playerIndex < m_netPlayers.size(); playerIndex++ )
// 		{
// 			NetPlayerData& player = m_netPlayers[ playerIndex ];
// 			if ( player.playerPersistent == _gameobject )
// 			{
// 				return &player;
// 			}
// 		}
// 		return nullptr;
// 	}
// 
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	void GameClient::OnGui()
// 	{
// 
// 		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
// 		{
// 
// 		} ImGui::PopItemWidth();
// 	}
// 
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	bool GameClient::Load( const Json& _json )
// 	{
// 
// 		return true;
// 	}
// 
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	bool GameClient::Save( Json& _json ) const
// 	{
// 
// 		return true;
// 	}
}