#pragma once

#include "project_spaceships/game/components/fanBullet.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiBullet
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Joystick16;
            info.mGroup      = EngineGroups::Game;
            info.onGui       = &GuiBullet::OnGui;
            info.mEditorPath = "game/";
            info.mEditorName = "bullet";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            Bullet& bullet = static_cast<Bullet&>( _component );

            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
            {
                ImGui::FanPrefab( "explosion prefab", bullet.mExplosionPrefab );
            }
            ImGui::PopItemWidth();
        }
    };
}