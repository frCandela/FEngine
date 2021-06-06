#include "engine/components/fanMeshSkinnedRenderer.hpp"
#include "core/memory/fanSerializable.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void MeshSkinnedRenderer::SetInfo( EcsComponentInfo& _info )
    {
        _info.load = &MeshSkinnedRenderer::Load;
        _info.save = &MeshSkinnedRenderer::Save;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void MeshSkinnedRenderer::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        MeshSkinnedRenderer& meshRenderer = static_cast<MeshSkinnedRenderer&>( _component );
        meshRenderer.mMesh = nullptr;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void MeshSkinnedRenderer::Save( const EcsComponent& _component, Json& _json )
    {
        const MeshSkinnedRenderer& meshRenderer = static_cast<const MeshSkinnedRenderer&>( _component );
        Serializable::SaveResourcePtr( _json, "mesh", meshRenderer.mMesh.mData );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void MeshSkinnedRenderer::Load( EcsComponent& _component, const Json& _json )
    {
        MeshSkinnedRenderer& meshRenderer = static_cast<MeshSkinnedRenderer&>( _component );
        Serializable::LoadResourcePtr( _json, "mesh", meshRenderer.mMesh.mData );
    }
}