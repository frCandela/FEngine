#include "engine/components/fanMeshRenderer.hpp"
#include <sstream>
#include "core/memory/fanSerializable.hpp"
#include "engine/fanEngineSerializable.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void MeshRenderer::SetInfo( EcsComponentInfo& _info )
    {
        _info.load = &MeshRenderer::Load;
        _info.save = &MeshRenderer::Save;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void MeshRenderer::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        MeshRenderer& meshRenderer = static_cast<MeshRenderer&>( _component );
        meshRenderer.mMesh = nullptr;
        meshRenderer.mTransparent = false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void MeshRenderer::Save( const EcsComponent& _component, Json& _json )
    {
        const MeshRenderer& meshRenderer = static_cast<const MeshRenderer&>( _component );
        Serializable::SaveResourcePtr( _json, "path", meshRenderer.mMesh );
        Serializable::SaveBool( _json, "is_transparent", meshRenderer.mTransparent );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void MeshRenderer::Load( EcsComponent& _component, const Json& _json )
    {
        MeshRenderer& meshRenderer = static_cast<MeshRenderer&>( _component );
        Serializable::LoadResourcePtr( _json, "path", meshRenderer.mMesh );
        Serializable::LoadBool( _json, "is_transparent", meshRenderer.mTransparent );
    }
}