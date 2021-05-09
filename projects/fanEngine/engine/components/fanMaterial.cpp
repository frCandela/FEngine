#include "engine/components/fanMaterial.hpp"
#include "render/fanRenderSerializable.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Material::SetInfo( EcsComponentInfo& _info )
    {
        _info.load = &Material::Load;
        _info.save = &Material::Save;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Material::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        Material& material = static_cast<Material&>( _component );

        material.mTexture   = nullptr;
        material.mShininess = 1;
        material.mColor     = Color::sWhite;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Material::Save( const EcsComponent& _component, Json& _json )
    {
        const Material& material = static_cast<const Material&>( _component );

        Serializable::SaveUInt( _json, "shininess", material.mShininess );
        Serializable::SaveColor( _json, "color", material.mColor );
        Serializable::SaveTexturePtr( _json, "material", material.mTexture );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Material::Load( EcsComponent& _component, const Json& _json )
    {
        Material& material = static_cast<Material&>( _component );

        Serializable::LoadUInt( _json, "shininess", material.mShininess );
        Serializable::LoadColor( _json, "color", material.mColor );
        Serializable::LoadTexturePtr( _json, "material", material.mTexture );
    }
}