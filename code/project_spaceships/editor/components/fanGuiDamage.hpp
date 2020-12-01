#pragma once

#include "project_spaceships/game/components/fanDamage.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiDamage
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Joystick16;
            info.mGroup      = EngineGroups::Game;
            info.onGui       = &GuiDamage::OnGui;
            info.mEditorPath = "game/";
            info.mEditorName = "damage";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            Damage& damage = static_cast<Damage&>( _component );

            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
            {
                ImGui::DragFloat( "damage ##Damage", &damage.mDamage );
            }
            ImGui::PopItemWidth();
        }
    };
}