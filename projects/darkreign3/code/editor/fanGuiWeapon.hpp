#pragma once

#include "game/components/fanWeapon.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiWeapon
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::None16;
            info.mGroup      = EngineGroups::Game;
            info.onGui       = &GuiWeapon::OnGui;
            info.mEditorName = "Weapon";
            info.mEditorPath = "game/";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
        {
            Weapon& weapon = static_cast<Weapon&>( _component );
            ImGui::PushID( "Weapon" );
            ImGui::DragFixed( "damage", &weapon.mDamage, 1, 0, 100 );
            ImGui::DragFixed( "rate of fire", &weapon.mRateOfFire );
            ImGui::FanToolTip( "shoots/s" );
            ImGui::PopID();
        }
    };
}