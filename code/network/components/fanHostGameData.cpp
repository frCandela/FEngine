#include "network/components/fanHostGameData.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void HostGameData::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::NETWORK16;
		_info.onGui = &HostGameData::OnGui;
		_info.name = "host game data";
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostGameData::Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
	{
		HostGameData& hostGameData = static_cast<HostGameData&>( _component );
		hostGameData.spaceshipID = 0;
		hostGameData.spaceshipHandle = 0;
		hostGameData.inputs = std::queue<PacketInput::InputData>();
		hostGameData.nextPlayerState = PacketPlayerGameState();
		hostGameData.nextPlayerStateFrame = 0;
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostGameData::ProcessPacket( PacketInput& _packet )
	{
		if( inputs.empty() )
		{
			for ( const PacketInput::InputData& inputData : _packet.inputs )
			{
				inputs.push( inputData );
			}			
		}
		else
		{
			for ( const PacketInput::InputData& inputData : _packet.inputs )
			{
				const FrameIndex mostRecentFrame = inputs.back().frameIndex;
				//assert( inputData.frameIndex <= mostRecentFrame + 1 );
				if( inputData.frameIndex == mostRecentFrame + 1 )
				{
					inputs.push( inputData );
				} 				
			}
		}	
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostGameData::OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component )
	{
		HostGameData& hostGameData = static_cast<HostGameData&>( _component );
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
		{
			ImGui::Text( "spaceshipID :      %u", hostGameData.spaceshipID );
			ImGui::Text( "spaceship handle : %u", hostGameData.spaceshipHandle );
			ImGui::Text( "inputs size :      %u", hostGameData.inputs.size() );

			if( ImGui::CollapsingHeader( "inputs" ) )
			{
				std::queue< PacketInput::InputData > inputsCpy = hostGameData.inputs;
				while( !inputsCpy.empty() )
				{
					ImGui::Text( "%d", inputsCpy.front().frameIndex );
					inputsCpy.pop();
				}
			}

		} ImGui::PopItemWidth();
	}
}