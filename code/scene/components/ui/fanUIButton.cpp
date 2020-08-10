#include "scene/components/ui/fanUIButton.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void UIButton::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::BUTTON16;
		_info.group = EngineGroups::SceneUI;
		_info.onGui = &UIButton::OnGui;
		_info.load = &UIButton::Load;
		_info.save = &UIButton::Save;
		_info.editorPath = "ui/";
		_info.name = "ui button";
	}

	//========================================================================================================
	//========================================================================================================
	void UIButton::Init( EcsWorld& _world, EcsEntity /*_entity*/, EcsComponent& _component )
	{
        UIButton& button = static_cast<UIButton&>( _component );
        button.mColorHovered = Color::Grey;
        button.mColorPressed = Color::sDarkGrey;
        button.mIsHovered = false;
        button.mIsPressed = false;
        button.mSlotPtr.Init( _world, button.mPressed.GetType() );
        button.mPressed.Clear();
        button.mPressed.Connect( _world, button.mSlotPtr );
	}

	//========================================================================================================
	//========================================================================================================
	void UIButton::Save( const EcsComponent& _component, Json& _json )
	{
		const UIButton& button = static_cast<const UIButton&>( _component );
		Serializable::SaveColor( _json, "color_hovered", button.mColorHovered );
        Serializable::SaveColor( _json, "color_pressed", button.mColorPressed );
        Serializable::SaveSlotPtr( _json, "pressed", button.mSlotPtr );
	}

	//========================================================================================================
	//========================================================================================================
	void UIButton::Load( EcsComponent& _component, const Json& _json )
	{
        UIButton& button = static_cast<UIButton&>( _component );
        Serializable::LoadColor( _json, "color_hovered", button.mColorHovered );
        Serializable::LoadColor( _json, "color_pressed", button.mColorPressed );
        Serializable::LoadSlotPtr(  _json, "pressed", button.mSlotPtr );
	}

    //========================================================================================================
    //========================================================================================================
    void UIButton::OnGui( EcsWorld& _world, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        UIButton& button = static_cast<UIButton&>( _component );

        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
         if( ImGui::Button( "##color hovered reset" ) ){ button.mColorHovered = Color::Grey; }
         ImGui::SameLine();
         ImGui::ColorEdit4( "color hovered", (float*)&button.mColorHovered[0], ImGui::fanColorEditFlags );
         if( ImGui::Button( "##color pressed reset" ) ){ button.mColorPressed = Color::sDarkGrey; }
         ImGui::SameLine();
         ImGui::ColorEdit4( "color pressed", (float*)&button.mColorPressed[0], ImGui::fanColorEditFlags );
        ImGui::PopItemWidth();

         ImGui::PushReadOnly();
         ImGui::Checkbox("is hovered", &button.mIsHovered );
         ImGui::SameLine();
         ImGui::Checkbox("is pressed", &button.mIsPressed );
         ImGui::PopReadOnly();

         ImGui::FanSlotPtr("pressed", _world, button.mSlotPtr );
    }
}