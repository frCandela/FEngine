#pragma once

#include "game/components/fanHealth.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiHealth
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Heart16;
            info.mGroup      = EngineGroups::Game;
            info.onGui       = &GuiHealth::OnGui;
            info.mEditorName = "Health";
            info.mEditorPath = "game/";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
        {
            Health& health = static_cast<Health&>( _component );
            ImGui::PushID( "Weapon" );
            if( ImGui::DragFixed( "max health", &health.mMaxHealth, 1, 1, 100 ) )
            {
                health.mHealth = health.mMaxHealth;
            }
            ImGui::DragFixed( "health", &health.mHealth, 1, 0, health.mMaxHealth.ToFloat() );
            ImGui::PopID();
        }
    };
}