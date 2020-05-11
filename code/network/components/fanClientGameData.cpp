#include "network/components/fanClientGameData.hpp"

namespace fan
{
	REGISTER_COMPONENT( ClientGameData, "client game data" );

	//================================================================================================================================
	//================================================================================================================================
	void ClientGameData::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::NETWORK16;
		_info.onGui = &ClientGameData::OnGui;
		_info.init =  &ClientGameData::Init;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientGameData::Init( EcsWorld& _world, Component& _component )
	{
		ClientGameData& gameData = static_cast<ClientGameData&>( _component );
		gameData.spaceshipSpawnFrameIndex = 0;
		gameData.spaceshipNetID = 0;
		gameData.spaceshipHandle = 0;
		gameData.frameSynced = false;
		gameData.previousInputs					  = std::deque< PacketInput::InputData >();	// clear
		gameData.previousInputsSinceLastGameState = std::deque< PacketInput::InputData >();	// clear
		gameData.previousStates					  = std::queue< PacketPlayerGameState >();	// clear	
		gameData.maxInputSent = 10;
		gameData.spaceshipSynced = true;
		gameData.lastServerState = {};

	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientGameData::ProcessPacket( const PacketPlayerGameState& _packet )
	{
		lastServerState = _packet;

		// removes all the previous inputs before the gameState
		while( !previousInputsSinceLastGameState.empty() )
		{
			const PacketInput::InputData& input = previousInputsSinceLastGameState.back();
			if( input.frameIndex < _packet.frameIndex )
			{
				previousInputsSinceLastGameState.pop_back();
			}
			else
			{
				break;
			}
		} 

		// get the corresponding game state for the client
		while( !previousStates.empty() )
		{
			const PacketPlayerGameState& clientState = previousStates.front();
			if( clientState.frameIndex < _packet.frameIndex )
			{
				previousStates.pop();
			}
			else
			{
				break;
			}
		}

		// compares the server state & client state to verify we are synchronized				
		if( !previousStates.empty() && previousStates.front().frameIndex == _packet.frameIndex )
		{
			const PacketPlayerGameState& packetState = previousStates.front();
			previousStates.pop();

			if( packetState != _packet )
			{
				Debug::Warning() << "player is out of sync" << Debug::Endl();
				spaceshipSynced = false;

				if( packetState.frameIndex != _packet.frameIndex )
				{
					Debug::Log() << "frame index difference: " << packetState.frameIndex << " " << _packet.frameIndex << Debug::Endl();
				}
				if( !( packetState.position - _packet.position ).fuzzyZero() )
				{
					const btVector3 diff = packetState.position - _packet.position;
					Debug::Log() << "position difference: " << diff[0] << " " << diff[1] << " " << diff[2] << Debug::Endl();
				}
				if( !( packetState.orientation - _packet.orientation ).fuzzyZero() )
				{
					const btVector3 diff = packetState.orientation - _packet.orientation;
					Debug::Log() << "orientation difference: " << diff[0] << " " << diff[1] << " " << diff[2] << Debug::Endl();
				}
				if( !( packetState.velocity - _packet.velocity ).fuzzyZero() )
				{
					const btVector3 diff = packetState.velocity - _packet.velocity;
					Debug::Log() << "velocity difference: " << diff[0] << " " << diff[1] << " " << diff[2] << Debug::Endl();
				}
				if( !( packetState.angularVelocity - _packet.angularVelocity ).fuzzyZero() )
				{
					const btVector3 diff = packetState.angularVelocity - _packet.angularVelocity;
					Debug::Log() << "angular velocity difference: " << diff[0] << " " << diff[1] << " " << diff[2] << Debug::Endl();
				}

			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientGameData::Write( Packet& _packet )
	{
		// calculates the number of inputs to send
		int numInputs = (int)previousInputs.size();
		if( numInputs > maxInputSent )
		{
			numInputs = maxInputSent;
		}

		if( numInputs > 0 )
		{
			// registers packet success
			_packet.onSuccess.Connect( &ClientGameData::OnInputReceived, this );
			inputsSent.push_front( { _packet.tag, previousInputs.front().frameIndex } );

			// generate & send inputs
			PacketInput packetInput;
			packetInput.inputs.resize( numInputs );
			for( int i = 0; i < numInputs; i++ )
			{
				packetInput.inputs[numInputs - i - 1] = * (previousInputs.begin() + i) ;
			}				
			packetInput.Write( _packet );			
		}
	}

	//================================================================================================================================
	// removes all inputs that have been received from the buffer
	//================================================================================================================================
	void ClientGameData::OnInputReceived( PacketTag _tag )
	{
		while( !inputsSent.empty() )
		{
			const InputSent inputSent = inputsSent.back();
			if( inputSent.tag < _tag ) // packets were lost but we don't care
			{
				inputsSent.pop_back();
			}
			else if( inputSent.tag == _tag ) 
			{
				inputsSent.pop_back();
				// input packet was received, remove all corresponding inputs from the buffer
				while( !previousInputs.empty() )
				{
					const PacketInput::InputData packetInput = previousInputs.back();
					if( packetInput.frameIndex <= inputSent.mostRecentFrame )
					{
						previousInputs.pop_back();
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

	//================================================================================================================================
	//================================================================================================================================
	void ClientGameData::OnShiftFrameIndex( const int _framesDelta )
	{
		previousStates = std::queue< PacketPlayerGameState >(); // clear
		frameSynced = true;

		if( std::abs( _framesDelta ) > maxFrameDeltaBeforeShift )
		{
			Debug::Log() << "Shifted client frame index : " << _framesDelta << Debug::Endl();
			previousStates = std::queue< PacketPlayerGameState >(); // clear
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientGameData::OnSpawnShip( NetID _spaceshipID, FrameIndex _frameIndex )
	{
		if( spaceshipNetID == 0 )
		{
			spaceshipSpawnFrameIndex = _frameIndex;
			spaceshipNetID = _spaceshipID;
			spaceshipSynced = true;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientGameData::OnGui( EcsWorld& _world, EntityID _entityID, Component& _component )
	{
		ClientGameData& gameData = static_cast<ClientGameData&>( _component );
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
		{
			ImGui::DragInt( "max input sent", &gameData.maxInputSent, 1.f, 0, 200 );
			ImGui::Text( "spaceship spawn frame: %d", gameData.spaceshipSpawnFrameIndex);
			ImGui::Text( "spaceship net ID:      %d", gameData.spaceshipNetID);
			ImGui::Text( "size previous states:  %d", gameData.previousStates.size());
			ImGui::Text( "%s", gameData.frameSynced ? "frame synced" : "frame not synced" );
			ImGui::Text( "size pennding inputs:  %d", gameData.previousInputs.size() );
			ImGui::Text( "size inputs sent:      %d", gameData.inputsSent.size() );
			ImGui::Text( "size previous inputs:  %d", gameData.previousInputsSinceLastGameState.size() );
			
		} ImGui::PopItemWidth();
	}
}