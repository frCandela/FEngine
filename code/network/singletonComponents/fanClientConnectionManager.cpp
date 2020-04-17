#include "network/singletonComponents/fanClientConnectionManager.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "core/time/fanTime.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( ClientConnectionManager );

	//================================================================================================================================
	//================================================================================================================================
	void ClientConnectionManager::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::JOYSTICK16;
		_info.init = &ClientConnectionManager::Init;
		_info.onGui = &ClientConnectionManager::OnGui;
		_info.name = "client connection manager";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientConnectionManager::Init( EcsWorld& _world, SingletonComponent& _component )
	{
		ClientConnectionManager& connectionManager = static_cast<ClientConnectionManager&>( _component );
		connectionManager.clientPort = 53001;
		connectionManager.serverIP = "127.0.0.1";
		connectionManager.serverPort = 53000;
		connectionManager.state = ClientState::Disconnected;
		connectionManager.roundTripTime = 0.f;
		connectionManager.timeoutTime = 10;
	}

	//================================================================================================================================
	// Write into the _packet to communicate with the server
	//================================================================================================================================
	void ClientConnectionManager::Send( Packet& _packet )
	{
		if( state == ClientState::Disconnected )
		{
			state = ClientState::PendingConnection;

			PacketHello hello;
			hello.name = "toto";
			hello.Save( _packet );
			_packet.onFail.Connect( &ClientConnectionManager::OnLoginFail, this );
		}
		else if( state == ClientState::Connected )
		{
			const double currentTime = Time::Get().ElapsedSinceStartup();
			if( serverLastResponse + timeoutTime < currentTime )
			{
				Debug::Log() << "server timeout " << Debug::Endl();
				state = ClientState::Disconnected;
			}
		}
	}

	//================================================================================================================================
	// login packet dropped our timed out. Resend a new one.
	//================================================================================================================================
	void ClientConnectionManager::OnLoginFail( HostID )
	{
		if( state == ClientState::PendingConnection )
		{
			Debug::Log() << "login failure" << Debug::Endl();
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
	//================================================================================================================================
	void ClientConnectionManager::ProcessPacket( const PacketPing& _packetPing )
	{

	}

	//================================================================================================================================
	// Editor gui helper
	//================================================================================================================================
	std::string ToString( ClientConnectionManager::ClientState _clientState )
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
	//================================================================================================================================
	void ClientConnectionManager::OnGui( SingletonComponent& _component )
	{
		ImGui::Indent(); ImGui::Indent();
		{
			ClientConnectionManager& connection = static_cast<ClientConnectionManager&>( _component );
			double currentTime = Time::Get().ElapsedSinceStartup();

			ImGui::Text( "Client" );
			ImGui::Separator();
			ImGui::Text( "state:                %s", ToString( connection.state ).c_str() );
			ImGui::Text( "client port           %u", connection.clientPort );
			ImGui::Text( "server adress         %s::%u", connection.serverIP.toString().c_str(), connection.serverPort );
			ImGui::Text( "ping                  %.01f", 0.5f * 1000.f * connection.roundTripTime );
			ImGui::Text( "server last response:  %.1f", Time::Get().ElapsedSinceStartup() - connection.serverLastResponse );
		}ImGui::Unindent(); ImGui::Unindent();
	}
}