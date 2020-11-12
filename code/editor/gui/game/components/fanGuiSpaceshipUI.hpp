#pragma once

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void SpaceshipUI::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::Spaceship16;
		_info.mGroup      = EngineGroups::Game;
		_info.onGui       = &SpaceshipUI::OnGui;
		_info.mEditorPath = "game/";
		_info.mName       = "spaceship ui";
	}

    //========================================================================================================
    //========================================================================================================
    void SpaceshipUI::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        SpaceshipUI& spaceshipUI = static_cast<SpaceshipUI&>( _component );

        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
        {
            ImGui::DragFloat2( "ui offset", &spaceshipUI.mUIOffset[0], 1.f );
            ImGui::FanComponent( "ui root transform", spaceshipUI.mUIRootTransform );
            ImGui::FanComponent( "health progress", spaceshipUI.mHealthProgress );
            ImGui::FanComponent( "energy progress", spaceshipUI.mEnergyProgress );
            ImGui::FanComponent( "signal progress", spaceshipUI.mSignalProgress );
            ImGui::FanComponent( "signal renderer", spaceshipUI.mSignalRenderer );
        } ImGui::PopItemWidth();
    }
}