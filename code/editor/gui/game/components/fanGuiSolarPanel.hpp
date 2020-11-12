#pragma once

namespace fan
{	
	//========================================================================================================
	//========================================================================================================
	void SolarPanel::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::Joystick16;
		_info.mGroup      = EngineGroups::Game;
		_info.onGui       = &SolarPanel::OnGui;
		_info.mEditorPath = "game/";
		_info.mName       = "solar panel";
	}

    //========================================================================================================
    //========================================================================================================
    void SolarPanel::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        SolarPanel& solarPanel = static_cast<SolarPanel&>( _component );

        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
        {
            ImGui::DragFloat( "min charging rate", &solarPanel.mMinChargingRate, 0.5f, 0.f, 100.f );
            ImGui::DragFloat( "max charging rate", &solarPanel.mMaxChargingRate, 0.5f, 0.f, 100.f );
            ImGui::DragFloat( "low range		", &solarPanel.mLowRange, 0.5f, 0.f, 100.f );
            ImGui::DragFloat( "high range 		", &solarPanel.mHighRange, 0.5f, 0.f, 100.f );
            ImGui::Spacing();
            ImGui::PushReadOnly();
            ImGui::Checkbox( "is inside sunlight", &solarPanel.mIsInSunlight );
            ImGui::DragFloat( "charging rate", &solarPanel.mCurrentChargingRate, 0.5f, 0.f, 100.f );
            ImGui::PopReadOnly();
        } ImGui::PopItemWidth();
    }
}