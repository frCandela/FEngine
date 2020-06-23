#include "network/components/fanClientConnection.hpp"

#include "core/fanDebug.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "network/singletons/fanTime.hpp"
#include "network/fanImGuiNetwork.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void ClientConnection::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.destroy = &ClientConnection::Destroy;
		_info.onGui = &ClientConnection::OnGui;
		_info.name = "client connection";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientConnection::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		ClientConnection& connection = static_cast<ClientConnection&>( _component );
		assert( connection.socket == nullptr );
		connection.socket = new UdpSocket();
		connection.clientPort = 53010;
		connection.serverIP = "127.0.0.1";
		connection.serverPort = 53000;
		connection.state = ClientState::Disconnected;
		connection.rtt = 0.f;
		connection.timeoutDelay = 5.f;
		connection.bandwidth = 0.f;
		connection.serverLastResponse = 0.f;
		connection.lastPacketPing = PacketPing();
		connection.mustSendBackPacketPing = false;	
		connection.onLoginSuccess.Clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientConnection::Destroy( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		ClientConnection& connection = static_cast<ClientConnection&>( _component );
		delete connection.socket;
		connection.socket = nullptr;
	}

	//================================================================================================================================
	// Write into the _packet to communicate with the server
	//================================================================================================================================
	void ClientConnection::Write( EcsWorld& _world, EcsEntity _entity, Packet& _packet )
	{
		switch( state )
		{

		case fan::ClientConnection::ClientState::Disconnected:
		{
			PacketHello hello;
			hello.name = "toto";
			hello.Write( _packet );
			_packet.onFail.Connect( &ClientConnection::OnLoginFail, _world, _world.GetHandle( _entity ) );
			state = ClientState::PendingConnection;
			Debug::Log() << "logging in..." << Debug::Endl();
		}
		break;

		case fan::ClientConnection::ClientState::PendingConnection:
			break;

		case fan::ClientConnection::ClientState::Connected:
			if( mustSendBackPacketPing )
			{
				lastPacketPing.Write( _packet );
				mustSendBackPacketPing = false;
			}
			break;

		case fan::ClientConnection::ClientState::Stopping:
		{
			{
				PacketDisconnect disconnect;
				disconnect.Write( _packet );
				state = ClientState::Disconnected;
			}
		}break;

		default:
			assert( false );
			break;
		}
	}

	//================================================================================================================================
	// login packet dropped our timed out. Resend a new one.
	//================================================================================================================================
	void ClientConnection::OnLoginFail( const PacketTag /*_packetTag*/ )
	{
		if( state == ClientState::PendingConnection )
		{
			state = ClientState::Disconnected;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientConnection::ProcessPacket( const PacketLoginSuccess& _packetLogin )
	{
		if( state == ClientState::PendingConnection )
		{
			Debug::Log() << "login success" << Debug::Endl();
			state = ClientState::Connected;
			assert( _packetLogin.playerID != 0 );
			onLoginSuccess.Emmit( _packetLogin.playerID );//playerID = _packetLogin.playerID;a
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientConnection::ProcessPacket( const PacketDisconnect& /*_packetDisconnect*/ )
	{
		state = ClientState::Disconnected;
		Debug::Log() << "disconnected from server" << Debug::Endl();		
	}

	//================================================================================================================================
	// received ping packet from the server.
	// updates the rtt & sends back the packet later while adding the current client frame index
	//================================================================================================================================
	void ClientConnection::ProcessPacket( const PacketPing& _packetPing, const FrameIndex _frameIndex )
	{
		if( state == ClientState::Connected )
		{
			lastPacketPing = _packetPing;
			lastPacketPing.clientFrame = _frameIndex,
			rtt = _packetPing.previousRtt;
			mustSendBackPacketPing = true;
		}
	}

	//================================================================================================================================
	// Editor gui helper
	//================================================================================================================================
	std::string GetStateName( const ClientConnection::ClientState _clientState )
	{
		switch( _clientState )
		{
		case fan::ClientConnection::ClientState::Disconnected:		return "Disconnected";		break;
		case fan::ClientConnection::ClientState::Stopping:			return "Stopping";			break;
		case fan::ClientConnection::ClientState::PendingConnection:	return "PendingConnection";	break;
		case fan::ClientConnection::ClientState::Connected:			return "Connected";			break;
		default:			assert( false );						return "Error";				break;
		}
	}

	//================================================================================================================================
	// returns a color corresponding to a rtt time in seconds
	//================================================================================================================================
	static ImVec4 GetStateColor( const ClientConnection::ClientState _clientState )
	{
		switch( _clientState )
		{
		case fan::ClientConnection::ClientState::Disconnected:		return Color::Red.ToImGui(); break;
		case fan::ClientConnection::ClientState::Stopping:			return Color::Purple.ToImGui(); break;
		case fan::ClientConnection::ClientState::PendingConnection:	return Color::Yellow.ToImGui(); break;
		case fan::ClientConnection::ClientState::Connected:			return Color::Green.ToImGui(); break;
		default:			assert( false );								return Color::Purple.ToImGui(); break;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientConnection::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		ImGui::Indent(); ImGui::Indent();
		{
			ClientConnection& connection = static_cast<ClientConnection&>( _component );

			ImGui::DragFloat( "timeout time", &connection.timeoutDelay, 0.1f, 0.f, 10.f );
			ImGui::Text( "state:               " ); ImGui::SameLine();
			ImGui::TextColored( GetStateColor( connection.state ), "%s", GetStateName( connection.state ).c_str() );
			ImGui::Text( "client port           %u", connection.clientPort );
			ImGui::Text( "server adress         %s::%u", connection.serverIP.toString().c_str(), connection.serverPort );			
			ImGui::Text( "rtt                  "); ImGui::SameLine();
			ImGui::TextColored( GetRttColor( connection.rtt ), "%.1f", 1000.f * connection.rtt );
			ImGui::Text( "bandwidth:            %.1f Ko/s", connection.bandwidth );
			ImGui::Text( "server last response: %.1f", Time::ElapsedSinceStartup() - connection.serverLastResponse );
		}ImGui::Unindent(); ImGui::Unindent();
	}
}