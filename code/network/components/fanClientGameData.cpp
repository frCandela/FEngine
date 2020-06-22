#include "network/components/fanClientGameData.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "network/singletons/fanTime.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void ClientGameData::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::NETWORK16;
		_info.onGui = &ClientGameData::OnGui;
		_info.name = "client game data";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientGameData::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		ClientGameData& gameData = static_cast<ClientGameData&>( _component );
		gameData.spaceshipHandle = 0;
		gameData.frameSynced = false;
		gameData.previousInputs					  = std::deque< PacketInput::InputData >();	// clear
		gameData.previousLocalStates					  = std::queue< PacketPlayerGameState >();	// clear	
		gameData.maxInputSent = 10;
		gameData.spaceshipSynced = true;
		gameData.lastServerState = {};

	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientGameData::ProcessPacket( const PacketPlayerGameState& _packet )
	{
		lastServerState = _packet;

		// removes all the previous rollback state before the gameState
//		while( !previousInputsSinceLastGameState.empty() )
//		{
// 			const PacketInput::InputData& input = previousInputsSinceLastGameState.back();
// 			if( input.frameIndex < _packet.frameIndex )
// 			{
// 				previousInputsSinceLastGameState.pop_back();
// 			}
// 			else
// 			{
// 				break;
// 			}
//		} 

		// get the corresponding game state for the client
		while( !previousLocalStates.empty() && previousLocalStates.front().frameIndex < _packet.frameIndex )
		{
			previousLocalStates.pop();
		}

		// compares the server state & client state to verify we are synchronized				
		if( !previousLocalStates.empty() && previousLocalStates.front().frameIndex == _packet.frameIndex )
		{
			const PacketPlayerGameState& packetState = previousLocalStates.front();
			previousLocalStates.pop();

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
	void ClientGameData::Write( EcsWorld& _world, EcsEntity _entity,  Packet& _packet )
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
			_packet.onSuccess.Connect( &ClientGameData::OnInputReceived, _world, _world.GetHandle(_entity) );
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
		previousLocalStates = std::queue< PacketPlayerGameState >(); // clear
		frameSynced = true;

		if( std::abs( _framesDelta ) > Time::s_maxFrameDeltaBeforeShift )
		{
			Debug::Log() << "Shifted client frame index : " << _framesDelta << Debug::Endl();
			previousLocalStates = std::queue< PacketPlayerGameState >(); // clear
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientGameData::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		ClientGameData& gameData = static_cast<ClientGameData&>( _component );
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
		{
			ImGui::DragInt( "max input sent", &gameData.maxInputSent, 1.f, 0, 200 );
			ImGui::Text( "size previous states:  %d", gameData.previousLocalStates.size());
			ImGui::Text( "%s", gameData.frameSynced ? "frame synced" : "frame not synced" );
			ImGui::Text( "size pending inputs:  %d", gameData.previousInputs.size() );
			ImGui::Text( "size inputs sent:      %d", gameData.inputsSent.size() );
			
		} ImGui::PopItemWidth();
	}
}