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
		gameData.synced = false;
		gameData.inputs = std::deque< PacketInput >();					// clear
		gameData.previousStates = std::queue< PacketPlayerGameState >();// clear		
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientGameData::ProcessPacket( const PacketPlayerGameState& _packet )
	{
		// get the current input for this client
		while( !previousStates.empty() )
		{
			const PacketPlayerGameState& packetInput = previousStates.front();
			if( packetInput.frameIndex < _packet.frameIndex )
			{
				previousStates.pop();
			}
			else
			{
				break;
			}
		}

		// moves spaceship						
		if( !previousStates.empty() && previousStates.front().frameIndex == _packet.frameIndex )
		{
			const PacketPlayerGameState& packetState = previousStates.front();
			previousStates.pop();

			if( packetState != _packet )
			{
				Debug::Warning() << "player is out of sync" << Debug::Endl();
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientGameData::Write( Packet& _packet )
	{
		if( !inputs.empty() )
		{
			PacketInput& lastInput = inputs.front();
			lastInput.tag = _packet.tag;
			lastInput.Write( _packet );
			_packet.onSuccess.Connect( &ClientGameData::OnInputReceived, this );
		}
	}

	//================================================================================================================================
	// removes all inputs that have been received from the buffer
	//================================================================================================================================
	void ClientGameData::OnInputReceived( PacketTag _tag )
	{
		while( ! inputs.empty() )
		{
			const PacketInput& oldestInput = inputs.back();
			if( oldestInput.tag <= _tag )
			{
				inputs.pop_back();
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
		synced = true;
		Debug::Log() << "Shifted client frame index : " << _framesDelta << Debug::Endl();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientGameData::OnSpawnShip( NetID _spaceshipID, FrameIndex _frameIndex )
	{
		if( spaceshipNetID == 0 )
		{
			spaceshipSpawnFrameIndex = _frameIndex;
			spaceshipNetID = _spaceshipID;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientGameData::OnGui( EcsWorld& _world, EntityID _entityID, Component& _component )
	{
		ClientGameData& gameData = static_cast<ClientGameData&>( _component );
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
		{
			ImGui::Text( "spaceship spawn frame: %d", gameData.spaceshipSpawnFrameIndex);
			ImGui::Text( "spaceship net ID:      %d", gameData.spaceshipNetID);
			ImGui::Text( "size inputs:           %d", gameData.inputs.size() );
			ImGui::Text( "size previous states:  %d", gameData.previousStates.size());
			ImGui::Text( "%s", gameData.synced ? "synced" : "unsynced" );
		} ImGui::PopItemWidth();
	}
}