#include "network/components/fanHostGameData.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void HostGameData::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon  = ImGui::GameData16;
		_info.mGroup = EngineGroups::Network;
		_info.onGui  = &HostGameData::OnGui;
		_info.mName  = "host game data";
	}

	//========================================================================================================
	//========================================================================================================
	void HostGameData::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		HostGameData& hostGameData = static_cast<HostGameData&>( _component );
		hostGameData.mSpaceshipID          = 0;
		hostGameData.mSpaceshipHandle      = 0;
		hostGameData.mInputs               = std::queue<PacketInput::InputData>();
		hostGameData.mNextPlayerState      = PacketPlayerGameState();
		hostGameData.mNextPlayerStateFrame = 0;
	}

	//========================================================================================================
	//========================================================================================================
	void HostGameData::ProcessPacket( PacketInput& _packet )
	{
		if( mInputs.empty() )
		{
			for ( const PacketInput::InputData& inputData : _packet.mInputs )
			{
				mInputs.push( inputData );
			}			
		}
		else
		{
			for ( const PacketInput::InputData& inputData : _packet.mInputs )
			{
				const FrameIndex mostRecentFrame = mInputs.back().mFrameIndex;
				//fanAssert( inputData.frameIndex <= mostRecentFrame + 1 );
				if( inputData.mFrameIndex == mostRecentFrame + 1 )
				{
					mInputs.push( inputData );
				} 				
			}
		}	
	}

	//========================================================================================================
	//========================================================================================================
	void HostGameData::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		HostGameData& hostGameData = static_cast<HostGameData&>( _component );
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
		{
			ImGui::Text( "spaceshipID :      %u", hostGameData.mSpaceshipID );
			ImGui::Text( "spaceship handle : %u", hostGameData.mSpaceshipHandle );
			ImGui::Text( "inputs size :      %u", hostGameData.mInputs.size() );

			if( ImGui::CollapsingHeader( "inputs" ) )
			{
				std::queue< PacketInput::InputData > inputsCpy = hostGameData.mInputs;
				while( !inputsCpy.empty() )
				{
					ImGui::Text( "%d", inputsCpy.front().mFrameIndex );
					inputsCpy.pop();
				}
			}
		} ImGui::PopItemWidth();
	}
}