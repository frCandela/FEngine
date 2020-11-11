#prama once

#include "network/components/fanClientGameData.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void ClientGameData::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon  = ImGui::GameData16;
		_info.mGroup = EngineGroups::Network;
		_info.onGui  = &ClientGameData::OnGui;
		_info.mName  = "client game data";
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