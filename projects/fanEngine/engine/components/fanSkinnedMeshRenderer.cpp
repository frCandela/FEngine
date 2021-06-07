#include "engine/components/fanSkinnedMeshRenderer.hpp"
#include "core/memory/fanSerializable.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SkinnedMeshRenderer::SetInfo( EcsComponentInfo& _info )
    {
        _info.load = &SkinnedMeshRenderer::Load;
        _info.save = &SkinnedMeshRenderer::Save;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SkinnedMeshRenderer::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        SkinnedMeshRenderer& meshRenderer = static_cast<SkinnedMeshRenderer&>( _component );
        meshRenderer.mMesh = nullptr;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SkinnedMeshRenderer::Save( const EcsComponent& _component, Json& _json )
    {
        const SkinnedMeshRenderer& meshRenderer = static_cast<const SkinnedMeshRenderer&>( _component );
        Serializable::SaveResourcePtr( _json, "mesh", meshRenderer.mMesh.mData );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SkinnedMeshRenderer::Load( EcsComponent& _component, const Json& _json )
    {
        SkinnedMeshRenderer& meshRenderer = static_cast<SkinnedMeshRenderer&>( _component );
        Serializable::LoadResourcePtr( _json, "mesh", meshRenderer.mMesh.mData );
    }
}