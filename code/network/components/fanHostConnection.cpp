#include "network/components/fanHostConnection.hpp"
#include "network/singletons/fanTime.hpp"
#include "network/fanImGuiNetwork.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void HostConnection::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon  = ImGui::IconType::Socket16;
		_info.mGroup = EngineGroups::Network;
		_info.onGui  = &HostConnection::OnGui;
		_info.mName  = "host connection";
	}

	//========================================================================================================
	//========================================================================================================
	void HostConnection::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		HostConnection& hostConnection = static_cast<HostConnection&>( _component );
		hostConnection.mIp                 = sf::IpAddress();
		hostConnection.mPort               = 0;
		hostConnection.mName               = "unknown";
		hostConnection.mState              = State::Disconnected;
		hostConnection.mLastResponseTime   = 0.f;
		hostConnection.mLastPingTime       = 0.f;
		hostConnection.mLastDisconnectTime = 0.f;
		hostConnection.mRtt                = -1.f;
		hostConnection.mBandwidth          = 0.f;
		hostConnection.mPingDelay          = .2f;
		hostConnection.mDisconnectDelay    = 1.f;
		hostConnection.mTimeoutDelay       = 5.f;

		hostConnection.mSynced           = false;
		hostConnection.mLastSync         = 0.f;
		hostConnection.mTargetBufferSize = 10;
		hostConnection.mFramesDelta      = { -1000, 0, 1000 };
		hostConnection.mNextDeltaIndex   = 0;
	}

	//========================================================================================================
	// sends a login packet to the clients needing approval
	// regularly sends ping to clients to calculate RTT & sync frame index
	//========================================================================================================
	void HostConnection::Write( EcsWorld& _world, EcsEntity _entity, Packet& _packet )
	{
 		const Time& time = _world.GetSingleton<Time>();

		// Send login packet
		if( mState == HostConnection::NeedingApprouval )
		{
			const EcsHandle handle = _world.GetHandle( _entity );

			PacketLoginSuccess packetLogin;
			packetLogin.playerID = handle;
			packetLogin.Write( _packet );			
			_packet.onSuccess.Connect( &HostConnection::OnLoginSuccess, _world, handle );
			_packet.onFail.Connect( &HostConnection::OnLoginFail, _world, handle );
            mState = HostConnection::PendingApprouval;
		}
		else if( mState == HostConnection::Connected )
		{
			// Ping client
			const double currentTime = Time::ElapsedSinceStartup();
			if( currentTime - mLastPingTime > mPingDelay )
			{
                mLastPingTime = currentTime;

				PacketPing packetPing;
				packetPing.previousRtt = mRtt;
				packetPing.serverFrame = time.mFrameIndex;
				packetPing.Write( _packet );
			}
		}
		else if( mState == HostConnection::Disconnected )
		{
			const double currentTime = Time::ElapsedSinceStartup();
			if( currentTime - mLastDisconnectTime > mDisconnectDelay )
			{
                mLastDisconnectTime = currentTime;

				PacketDisconnect packetDisconnect;
				packetDisconnect.Write( _packet );

				Debug::Log() << "host connection error " << mIp.toString() << "::" << mPort << Debug::Endl();
			}
		}
	}

	//========================================================================================================
	//========================================================================================================
	void HostConnection::ProcessPacket( const PacketHello& _packetHello ) 
	{
		if( mState == HostConnection::Disconnected )
		{
            mName  = _packetHello.name;
            mState = HostConnection::NeedingApprouval;
		}
		else if( mState == HostConnection::Connected )
		{
            mState = HostConnection::NeedingApprouval;
			Debug::Log() << "host disconnected " << mIp.toString() << "::" << mPort << Debug::Endl();
		}
	}

	//========================================================================================================
	//========================================================================================================
    void HostConnection::ProcessPacket( const PacketPing& _packetPing,
                                        const FrameIndex _frameIndex,
                                        const float _logicDelta )
	{
        // number of frames elapsed between sending & receiving
		const FrameIndex delta = _frameIndex - _packetPing.serverFrame;
		const FrameIndex clientCurrentFrameIndex = _packetPing.clientFrame + delta / 2;

        mRtt = _logicDelta * delta;
        mFramesDelta[mNextDeltaIndex] = _frameIndex - clientCurrentFrameIndex;
        mNextDeltaIndex = ( mNextDeltaIndex + 1 ) % int( mFramesDelta.size() );
	}

	//========================================================================================================
	//========================================================================================================
	void HostConnection::OnSyncSuccess()
	{
        mSynced = true;
	}

	//========================================================================================================
	//========================================================================================================
	void HostConnection::OnLoginFail( const PacketTag  )
	{
		if( mState == HostConnection::PendingApprouval )
		{
			Debug::Log() << "host " << mIp.toString() << "::"
			             << mPort << " login failed, resending approval. " << Debug::Endl();
            mState = HostConnection::NeedingApprouval;
		}
	}

	//========================================================================================================
	//========================================================================================================
	void HostConnection::OnLoginSuccess( const PacketTag )
	{
		if( mState == HostConnection::PendingApprouval )
		{
			Debug::Log() << "host connected " << mIp.toString() << "::" << mPort << Debug::Endl();
            mState = HostConnection::Connected;
		}
	}

	//========================================================================================================
	// returns the perfect frame index timing for something to spawn immediately when sent from the server
	// to the client
	//========================================================================================================
	FrameIndex HostConnection::CalculatePerfectSpawnTiming( const HostConnection& _connection,
	                                                        const Time& _time )
	{
		int averageFrameDelta = 0;
		for( int delta : _connection.mFramesDelta )
		{
			averageFrameDelta += delta;
		}
		averageFrameDelta /= (int)_connection.mFramesDelta.size();

		return FrameIndex( -averageFrameDelta ) + FrameIndex( _connection.mRtt / _time.mLogicDelta ) + 3;
	}

	//========================================================================================================
	// Editor gui helper
	//========================================================================================================
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

	//========================================================================================================
	// returns a color corresponding to a rtt time in seconds
	//========================================================================================================
	static ImVec4 GetStateColor( const HostConnection::State _clientState )
	{
		switch( _clientState )
		{
		case HostConnection::Disconnected:		return  Color::sOrange.ToImGui(); break;
		case HostConnection::NeedingApprouval:	return  Color::sYellow.ToImGui(); break;
		case HostConnection::PendingApprouval:	return  Color::sYellow.ToImGui(); break;
		case HostConnection::Connected:			return  Color::sGreen.ToImGui(); break;
		default:			assert( false );	return  Color::sPurple.ToImGui(); break;
		}
	}

	//========================================================================================================
	//========================================================================================================
	void HostConnection::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		HostConnection& hostConnection = static_cast<HostConnection&>( _component );
		const double currentTime = Time::ElapsedSinceStartup();
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
		{
            ImGui::Text( "name:               %s", hostConnection.mName.c_str() );
            ImGui::TextColored( GetStateColor( hostConnection.mState ),
                                "state:              %s",
                                GetStateName( hostConnection.mState ).c_str() );
            ImGui::TextColored( GetRttColor( hostConnection.mRtt ),
                                "rtt:                %.1f",
                                1000.f * hostConnection.mRtt );
            ImGui::Text( "bandwidth :         %.1f Ko/s", hostConnection.mBandwidth );
            ImGui::Text( "last response:      %.1f", currentTime - hostConnection.mLastResponseTime );
            ImGui::Text( "adress:             %s::%u",
                         hostConnection.mIp.toString().c_str(),
                         hostConnection.mPort );
            ImGui::Text( "target buffer size: %d", hostConnection.mTargetBufferSize );
            ImGui::Text( "frame delta:        %d %d %d",
                         hostConnection.mFramesDelta[0],
                         hostConnection.mFramesDelta[1],
                         hostConnection.mFramesDelta[2] );
            ImGui::DragFloat( "ping delay", &hostConnection.mPingDelay, 0.1f, 0.f, 10.f );
            ImGui::DragFloat( "timeout time", &hostConnection.mTimeoutDelay, 0.1f, 0.f, 10.f );
		} ImGui::PopItemWidth();
	}
}