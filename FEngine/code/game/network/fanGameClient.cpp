#include "fanGlobalIncludes.h"
#include "game/network/fanGameClient.h"

#include "core/input/fanInputManager.h"
#include "core/input/fanInput.h"
#include "core/input/fanMouse.h"
#include "core/input/fanJoystick.h"
#include "scene/components/fanCamera.h"
#include "scene/components/fanTransform.h"
#include "network/packets/fanIPacket.h"
#include "network/packets/fanPacketLogin.h"

namespace fan
{
	REGISTER_TYPE_INFO( GameClient, TypeInfo::Flags::EDITOR_COMPONENT, "game/net/" )

	//================================================================================================================================
	//================================================================================================================================
	void GameClient::OnAttach()
	{
		Actor::OnAttach();
		m_socket.Create("[unknown]", 53001);
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameClient::OnDetach()
	{
		Actor::OnDetach();
		m_socket.UnBind();
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameClient::Start()
	{
		// Remove this if we are on a server scene HACK?
		if ( GetScene().IsServer() )
		{
			GetScene().DeleteComponent( this );
			return;
		}

		while ( !m_socket.Bind() )
		{			
			m_socket.SetPort( m_socket.GetPort() + 1 );
		}
		ConnectToServer( 53000, "127.0.0.1" );
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameClient::Stop()
	{
		m_socket.UnBind();
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameClient::Update( const float _delta )
	{
		m_timer -= _delta;

		Receive();		

		switch ( m_state )
		{
		case ClientState::NONE:
			break;
		case ClientState::CONNECTING:
			if ( m_timer < 0.f )
			{
				Debug::Log() << m_socket.GetName() << " attempting connection to server" << Debug::Endl();
				PacketLogin packet(  m_socket.GetName() );
				m_socket.Send( packet.ToPacket(), m_serverIp, m_serverPort );
				m_timer = 0.5f;
			}
			break;
		case ClientState::CONNECTED:
			break;

		default:
			break;
		}
		
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameClient::LateUpdate( const float /*_delta*/ ){}

	//================================================================================================================================
	//================================================================================================================================
	void GameClient::Receive( )
	{
		bool disconnected = false;
		sf::Packet packet;
		while ( m_socket.Receive( packet, disconnected) )
		{
			// Process packet
			sf::Uint16 intType;
			packet >> intType;
			const PacketType type = PacketType( intType );

			switch ( type )
			{
			case PacketType::ACK_LOGIN:
				Debug::Log() << m_socket.GetName() << " connected !" << Debug::Endl();
				m_state = ClientState::CONNECTED;
				break;
			case PacketType::PING:
				m_socket.Send( packet, m_serverIp, m_serverPort  );
				break;
			case PacketType::START_GAME:
				Debug::Log() << m_socket.GetName() << " start game " << Debug::Endl();
				break;
			default:
				Debug::Warning() << m_socket.GetName() << " strange packet received with id: " << intType << Debug::Endl();
				break;
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameClient::ConnectToServer( const Port _serverPort, const sf::IpAddress _serverIp )
	{
		m_socket.OnlyReceiveFrom( _serverIp, _serverPort );

		m_serverPort = _serverPort;
		m_serverIp = _serverIp;

		m_state = CONNECTING;
		m_timer = 0.5f;
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void GameClient::OnGui()
	{
		Actor::OnGui();

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{

		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool GameClient::Load( const Json & _json )
	{
		Actor::Load( _json );
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool GameClient::Save( Json & _json ) const
	{
		Actor::Save( _json );
		return true;
	}


}