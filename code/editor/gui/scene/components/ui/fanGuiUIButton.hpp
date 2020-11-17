#pragma once

#include "scene/components/ui/fanUIButton.hpp"
#include "scene/fanSceneTags.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"
#include "editor/gui/fanGuiSlot.hpp"
#include "editor/gui/fanGuiRenderResources.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    struct GuiUIButton
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::Button16;
            info.mGroup      = EngineGroups::SceneUI;
            info.onGui       = &GuiUIButton::OnGui;
            info.mEditorPath = "ui/";
            info.mEditorName       = "ui button";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
        {
            UIButton& button = static_cast<UIButton&>( _component );

            ImGui::PushID( "uibutton" );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );

            bool isEnabled = _world.HasTag<TagUIEnabled>( _entity );
            if( ImGui::Checkbox( "enabled", &isEnabled ) )
            {
                if( isEnabled ){ _world.AddTag<TagUIEnabled>( _entity ); }
                else{ _world.RemoveTag<TagUIEnabled>( _entity ); }
            }

            if( ImGui::Button( "##color normal reset" ) ){ button.mColorNormal = Color::sDarkGrey; }
            ImGui::SameLine();
            ImGui::ColorEdit4( "color normal", (float*)&button.mColorNormal[0], ImGui::fanColorEditFlags );

            if( ImGui::Button( "##color hovered reset" ) ){ button.mColorHovered = Color::sGrey; }
            ImGui::SameLine();
            ImGui::ColorEdit4( "color hovered", (float*)&button.mColorHovered[0], ImGui::fanColorEditFlags );

            ImGui::FanTexturePtr( "idle", button.mImageNormal );
            ImGui::FanTexturePtr( "pressed", button.mImagePressed );
            ImGui::FanSlotPtr( "on pressed", _world, button.mSlotPtr );

            ImGui::PopItemWidth();

            ImGui::PushReadOnly();
            ImGui::Checkbox( "is hovered", &button.mIsHovered );
            ImGui::SameLine();
            ImGui::Checkbox( "is pressed", &button.mIsPressed );
            ImGui::PopReadOnly();

            ImGui::PopID();
        }
    };
}