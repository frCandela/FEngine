#include "fanRenderResources.hpp"

#include "render/resources/fanMesh.hpp"
#include "render/resources/fanMesh2D.hpp"
#include "render/resources/fanTextureManager.hpp"
#include "render/resources/fanMesh2DManager.hpp"
#include "render/fanFont.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    void RenderResources::SetInfo( EcsSingletonInfo& _info )
    {
        _info.mIcon  = ImGui::RENDERER16;
        _info.mGroup = EngineGroups::SceneRender;
        _info.mName  = "render resources";
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
                                       TextureManager* _textureManager,
                                       Font* _font  )
    {
        mMeshManager   = _meshManager;
        mMesh2DManager = _mesh2DManager;
        mTextureManager = _textureManager;
        mFont = _font;
    }

    //========================================================================================================
    //========================================================================================================
    void RenderResources::SetupResources( MeshManager& _meshManager,
                                          Mesh2DManager& _mesh2DManager,
                                          TextureManager& _textureManager,
                                          Font& _font )
    {
        ResourcePtr<Mesh>::sOnResolve.Connect( &MeshManager::ResolvePtr, &_meshManager );
        ResourcePtr< Texture >::sOnResolve.Connect( &TextureManager::ResolvePtr, &_textureManager );

        _meshManager.Load( RenderGlobal::sDefaultMesh );
        Mesh2D* quad2D = RenderResources::CreateMesh2DQuad();
        _mesh2DManager.Add( quad2D, RenderGlobal::sMesh2DQuad );
        _textureManager.Load( RenderGlobal::sWhiteTexture );

        Font::InitFreetype();
        _font.LoadFont( RenderGlobal::sDefaultGameFont );
        _font.SetHeight( 34 );
        _textureManager.Add( _font.GenerateAtlas(), "font48" ) ;
    }

    //========================================================================================================
    //========================================================================================================
    Mesh2D* RenderResources::CreateMesh2DQuad()
    {
        Mesh2D* mesh2D = new Mesh2D();
        std::vector<UIVertex> vertices = {
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
