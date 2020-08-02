#include "fanRenderResources.hpp"

#include "render/resources/fanMesh.hpp"
#include "render/core/fanTexture.hpp"
#include "render/fanRenderGlobal.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    void RenderResources::SetInfo( EcsSingletonInfo& _info )
    {
        _info.icon  = ImGui::RENDERER16;
        _info.group = EngineGroups::SceneRender;
        _info.name  = "render resources";
        _info.onGui = &RenderResources::OnGui;
    }

    //========================================================================================================
    //========================================================================================================
    void RenderResources::Init( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        RenderResources& renderResources = static_cast<RenderResources&>( _singleton );
        (void)renderResources;
    }

    //========================================================================================================
    //========================================================================================================
    void RenderResources::SetPointers( MeshManager* _meshManager,
                                       Mesh2DManager* _mesh2DManager,
                                       TextureManager* _textureManager )
    {
        mMeshManager   = _meshManager;
        mMesh2DManager = _mesh2DManager;
        mTextureManager = _textureManager;
    }

    //========================================================================================================
    //========================================================================================================
    void RenderResources::SetupResources( MeshManager& _meshManager,
                                          Mesh2DManager& _mesh2DManager,
                                          TextureManager& _textureManager )
    {
        ResourcePtr<Mesh>::s_onResolve.Connect( &MeshManager::ResolvePtr, &_meshManager );
        ResourcePtr< Texture >::s_onResolve.Connect( &TextureManager::ResolvePtr, &_textureManager );

        _meshManager.Load( RenderGlobal::sDefaultMesh );
        Mesh2D* quad2D = RenderResources::CreateMesh2DQuad();
        _mesh2DManager.Add( quad2D, RenderGlobal::sMesh2DQuad );
        _textureManager.Load( RenderGlobal::sWhiteTexture );
    }

    //========================================================================================================
    //========================================================================================================
    Mesh2D* RenderResources::CreateMesh2DQuad()
    {
        Mesh2D* mesh2D = new Mesh2D();
        std::vector<UIVertex> vertices = { // tmp make a 2D quad
                UIVertex( glm::vec2( +2.f, +0.f ), glm::vec2( +1.f, +0.f ) ),
                UIVertex( glm::vec2( +0.f, +0.f ), glm::vec2( +0.f, +0.f ) ),
                UIVertex( glm::vec2( +2.f, +2.f ), glm::vec2( +1.f, +1.f ) ),
                UIVertex( glm::vec2( +0.f, +0.f ), glm::vec2( +0.f, +0.f ) ),
                UIVertex( glm::vec2( +0.f, +2.f ), glm::vec2( +0.f, +1.f ) ),
                UIVertex( glm::vec2( +2.f, +2.f ), glm::vec2( +1.f, +1.f ) )
        };
        mesh2D->LoadFromVertices( vertices );
        return mesh2D;
    }

    //========================================================================================================
    //========================================================================================================
    void RenderResources::OnGui( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        RenderResources& renderResources = static_cast<RenderResources&>( _singleton );

        ImGui::Indent();
        ImGui::Indent();
        {
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
                for( const Texture         *  texture : textures )
                {
                    ImGui::Text( texture->mPath.c_str() );
                }
            }
        }
        ImGui::Unindent();
        ImGui::Unindent();
    }
}
