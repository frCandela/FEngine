#pragma once

#include "engine/ui/fanUIButton.hpp"
#include "engine/fanEngineTags.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/gui/fanGuiResourcePtr.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiUIButton
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::Button16;
            info.mGroup      = EngineGroups::SceneUI;
            info.onGui       = &GuiUIButton::OnGui;
            info.mEditorPath = "ui/";
            info.mEditorName = "Ui button";
            return info;
        }

        static void OnGui( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
        {
            UIButton& button = static_cast<UIButton&>( _component );

            ImGui::PushID( "uibutton" );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );

            bool isEnabled = _world.HasTag<TagEnabled>( _entity );
            if( ImGui::Checkbox( "enabled", &isEnabled ) )
            {
                if( isEnabled ){ _world.AddTag<TagEnabled>( _entity ); }
                else{ _world.RemoveTag<TagEnabled>( _entity ); }
            }

            if( ImGui::Button( "##color normal reset" ) ){ button.mColorNormal = Color::sDarkGrey; }
            ImGui::SameLine();
            ImGui::ColorEdit4( "color normal", (float*)&button.mColorNormal[0], ImGui::fanColorEditFlags );

            if( ImGui::Button( "##color hovered reset" ) ){ button.mColorHovered = Color::sGrey; }
            ImGui::SameLine();
            ImGui::ColorEdit4( "color hovered", (float*)&button.mColorHovered[0], ImGui::fanColorEditFlags );

            ImGui::FanResourcePtr<Texture>( "idle", button.mImageNormal);
            ImGui::FanResourcePtr<Texture>( "pressed", button.mImagePressed );

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