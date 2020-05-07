#include "network/singletonComponents/fanClientConnection.hpp"

#include "core/fanDebug.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "core/time/fanTime.hpp"
#include "network/fanImGuiNetwork.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( ClientConnection );

	//================================================================================================================================
	//================================================================================================================================
	void ClientConnection::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.init = &ClientConnection::Init;
		_info.onGui = &ClientConnection::OnGui;
		_info.name = "client connection";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientConnection::Init( EcsWorld& _world, SingletonComponent& _component )
	{
		ClientConnection& connection = static_cast<ClientConnection&>( _component );
		connection.socket.Unbind();
		connection.clientPort = 53010;
		connection.serverIP = "127.0.0.1";
		connection.serverPort = 53000;
		connection.state = ClientState::Disconnected;
		connection.rtt = 0.f;
		connection.timeoutTime = 10.f;
		connection.bandwidth = 0.f;
		connection.serverLastResponse = 0.f;
		connection.lastPacketPing = PacketPing();
		connection.mustSendBackPacketPing = false;
	}

	//================================================================================================================================
	// Write into the _packet to communicate with the server
	//================================================================================================================================
	void ClientConnection::Write( Packet& _packet )
	{
		if( state == ClientState::Disconnected )
		{
			state = ClientState::PendingConnection;

			PacketHello hello;
			hello.name = "toto";
			hello.Write( _packet );
			_packet.onFail.Connect( &ClientConnection::OnLoginFail, this );
		}
		else if( state == ClientState::Connected )
		{
			if( mustSendBackPacketPing )
			{
				lastPacketPing.Write( _packet ); 
				mustSendBackPacketPing = false;
			}
		}
	}

	//================================================================================================================================
	// login packet dropped our timed out. Resend a new one.
	//================================================================================================================================
	void ClientConnection::OnLoginFail( const PacketTag /*_packetTag*/ )
	{
		if( state == ClientState::PendingConnection )
		{
			Debug::Log() << "login fail" << Debug::Endl();
			state = ClientState::Disconnected;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientConnection::ProcessPacket( const PacketLoginSuccess& _packetLogin )
	{
		if( state == ClientState::PendingConnection )
		{
			Debug::Log() << "login success !" << Debug::Endl();
			state = ClientState::Connected;
		}
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
	//================================================================================================================================
	void ClientConnection::DetectServerTimout()
	{
		if( state == ClientState::Connected )
		{
			const double currentTime = Time::Get().ElapsedSinceStartup();
			if( serverLastResponse + timeoutTime < currentTime )
			{
				DisconnectFromServer();
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientConnection::DisconnectFromServer()
	{
		Debug::Log() << "server timeout " << Debug::Endl();
		state = ClientState::Disconnected;
	}

	//================================================================================================================================
	// Editor gui helper
	//================================================================================================================================
	std::string GetStateName( const ClientConnection::ClientState _clientState )
	{
		switch( _clientState )
		{
		case fan::ClientConnection::ClientState::Disconnected:		return "Disconnected";		break;
		case fan::ClientConnection::ClientState::PendingConnection:	return "PendingConnection";	break;
		case fan::ClientConnection::ClientState::Connected:			return "Connected";			break;
		default:			assert( false );								return "Error";				break;
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
		case fan::ClientConnection::ClientState::PendingConnection:	return Color::Yellow.ToImGui(); break;
		case fan::ClientConnection::ClientState::Connected:			return Color::Green.ToImGui(); break;
		default:			assert( false );								return Color::Purple.ToImGui(); break;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientConnection::OnGui( EcsWorld&, SingletonComponent& _component )
	{
		ImGui::Indent(); ImGui::Indent();
		{
			ClientConnection& connection = static_cast<ClientConnection&>( _component );
			double currentTime = Time::Get().ElapsedSinceStartup();

			ImGui::Text( "Client" );
			ImGui::Separator();
			ImGui::DragFloat( "timeout time", &connection.timeoutTime, 0.1f, 0.f, 10.f );
			ImGui::Text( "state:               " ); ImGui::SameLine();
			ImGui::TextColored( GetStateColor( connection.state ), "%s", GetStateName( connection.state ).c_str() );
			ImGui::Text( "client port           %u", connection.clientPort );
			ImGui::Text( "server adress         %s::%u", connection.serverIP.toString().c_str(), connection.serverPort );			
			ImGui::Text( "rtt                  "); ImGui::SameLine();
			ImGui::TextColored( GetRttColor( connection.rtt ), "%.1f", 1000.f * connection.rtt );
			ImGui::Text( "bandwidth:            %.1f Ko/s", connection.bandwidth );
			ImGui::Text( "server last response: %.1f", Time::Get().ElapsedSinceStartup() - connection.serverLastResponse );
		}ImGui::Unindent(); ImGui::Unindent();
	}
}