#pragma once

#include "scene/components/ui/fanUIButton.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void UIButton::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::Button16;
		_info.mGroup      = EngineGroups::SceneUI;
		_info.onGui       = &UIButton::OnGui;
		_info.mEditorPath = "ui/";
		_info.mName       = "ui button";
	}

    //========================================================================================================
    //========================================================================================================
    void UIButton::OnGui( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
    {
        UIButton& button = static_cast<UIButton&>( _component );

        ImGui::PushID("uibutton");
        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );

        bool isEnabled = _world.HasTag<TagUIEnabled>( _entity);
        if( ImGui::Checkbox("enabled", &isEnabled ) )
        {
            if( isEnabled ) { _world.AddTag<TagUIEnabled>(_entity);     }
            else            { _world.RemoveTag<TagUIEnabled>(_entity);  }
        }

        if( ImGui::Button( "##color normal reset" ) ){ button.mColorNormal = Color::sDarkGrey; }
        ImGui::SameLine();
        ImGui::ColorEdit4( "color normal", (float*)&button.mColorNormal[0], ImGui::fanColorEditFlags );

        if( ImGui::Button( "##color hovered reset" ) ){ button.mColorHovered = Color::sGrey; }
        ImGui::SameLine();
        ImGui::ColorEdit4( "color hovered", (float*)&button.mColorHovered[0], ImGui::fanColorEditFlags );

        ImGui::FanTexturePtr( "idle", button.mImageNormal );
        ImGui::FanTexturePtr( "pressed", button.mImagePressed );
        ImGui::FanSlotPtr("on pressed", _world, button.mSlotPtr );

        ImGui::PopItemWidth();

         ImGui::PushReadOnly();
         ImGui::Checkbox("is hovered", &button.mIsHovered );
         ImGui::SameLine();
         ImGui::Checkbox("is pressed", &button.mIsPressed );
         ImGui::PopReadOnly();


         ImGui::PopID();
    }
}