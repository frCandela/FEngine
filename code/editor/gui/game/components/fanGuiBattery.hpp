#pragma once

#include "project_spaceships/components/fanBattery.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiBattery
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Energy16;
            info.mGroup      = EngineGroups::Game;
            info.onGui       = &GuiBattery::OnGui;
            info.mEditorName = "battery";
            info.mEditorPath = "game/";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            Battery& battery = static_cast<Battery&>( _component );

            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
            {
                ImGui::DragFloat( "current energy ##Battery", &battery.mCurrentEnergy );
                ImGui::DragFloat( "max energy     ##Battery", &battery.mMaxEnergy );
            }
            ImGui::PopItemWidth();
        }
    };
}