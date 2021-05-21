#include "core/ecs/fanEcsWorld.hpp"
#include "render/resources/fanMesh2D.hpp"
#include "render/resources/fanMesh2DManager.hpp"
#include "fanUIText.hpp"
#include "engine/fanEngineSerializable.hpp"
#include "engine/singletons/fanEngineResources.hpp"
#include "engine/fanSceneTags.hpp"
#include "render/fanRenderSerializable.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UIText::SetInfo( EcsComponentInfo& _info )
    {
        _info.load = &UIText::Load;
        _info.save = &UIText::Save;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UIText::Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
    {
        UIText& text = static_cast<UIText&>( _component );
        text.mText    = "";
        text.mMesh2D  = new Mesh2D();
        text.mSize    = 18;
        text.mFontPtr = nullptr;

        _world.GetSingleton<EngineResources>().mMesh2DManager->Add( text.mMesh2D, "text_mesh" );
        _world.AddTag<TagUIModified>( _entity );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UIText::Save( const EcsComponent& _component, Json& _json )
    {
        const UIText& text = static_cast<const UIText&>( _component );
        Serializable::SaveString( _json, "text", text.mText );
        Serializable::SaveInt( _json, "size", text.mSize );
        Serializable::SaveFontPtr( _json, "font", text.mFontPtr );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UIText::Load( EcsComponent& _component, const Json& _json )
    {
        UIText& text = static_cast<UIText&>( _component );
        Serializable::LoadString( _json, "text", text.mText );
        Serializable::LoadInt( _json, "size", text.mSize );
        Serializable::LoadFontPtr( _json, "font", text.mFontPtr );
    }
}