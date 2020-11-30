#pragma once

#include "project_spaceships/components/fanHealth.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiHealth
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Heart16;
            info.mGroup      = EngineGroups::Game;
            info.onGui       = &GuiHealth::OnGui;
            info.mEditorPath = "game/";
            info.mEditorName = "health";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            Health& health = static_cast<Health&>( _component );

            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
            {
                ImGui::SliderFloat( "current health", &health.mCurrentHealth, 0.f, health.mMaxHealth );
                ImGui::DragFloat( "max health", &health.mMaxHealth );
                ImGui::Checkbox( "invincible", &health.mInvincible );
            }
            ImGui::PopItemWidth();
        }
    };
}