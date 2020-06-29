#include "network/components/fanHostConnection.hpp"

#include "network/singletons/fanTime.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "network/singletons/fanTime.hpp"
#include "network/fanImGuiNetwork.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void HostConnection::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::SOCKET16;
		_info.onGui = &HostConnection::OnGui;
		_info.name = "host connection";
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostConnection::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		HostConnection& hostConnection = static_cast<HostConnection&>( _component );
		hostConnection.ip = sf::IpAddress();
		hostConnection.port = 0;
		hostConnection.name = "unknown";
		hostConnection.state = State::Disconnected;
		hostConnection.lastResponseTime = 0.f;
		hostConnection.lastPingTime = 0.f;
		hostConnection.lastDisconnectTime = 0.f;
		hostConnection.rtt = -1.f;
		hostConnection.bandwidth = 0.f;
		hostConnection.pingDelay = .2f;
		hostConnection.disconnectDelay = 1.f;
		hostConnection.timeoutDelay = 5.f;

		hostConnection.synced = false;
		hostConnection.lastSync = 0.f;
		hostConnection.targetBufferSize = 10;
		hostConnection.framesDelta = { -1000,0,1000 };
		hostConnection.nextDeltaIndex = 0;
	}

	//================================================================================================================================
	// sends a login packet to the clients needing approval
	// regularly sends ping to clients to calculate RTT & sync frame index
	//================================================================================================================================
	void HostConnection::Write( EcsWorld& _world, EcsEntity _entity, Packet& _packet )
	{
 		const Time& time = _world.GetSingleton<Time>();

		// Send login packet
		if( state == HostConnection::NeedingApprouval )
		{
			const EcsHandle handle = _world.GetHandle( _entity );

			PacketLoginSuccess packetLogin;
			packetLogin.playerID = handle;
			packetLogin.Write( _packet );			
			_packet.onSuccess.Connect( &HostConnection::OnLoginSuccess, _world, handle );
			_packet.onFail.Connect( &HostConnection::OnLoginFail, _world, handle );
			state = HostConnection::PendingApprouval;
		}
		else if( state == HostConnection::Connected )
		{
			// Ping client
			const double currentTime = Time::ElapsedSinceStartup();
			if( currentTime - lastPingTime > pingDelay )
			{
				lastPingTime = currentTime;

				PacketPing packetPing;
				packetPing.previousRtt = rtt;
				packetPing.serverFrame = time.frameIndex;
				packetPing.Write( _packet );
			}
		}
		else if( state == HostConnection::Disconnected )
		{
			const double currentTime = Time::ElapsedSinceStartup();
			if( currentTime - lastDisconnectTime > disconnectDelay )
			{
				lastDisconnectTime = currentTime;

				PacketDisconnect packetDisconnect;
				packetDisconnect.Write( _packet );

				Debug::Log() << "host connection error " << ip.toString() << "::" << port << Debug::Endl();
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostConnection::ProcessPacket( const PacketHello& _packetHello ) 
	{
		if( state == HostConnection::Disconnected )
		{
			name = _packetHello.name;
			state = HostConnection::NeedingApprouval;
		}
		else if( state == HostConnection::Connected )
		{
			state = HostConnection::NeedingApprouval;
			Debug::Log() << "host disconnected " << ip.toString() << "::" << port << Debug::Endl();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostConnection::ProcessPacket( const PacketPing& _packetPing, const FrameIndex _frameIndex, const float _logicDelta )
	{
		const FrameIndex delta = _frameIndex - _packetPing.serverFrame; // number of frames elapsed between sending & receiving
		const FrameIndex clientCurrentFrameIndex = _packetPing.clientFrame + delta / 2;

		rtt = _logicDelta * delta;
		framesDelta[nextDeltaIndex] = _frameIndex - clientCurrentFrameIndex;
		nextDeltaIndex = ( nextDeltaIndex + 1 ) % int( framesDelta.size() );
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostConnection::OnSyncSuccess()
	{
		synced = true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostConnection::OnLoginFail( const PacketTag  )
	{
		if( state == HostConnection::PendingApprouval )
		{
			Debug::Log() << "host " << ip.toString() << "::" << port << " login failed, resending approval. " << Debug::Endl();
			state = HostConnection::NeedingApprouval;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostConnection::OnLoginSuccess( const PacketTag )
	{
		if( state == HostConnection::PendingApprouval )
		{
			Debug::Log() << "host connected " << ip.toString() << "::" << port << Debug::Endl();
			state = HostConnection::Connected;
		}
	}

	//================================================================================================================================
	// returns the perfect frame index timing for something to spawn immediately when sent from the server to the client
	//================================================================================================================================
	FrameIndex HostConnection::CalculatePerfectSpawnTiming( const HostConnection& _connection, const Time& _time )
	{
		int averageFrameDelta = 0;
		for( int delta : _connection.framesDelta )
		{
			averageFrameDelta += delta;
		}
		averageFrameDelta /= (int)_connection.framesDelta.size();

		return FrameIndex( -averageFrameDelta ) + FrameIndex( _connection.rtt / _time.logicDelta ) + 3;
	}

	//================================================================================================================================
	// Editor gui helper
	//================================================================================================================================
	std::string GetStateName( const HostConnection::State _clientState )
	{
		switch( _clientState )
		{
		case HostConnection::Disconnected:		return "Disconnected";		break;
		case HostConnection::NeedingApprouval:	return "NeedingApprouval";	break;
		case HostConnection::PendingApprouval:	return "PendingApprouval";	break;
		case HostConnection::Connected:			return "Connected";			break;
		default:			assert( false );		return "error";				break;
		}
	}

	//================================================================================================================================
	// returns a color corresponding to a rtt time in seconds
	//================================================================================================================================
	static ImVec4 GetStateColor( const HostConnection::State _clientState )
	{
		switch( _clientState )
		{
		case HostConnection::Disconnected:		return  Color::Orange.ToImGui(); break;
		case HostConnection::NeedingApprouval:	return  Color::Yellow.ToImGui(); break;
		case HostConnection::PendingApprouval:	return  Color::Yellow.ToImGui(); break;
		case HostConnection::Connected:			return  Color::Green.ToImGui(); break;
		default:			assert( false );	return  Color::Purple.ToImGui(); break;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostConnection::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		HostConnection& hostConnection = static_cast<HostConnection&>( _component );
		const double currentTime = Time::ElapsedSinceStartup();
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
		{
			ImGui::Text( "name:               %s", hostConnection.name.c_str() );
			ImGui::TextColored( GetStateColor( hostConnection.state ), "state:              %s", GetStateName( hostConnection.state ).c_str() );
			ImGui::TextColored( GetRttColor( hostConnection.rtt ), "rtt:                %.1f", 1000.f * hostConnection.rtt );
			ImGui::Text( "bandwidth :         %.1f Ko/s", hostConnection.bandwidth );
			ImGui::Text( "last response:      %.1f", currentTime - hostConnection.lastResponseTime );			
			ImGui::Text( "adress:             %s::%u", hostConnection.ip.toString().c_str(), hostConnection.port );
			ImGui::Text( "target buffer size: %d", hostConnection.targetBufferSize );
			ImGui::Text( "frame delta:        %d %d %d", hostConnection.framesDelta[0], hostConnection.framesDelta[1], hostConnection.framesDelta[2]); 
			ImGui::DragFloat( "ping delay", &hostConnection.pingDelay, 0.1f, 0.f, 10.f );
			ImGui::DragFloat( "timeout time", &hostConnection.timeoutDelay, 0.1f, 0.f, 10.f );
		} ImGui::PopItemWidth();
	}
}