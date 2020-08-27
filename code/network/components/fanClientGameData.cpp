#include "network/components/fanClientGameData.hpp"
#include "network/singletons/fanTime.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void ClientGameData::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::GAME_DATA16;
		_info.group = EngineGroups::Network;
		_info.onGui = &ClientGameData::OnGui;
		_info.name = "client game data";
	}

	//========================================================================================================
	//========================================================================================================
	void ClientGameData::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		ClientGameData& gameData = static_cast<ClientGameData&>( _component );
		gameData.sSpaceshipHandle     = 0;
		gameData.mFrameSynced         = false;
		gameData.mPreviousInputs      = std::deque<PacketInput::InputData >();	// clear
		gameData.mPreviousLocalStates = std::queue<PacketPlayerGameState >();	// clear
		gameData.mMaxInputSent        = 10;
		gameData.mSpaceshipSynced     = true;
		gameData.mLastServerState     = {};
		gameData.mPlayerId            = 0;

	}

	//========================================================================================================
	//========================================================================================================
	void ClientGameData::ProcessPacket( const PacketPlayerGameState& _packet )
	{
        mLastServerState = _packet; // @todo store multiple server states to allow deeper rollback

		// get the corresponding game state for the client
		while( !mPreviousLocalStates.empty() && mPreviousLocalStates.front().frameIndex < _packet.frameIndex )
		{
			mPreviousLocalStates.pop();
		}

		// compares the server state & client state to verify we are synchronized				
		if( !mPreviousLocalStates.empty() && mPreviousLocalStates.front().frameIndex == _packet.frameIndex )
		{
			const PacketPlayerGameState& packetState = mPreviousLocalStates.front();
			mPreviousLocalStates.pop();

			if( packetState != _packet )
			{
				Debug::Warning() << "player is out of sync" << Debug::Endl();
                mSpaceshipSynced = false;

				if( packetState.frameIndex != _packet.frameIndex )
				{
					Debug::Log() << "frame index difference: " << packetState.frameIndex
					             << " " << _packet.frameIndex << Debug::Endl();
				}
				if( !( packetState.position - _packet.position ).fuzzyZero() )
				{
					const btVector3 diff = packetState.position - _packet.position;
					Debug::Log() << "position difference: "
					             << diff[0] << " " << diff[1] << " " << diff[2] << Debug::Endl();
				}
				if( !( packetState.orientation - _packet.orientation ).fuzzyZero() )
				{
					const btVector3 diff = packetState.orientation - _packet.orientation;
					Debug::Log() << "orientation difference: "
					             << diff[0] << " " << diff[1] << " " << diff[2] << Debug::Endl();
				}
				if( !( packetState.velocity - _packet.velocity ).fuzzyZero() )
				{
					const btVector3 diff = packetState.velocity - _packet.velocity;
					Debug::Log() << "velocity difference: "
					             << diff[0] << " " << diff[1] << " " << diff[2] << Debug::Endl();
				}
				if( !( packetState.angularVelocity - _packet.angularVelocity ).fuzzyZero() )
				{
					const btVector3 diff = packetState.angularVelocity - _packet.angularVelocity;
					Debug::Log() << "angular velocity difference: "
					             << diff[0] << " " << diff[1] << " " << diff[2] << Debug::Endl();
				}

			}
		}
	}

	//========================================================================================================
	//========================================================================================================
	void ClientGameData::Write( EcsWorld& _world, EcsEntity _entity,  Packet& _packet )
	{
		// calculates the number of inputs to send
		int numInputs = (int)mPreviousInputs.size();
		if( numInputs > mMaxInputSent )
		{
			numInputs = mMaxInputSent;
		}

		if( numInputs > 0 )
		{
			// registers packet success
			_packet.onSuccess.Connect( &ClientGameData::OnInputReceived, _world, _world.GetHandle(_entity) );
			mInputsSent.push_front( { _packet.tag, mPreviousInputs.front().frameIndex } );

			// generate & send inputs
			PacketInput packetInput;
			packetInput.inputs.resize( numInputs );
			for( int i = 0; i < numInputs; i++ )
			{
				packetInput.inputs[numInputs - i - 1] = * ( mPreviousInputs.begin() + i) ;
			}				
			packetInput.Write( _packet );			
		}
	}

	//========================================================================================================
	// removes all inputs that have been received from the buffer
	//========================================================================================================
	void ClientGameData::OnInputReceived( PacketTag _tag )
	{
		while( !mInputsSent.empty() )
		{
			const InputSent inputSent = mInputsSent.back();
			if( inputSent.mTag < _tag ) // packets were lost but we don't care
			{
				mInputsSent.pop_back();
			}
			else if( inputSent.mTag == _tag )
			{
				mInputsSent.pop_back();
				// input packet was received, remove all corresponding inputs from the buffer
				while( !mPreviousInputs.empty() )
				{
					const PacketInput::InputData packetInput = mPreviousInputs.back();
					if( packetInput.frameIndex <= inputSent.mMostRecentFrame )
					{
						mPreviousInputs.pop_back();
					}
					else
					{
						break;
					}
				}
				break;
			}
			else
			{
				break;
			}
		}
	}

	//========================================================================================================
	//========================================================================================================
	void ClientGameData::OnShiftFrameIndex( const int _framesDelta )
	{
        mPreviousLocalStates = std::queue<PacketPlayerGameState >(); // clear
		mFrameSynced         = true;

		if( std::abs( _framesDelta ) > Time::sMaxFrameDeltaBeforeShift )
		{
			Debug::Log() << "Shifted client frame index : " << _framesDelta << Debug::Endl();
            mPreviousLocalStates = std::queue<PacketPlayerGameState >(); // clear
		}
	}

	//========================================================================================================
	//========================================================================================================
	void ClientGameData::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		ClientGameData& gameData = static_cast<ClientGameData&>( _component );
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
		{
			ImGui::Text( "player ID           : %u", gameData.mPlayerId );
			ImGui::DragInt( "max input sent", &gameData.mMaxInputSent, 1.f, 0, 200 );
			ImGui::Text( "size previous states:  %d", gameData.mPreviousLocalStates.size());
			ImGui::Text( "%s", gameData.mFrameSynced ? "frame synced" : "frame not synced" );
			ImGui::Text( "size pending inputs:  %d", gameData.mPreviousInputs.size() );
			ImGui::Text( "size inputs sent:      %d", gameData.mInputsSent.size() );
			
		} ImGui::PopItemWidth();
	}
}