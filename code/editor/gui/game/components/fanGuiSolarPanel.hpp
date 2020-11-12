#pragma once

#include "game/components/fanSolarPanel.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    struct GuiSolarPanel
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Joystick16;
            info.mGroup      = EngineGroups::Game;
            info.onGui       = &GuiSolarPanel::OnGui;
            info.mEditorPath = "game/";
            info.mEditorName = "solar panel";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
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
            }
            ImGui::PopItemWidth();
        }
    };
}