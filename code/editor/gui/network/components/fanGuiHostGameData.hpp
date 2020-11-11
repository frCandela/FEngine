#prama once

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