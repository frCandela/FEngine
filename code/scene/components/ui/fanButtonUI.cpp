#include "scene/components/ui/fanButtonUI.hpp"
#include "editor/fanModals.hpp"
#include "scene/fanSceneSerializable.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Button::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::BUTTON16;
		_info.group = EngineGroups::SceneUI;
		_info.onGui = &Button::OnGui;
		_info.load = &Button::Load;
		_info.save = &Button::Save;
		_info.editorPath = "ui/";
		_info.name = "button ui";
	}

	//========================================================================================================
	//========================================================================================================
	void Button::Init( EcsWorld& _world, EcsEntity /*_entity*/, EcsComponent& _component )
	{
        Button& button = static_cast<Button&>( _component );
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
	void Button::Save( const EcsComponent& _component, Json& _json )
	{
		const Button& button = static_cast<const Button&>( _component );
		Serializable::SaveColor( _json, "color_hovered", button.mColorHovered );
        Serializable::SaveColor( _json, "color_pressed", button.mColorPressed );
        Serializable::SaveSlotPtr( _json, "pressed", button.mSlotPtr );
	}

	//========================================================================================================
	//========================================================================================================
	void Button::Load( EcsComponent& _component, const Json& _json )
	{
        Button& button = static_cast<Button&>( _component );
        Serializable::LoadColor( _json, "color_hovered", button.mColorHovered );
        Serializable::LoadColor( _json, "color_pressed", button.mColorPressed );
        Serializable::LoadSlotPtr(  _json, "pressed", button.mSlotPtr );
	}

    //========================================================================================================
    //========================================================================================================
    void Button::OnGui( EcsWorld& _world, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        Button& button = static_cast<Button&>( _component );

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