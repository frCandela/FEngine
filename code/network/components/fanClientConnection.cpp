#include "network/components/fanClientConnection.hpp"
#include "network/singletons/fanTime.hpp"
#include "network/fanImGuiNetwork.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void ClientConnection::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon   = ImGui::Socket16;
		_info.mGroup  = EngineGroups::Network;
		_info.destroy = &ClientConnection::Destroy;
		_info.onGui   = &ClientConnection::OnGui;
		_info.mName   = "client connection";
	}

	//========================================================================================================
	//========================================================================================================
	void ClientConnection::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		ClientConnection& connection = static_cast<ClientConnection&>( _component );
        fanAssert( connection.mSocket == nullptr );
		connection.mSocket                 = new UdpSocket();
		connection.mClientPort             = 53010;
		connection.mServerIP               = "127.0.0.1";
		connection.mServerPort             = 53000;
		connection.mState                  = ClientState::Disconnected;
		connection.mRtt                    = 0.f;
		connection.mTimeoutDelay           = 5.f;
		connection.mBandwidth              = 0.f;
		connection.mServerLastResponse     = 0.f;
		connection.mLastPacketPing         = PacketPing();
		connection.mMustSendBackPacketPing = false;
		connection.mOnLoginSuccess.Clear();
	}

	//========================================================================================================
	//========================================================================================================
	void ClientConnection::Destroy( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		ClientConnection& connection = static_cast<ClientConnection&>( _component );
		delete connection.mSocket;
		connection.mSocket = nullptr;
	}

	//========================================================================================================
	// Write into the _packet to communicate with the server
	//========================================================================================================
	void ClientConnection::Write( EcsWorld& _world, EcsEntity _entity, Packet& _packet )
	{
		switch( mState )
		{

		case fan::ClientConnection::ClientState::Disconnected:
		{
			PacketHello hello;
			hello.mName = "toto";
			hello.Write( _packet );
			_packet.mOnFail.Connect( &ClientConnection::OnLoginFail, _world, _world.GetHandle( _entity ) );
            mState = ClientState::PendingConnection;
			Debug::Log() << "logging in..." << Debug::Endl();
		}
		break;

		case fan::ClientConnection::ClientState::PendingConnection:
			break;

		case fan::ClientConnection::ClientState::Connected:
			if( mMustSendBackPacketPing )
			{
				mLastPacketPing.Write( _packet );
                mMustSendBackPacketPing = false;
			}
			break;

		case fan::ClientConnection::ClientState::Stopping:
		{
			{
				PacketDisconnect disconnect;
				disconnect.Write( _packet );
                mState = ClientState::Disconnected;
			}
		}break;

		default:
            fanAssert( false );
			break;
		}
	}

	//========================================================================================================
	// login packet dropped our timed out. Resend a new one.
	//========================================================================================================
	void ClientConnection::OnLoginFail( const PacketTag /*_packetTag*/ )
	{
		if( mState == ClientState::PendingConnection )
		{
            mState = ClientState::Disconnected;
		}
	}

	//========================================================================================================
	//========================================================================================================
	void ClientConnection::ProcessPacket( const PacketLoginSuccess& _packetLogin )
	{
		if( mState == ClientState::PendingConnection )
		{
			Debug::Log() << "login success" << Debug::Endl();
            mState = ClientState::Connected;
            fanAssert( _packetLogin.mPlayerId != 0 );
			mOnLoginSuccess.Emmit( _packetLogin.mPlayerId );//playerID = _packetLogin.playerID;a
		}
	}

	//========================================================================================================
	//========================================================================================================
	void ClientConnection::ProcessPacket( const PacketDisconnect& /*_packetDisconnect*/ )
	{
        mState = ClientState::Disconnected;
		Debug::Log() << "disconnected from server" << Debug::Endl();		
	}

	//========================================================================================================
	// received ping packet from the server.
	// updates the rtt & sends back the packet later while adding the current client frame index
	//========================================================================================================
	void ClientConnection::ProcessPacket( const PacketPing& _packetPing, const FrameIndex _frameIndex )
	{
		if( mState == ClientState::Connected )
		{
            mLastPacketPing = _packetPing;
            mLastPacketPing.mClientFrame = _frameIndex,
                    mRtt            = _packetPing.mPreviousRtt;
            mMustSendBackPacketPing = true;
		}
	}

	//========================================================================================================
	// Editor gui helper
	//========================================================================================================
	std::string GetStateName( const ClientConnection::ClientState _clientState )
	{
		switch( _clientState )
		{
		case fan::ClientConnection::ClientState::Disconnected:		return "Disconnected";		break;
		case fan::ClientConnection::ClientState::Stopping:			return "Stopping";			break;
		case fan::ClientConnection::ClientState::PendingConnection:	return "PendingConnection";	break;
		case fan::ClientConnection::ClientState::Connected:			return "Connected";			break;
		default:			fanAssert( false );						return "Error";				break;
		}
	}

	//========================================================================================================
	// returns a color corresponding to a rtt time in seconds
	//========================================================================================================
	static ImVec4 GetStateColor( const ClientConnection::ClientState _clientState )
	{
		switch( _clientState )
		{
		case fan::ClientConnection::ClientState::Disconnected:		return Color::sRed.ToImGui();
		case fan::ClientConnection::ClientState::Stopping:			return Color::sPurple.ToImGui();
		case fan::ClientConnection::ClientState::PendingConnection:	return Color::sYellow.ToImGui();
		case fan::ClientConnection::ClientState::Connected:			return Color::sGreen.ToImGui();
		default:			fanAssert( false );						return Color::sPurple.ToImGui();
		}
	}

	//========================================================================================================
	//========================================================================================================
	void ClientConnection::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
        ClientConnection& connection = static_cast<ClientConnection&>( _component );

        ImGui::DragFloat( "timeout time", &connection.mTimeoutDelay, 0.1f, 0.f, 10.f );
        ImGui::Text( "state:               " );
        ImGui::SameLine();
        ImGui::TextColored( GetStateColor( connection.mState ),
                            "%s",
                            GetStateName( connection.mState ).c_str() );
        ImGui::Text( "client port           %u", connection.mClientPort );
        ImGui::Text( "server adress         %s::%u", connection.mServerIP.toString().c_str(),
                                                     connection.mServerPort );
        ImGui::Text( "rtt                  " );
        ImGui::SameLine();
        ImGui::TextColored( GetRttColor( connection.mRtt ), "%.1f", 1000.f * connection.mRtt );
        ImGui::Text( "bandwidth:            %.1f Ko/s", connection.mBandwidth );
        ImGui::Text( "server last response: %.1f",
                     Time::ElapsedSinceStartup() - connection.mServerLastResponse );
	}
}