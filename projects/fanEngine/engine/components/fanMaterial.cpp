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
        material.mMaterials.resize(1);
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Material::Save( const EcsComponent& _component, Json& _json )
    {
        const Material& material = static_cast<const Material&>( _component );
        Json & jsonMaterials = _json["materials"];
        for( int i = 0; i < (int)material.mMaterials.size(); i++)
        {
            Serializable::SaveUInt(         jsonMaterials[i], "shininess",  material.mMaterials[i].mShininess );
            Serializable::SaveColor(        jsonMaterials[i], "color",      material.mMaterials[i].mColor );
            Serializable::SaveTexturePtr(   jsonMaterials[i], "material",   material.mMaterials[i].mTexture );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Material::Load( EcsComponent& _component, const Json& _json )
    {
        Material& material = static_cast<Material&>( _component );

        auto it = _json.find("materials");
        if( it != _json.end() )
        {
            const Json & jsonMaterials = _json["materials"];
            material.mMaterials.resize( jsonMaterials.size() );
            for( int i = 0; i < (int)material.mMaterials.size(); i++)
            {
                Serializable::LoadUInt(         jsonMaterials[i], "shininess",  material.mMaterials[i].mShininess );
                Serializable::LoadColor(        jsonMaterials[i], "color",      material.mMaterials[i].mColor );
                Serializable::LoadTexturePtr(   jsonMaterials[i], "material",   material.mMaterials[i].mTexture );
            }
        }
        else
        {
            material.mMaterials.resize( 1 );
            Serializable::LoadUInt(        _json , "shininess",  material.mMaterials[0].mShininess );
            Serializable::LoadColor(       _json , "color",      material.mMaterials[0].mColor );
            Serializable::LoadTexturePtr(  _json , "material",   material.mMaterials[0].mTexture );
        }
    }
}