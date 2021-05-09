#include "fanUIButton.hpp"
#include "editor/fanModals.hpp"
#include "render/fanRenderSerializable.hpp"
#include "engine/fanSceneTags.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void UIButton::SetInfo( EcsComponentInfo& _info )
	{
		_info.load        = &UIButton::Load;
		_info.save        = &UIButton::Save;
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
}