#include "network/components/fanHostConnection.hpp"

#include "core/fanColor.hpp"
#include "core/time/fanTime.hpp"
#include "network/fanImGuiNetwork.hpp"

namespace fan
{
	REGISTER_COMPONENT( HostConnection, "host connection" );

	//================================================================================================================================
	//================================================================================================================================
	void HostConnection::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::NETWORK16;
		_info.onGui = &HostConnection::OnGui;
		_info.init = &HostConnection::Init;
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostConnection::Init( EcsWorld& _world, Component& _component )
	{
		HostConnection& hostConnection = static_cast<HostConnection&>( _component );
		hostConnection.hostId = 0;
		hostConnection.ip = sf::IpAddress();
		hostConnection.port = 0;
		hostConnection.name = "";
		hostConnection.state = State::Null;
		hostConnection.lastResponseTime = 0.f;
		hostConnection.lastPingTime = 0.f;
		hostConnection.rtt = -1.f;
		hostConnection.bandwidth = 0.f;

		hostConnection.synced = false;
		hostConnection.lastSync = 0.f;
		hostConnection.framesDelta = { -1000,0,0,0,1000 };
		hostConnection.nextDeltaIndex = 0;
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostConnection::ProcessPacket( const HostID _clientID, const PacketHello& _packetHello ) 
	{
		if( state == HostConnection::Disconnected )
		{
			name = _packetHello.name;
			state = HostConnection::NeedingApprouval;
		}
		else if( state == HostConnection::Connected )
		{
			state = HostConnection::NeedingApprouval;
			Debug::Log() << "Client " << _clientID << " disconnected" << Debug::Endl();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostConnection::ProcessPacket( const HostID _clientID, const PacketPing& _packetPing, const FrameIndex _frameIndex, const float _logicDelta )
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
	void HostConnection::OnLoginFail( const PacketTag /*_packetTag*/ )
	{
		if( state == HostConnection::PendingApprouval )
		{
			Debug::Log() << " login failed, resending approval. " << Debug::Endl();
			state = HostConnection::NeedingApprouval;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostConnection::OnLoginSuccess( const PacketTag /*_packetTag*/ )
	{
		if( state == HostConnection::PendingApprouval )
		{
			Debug::Log() << " connected " << Debug::Endl();
			state = HostConnection::Connected;
		}
	}

	//================================================================================================================================
	// Editor gui helper
	//================================================================================================================================
	std::string GetStateName( const HostConnection::State _clientState )
	{
		switch( _clientState )
		{
		case HostConnection::Null:				return "Null";				break;
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
		case HostConnection::Null:				return  Color::Red.ToImGui(); break;
		case HostConnection::Disconnected:		return  Color::Orange.ToImGui(); break;
		case HostConnection::NeedingApprouval:	return  Color::Yellow.ToImGui(); break;
		case HostConnection::PendingApprouval:	return  Color::Yellow.ToImGui(); break;
		case HostConnection::Connected:			return  Color::Green.ToImGui(); break;
		default:			assert( false );		return  Color::Purple.ToImGui(); break;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostConnection::OnGui( EcsWorld& _world, EntityID _entityID, Component& _component )
	{
		HostConnection& hostConnection = static_cast<HostConnection&>( _component );
		const double currentTime = Time::Get().ElapsedSinceStartup();
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
		{
			ImGui::Text( "name:          %s", hostConnection.name.c_str() );
			ImGui::TextColored( GetStateColor( hostConnection.state ), "state:         %s", GetStateName( hostConnection.state ).c_str() );
			ImGui::TextColored( GetRttColor( hostConnection.rtt ), "rtt:           %.1f", 1000.f * hostConnection.rtt );
			ImGui::Text( "bandwidth :    %.1f Ko/s", hostConnection.bandwidth );
			ImGui::Text( "last response: %.1f", currentTime - hostConnection.lastResponseTime );
			ImGui::Text( "adress:        %s::%u", hostConnection.ip.toString().c_str(), hostConnection.port );
			ImGui::Text( "frame delta:   %d %d %d %d %d", hostConnection.framesDelta[0], hostConnection.framesDelta[1], hostConnection.framesDelta[2], hostConnection.framesDelta[3], hostConnection.framesDelta[4] );
		} ImGui::PopItemWidth();
	}
}