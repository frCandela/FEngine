#include "scene/components/ui/fanButtonUI.hpp"

#include "scene/fanSceneSerializable.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Button::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::NONE;
		_info.group = EngineGroups::SceneUI;
		_info.onGui = &Button::OnGui;
		_info.load = &Button::Load;
		_info.save = &Button::Save;
		_info.editorPath = "ui/";
		_info.name = "button ui";
	}

	//========================================================================================================
	//========================================================================================================
	void Button::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
        Button& button = static_cast<Button&>( _component );
        button.mColor = Color::White;
        button.mIsHovered = false;
        button.mPressed = false;
	}

	//========================================================================================================
	//========================================================================================================
	void Button::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
        Button& button = static_cast<Button&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
            ImGui::ColorEdit4( "color1", (float*)&button.mColor[0], ImGui::fanColorEditFlags );
            ImGui::PushReadOnly();
            ImGui::Checkbox("is hovered", &button.mIsHovered );
            ImGui::PopReadOnly();
		}
		ImGui::PopItemWidth();
	}

	//========================================================================================================
	//========================================================================================================
	void Button::Save( const EcsComponent& _component, Json& /*_json*/ )
	{
		const Button& button = static_cast<const Button&>( _component );
        (void)button;
		//Serializable::SaveComponentPtr( _json, "target_ui_transform", progressBar.targetUiTransform );
		//Serializable::SaveFloat( _json, "max_scale", progressBar.maxScale );
	}

	//========================================================================================================
	//========================================================================================================
	void Button::Load( EcsComponent& _component, const Json& /*_json*/ )
	{
        Button& button = static_cast<Button&>( _component );
        (void)button;
		//Serializable::LoadComponentPtr( _json, "target_ui_transform", progressBar.targetUiTransform );
		//Serializable::LoadFloat( _json, "max_scale", progressBar.maxScale );
	}
}