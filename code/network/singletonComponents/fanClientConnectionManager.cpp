#include "network/singletonComponents/fanClientConnectionManager.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "core/time/fanTime.hpp"
#include "network/fanImGuiNetwork.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( ClientConnectionManager );

	//================================================================================================================================
	//================================================================================================================================
	void ClientConnectionManager::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.init = &ClientConnectionManager::Init;
		_info.onGui = &ClientConnectionManager::OnGui;
		_info.name = "client connection manager";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientConnectionManager::Init( EcsWorld& _world, SingletonComponent& _component )
	{
		ClientConnectionManager& connectionManager = static_cast<ClientConnectionManager&>( _component );
		connectionManager.clientPort = 53010;
		connectionManager.serverIP = "127.0.0.1";
		connectionManager.serverPort = 53000;
		connectionManager.state = ClientState::Disconnected;
		connectionManager.rtt = 0.f;
		connectionManager.timeoutTime = 10.f;
		connectionManager.serverLastResponse = 0.f;
		connectionManager.mustSendBackPacketPing = false;
		connectionManager.onServerDisconnected.Clear();
	}

	//================================================================================================================================
	// Write into the _packet to communicate with the server
	//================================================================================================================================
	void ClientConnectionManager::Write( Packet& _packet )
	{
		if( state == ClientState::Disconnected )
		{
			state = ClientState::PendingConnection;

			PacketHello hello;
			hello.name = "toto";
			hello.Write( _packet );
			_packet.onFail.Connect( &ClientConnectionManager::OnLoginFail, this );
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
	void ClientConnectionManager::OnLoginFail( HostID, const PacketTag /*_packetTag*/ )
	{
		if( state == ClientState::PendingConnection )
		{
			Debug::Log() << "login fail" << Debug::Endl();
			state = ClientState::Disconnected;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientConnectionManager::ProcessPacket( const PacketLoginSuccess& _packetLogin )
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
	void ClientConnectionManager::ProcessPacket( const PacketPing& _packetPing, const FrameIndex _frameIndex )
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
	void ClientConnectionManager::DetectServerTimout()
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
	void ClientConnectionManager::DisconnectFromServer()
	{
		onServerDisconnected.Emmit(0);
		Debug::Log() << "server timeout " << Debug::Endl();
		state = ClientState::Disconnected;
	}

	//================================================================================================================================
	// Editor gui helper
	//================================================================================================================================
	std::string GetStateName( const ClientConnectionManager::ClientState _clientState )
	{
		switch( _clientState )
		{
		case fan::ClientConnectionManager::ClientState::Disconnected:		return "Disconnected";		break;
		case fan::ClientConnectionManager::ClientState::PendingConnection:	return "PendingConnection";	break;
		case fan::ClientConnectionManager::ClientState::Connected:			return "Connected";			break;
		default:			assert( false );								return "Error";				break;
		}
	}

	//================================================================================================================================
	// returns a color corresponding to a rtt time in seconds
	//================================================================================================================================
	static ImVec4 GetStateColor( const ClientConnectionManager::ClientState _clientState )
	{
		switch( _clientState )
		{
		case fan::ClientConnectionManager::ClientState::Disconnected:		return Color::Red.ToImGui(); break;
		case fan::ClientConnectionManager::ClientState::PendingConnection:	return Color::Yellow.ToImGui(); break;
		case fan::ClientConnectionManager::ClientState::Connected:			return Color::Green.ToImGui(); break;
		default:			assert( false );								return Color::Purple.ToImGui(); break;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientConnectionManager::OnGui( EcsWorld&, SingletonComponent& _component )
	{
		ImGui::Indent(); ImGui::Indent();
		{
			ClientConnectionManager& connection = static_cast<ClientConnectionManager&>( _component );
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