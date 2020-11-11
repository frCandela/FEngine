#prama once

#include "scene/singletons/fanRenderResources.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    void RenderResources::SetInfo( EcsSingletonInfo& _info )
    {
        _info.mIcon  = ImGui::Renderer16;
        _info.mGroup = EngineGroups::SceneRender;
        _info.mName  = "render resources";
        _info.onGui = &RenderResources::OnGui;
    }

    //========================================================================================================
    //========================================================================================================
    void RenderResources::OnGui( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        RenderResources& renderResources = static_cast<RenderResources&>( _singleton );
        if( ImGui::CollapsingHeader( "meshes" ) )
        {
            const std::vector<Mesh*>& meshes = renderResources.mMeshManager->GetMeshes();
            for( const Mesh         * mesh : meshes )
            {
                ImGui::Text( mesh->mPath.c_str() );
            }
        }

        if( ImGui::CollapsingHeader( "meshes2D" ) )
        {
            const std::vector<Mesh2D*>& meshes = renderResources.mMesh2DManager->GetMeshes();
            for( const Mesh2D         * mesh : meshes )
            {
                ImGui::Text( mesh->mPath.c_str() );
            }
        }

        if( ImGui::CollapsingHeader( "textures" ) )
        {
            const std::vector<Texture*>& textures = renderResources.mTextureManager->GetTextures();
            for( const Texture         * texture : textures )
            {
                ImGui::Text( texture->mPath.c_str() );
            }
        }
    }
}
