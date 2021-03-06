#include "engine/ui/fanUIButton.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "engine/fanEngineSerializable.hpp"
#include "engine/fanEngineTags.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UIButton::SetInfo( EcsComponentInfo& _info )
    {
        _info.load = &UIButton::Load;
        _info.save = &UIButton::Save;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UIButton::Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
    {
        UIButton& button = static_cast<UIButton&>( _component );
        button.mIsHovered    = false;
        button.mIsPressed    = false;
        button.mColorNormal  = Color::sWhite;
        button.mColorHovered = Color::sGrey;
        button.mImageNormal  = nullptr;
        button.mImagePressed = nullptr;;
        button.mPressed.Clear();

        _world.AddTag<TagEnabled>( _entity );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UIButton::Save( const EcsComponent& _component, Json& _json )
    {
        const UIButton& button = static_cast<const UIButton&>( _component );

        Serializable::SaveColor( _json, "color_normal", button.mColorNormal );
        Serializable::SaveColor( _json, "color_hovered", button.mColorHovered );
        Serializable::SaveResourcePtr( _json, "image_normal", button.mImageNormal );
        Serializable::SaveResourcePtr( _json, "image_pressed", button.mImagePressed );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UIButton::Load( EcsComponent& _component, const Json& _json )
    {
        UIButton& button = static_cast<UIButton&>( _component );
        Serializable::LoadColor( _json, "color_normal", button.mColorNormal );
        Serializable::LoadColor( _json, "color_hovered", button.mColorHovered );
        Serializable::LoadResourcePtr( _json, "image_normal", button.mImageNormal );
        Serializable::LoadResourcePtr( _json, "image_pressed", button.mImagePressed );
    }
}