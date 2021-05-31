#include "engine/ui/fanUIText.hpp"
#include "core/resources/fanResources.hpp"
#include "render/resources/fanMesh2D.hpp"
#include "engine/fanEngineSerializable.hpp"
#include "engine/singletons/fanApplication.hpp"
#include "engine/fanEngineTags.hpp"

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
        Resources& resources = *_world.GetSingleton<Application>().mResources;

        UIText& text = static_cast<UIText&>( _component );
        text.mText    = "";
        text.mSize    = 18;
        text.mFontPtr = nullptr;

        const std::string name = "text_mesh_" + std::to_string( resources.GetUniqueID() );
        text.mMesh2D = resources.Get<Mesh2D>( name );
        if( text.mMesh2D == nullptr )
        {
            text.mMesh2D = resources.Add<Mesh2D>( new Mesh2D, name );
        }

        _world.AddTag<TagUIModified>( _entity );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UIText::Save( const EcsComponent& _component, Json& _json )
    {
        const UIText& text = static_cast<const UIText&>( _component );
        Serializable::SaveString( _json, "text", text.mText );
        Serializable::SaveInt( _json, "size", text.mSize );
        Serializable::SaveResourcePtr( _json, "font", text.mFontPtr.mData );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UIText::Load( EcsComponent& _component, const Json& _json )
    {
        UIText& text = static_cast<UIText&>( _component );
        Serializable::LoadString( _json, "text", text.mText );
        Serializable::LoadInt( _json, "size", text.mSize );
        Serializable::LoadResourcePtr( _json, "font", text.mFontPtr.mData );
    }
}