#include "scene/components/ui/fanUIButton.hpp"
#include "editor/fanModals.hpp"
#include "render/fanRenderSerializable.hpp"
#include "scene/fanSceneTags.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void UIButton::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::Button16;
		_info.mGroup      = EngineGroups::SceneUI;
		_info.onGui       = &UIButton::OnGui;
		_info.load        = &UIButton::Load;
		_info.save        = &UIButton::Save;
		_info.mEditorPath = "ui/";
		_info.mName       = "ui button";
	}

	//========================================================================================================
	//========================================================================================================
	void UIButton::Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
	{
        UIButton& button = static_cast<UIButton&>( _component );
        button.mIsHovered = false;
        button.mIsPressed = false;
        button.mColorNormal = Color::sWhite;
        button.mColorHovered = Color::sGrey;
        button.mImageNormal = nullptr;
        button.mImagePressed = nullptr;
        button.mSlotPtr.Init( _world, button.mPressed.GetType() );
        button.mPressed.Clear();
        button.mPressed.Connect( _world, button.mSlotPtr );

        _world.AddTag<TagUIEnabled>( _entity );
	}

	//========================================================================================================
	//========================================================================================================
	void UIButton::Save( const EcsComponent& _component, Json& _json )
	{
		const UIButton& button = static_cast<const UIButton&>( _component );

        Serializable::SaveColor( _json, "color_normal", button.mColorNormal );
		Serializable::SaveColor( _json, "color_hovered", button.mColorHovered );
        Serializable::SaveTexturePtr( _json, "image_normal", button.mImageNormal );
        Serializable::SaveTexturePtr( _json, "image_pressed", button.mImagePressed );
        Serializable::SaveSlotPtr( _json, "pressed_slot_ptr", button.mSlotPtr );
	}

	//========================================================================================================
	//========================================================================================================
	void UIButton::Load( EcsComponent& _component, const Json& _json )
	{
        UIButton& button = static_cast<UIButton&>( _component );
        Serializable::LoadColor( _json, "color_normal", button.mColorNormal );
        Serializable::LoadColor( _json, "color_hovered", button.mColorHovered );
        Serializable::LoadTexturePtr( _json, "image_normal", button.mImageNormal );
        Serializable::LoadTexturePtr( _json, "image_pressed", button.mImagePressed );
        Serializable::LoadSlotPtr( _json, "pressed_slot_ptr", button.mSlotPtr );
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