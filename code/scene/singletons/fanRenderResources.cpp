#include "scene/singletons/fanRenderResources.hpp"
#include "render/resources/fanMesh.hpp"
#include "render/resources/fanMesh2D.hpp"
#include "render/resources/fanTextureManager.hpp"
#include "render/resources/fanMesh2DManager.hpp"
#include "render/resources/fanFontManager.hpp"
#include "render/resources/fanFont.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    void RenderResources::SetInfo( EcsSingletonInfo& /*_info*/ )
    {
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
                                       FontManager* _fontManager  )
    {
        mMeshManager   = _meshManager;
        mMesh2DManager = _mesh2DManager;
        mTextureManager = _textureManager;
        mFontManager = _fontManager;
    }

    //========================================================================================================
    //========================================================================================================
    void RenderResources::SetupResources( MeshManager& _meshManager,
                                          Mesh2DManager& _mesh2DManager,
                                          TextureManager& _textureManager,
                                          FontManager& _fontManager )
    {
        ResourcePtr<Mesh>::sOnResolve.Connect( &MeshManager::ResolvePtr, &_meshManager );
        ResourcePtr< Texture >::sOnResolve.Connect( &TextureManager::ResolvePtr, &_textureManager );
        ResourcePtr< Font >::sOnResolve.Connect( &FontManager::ResolvePtr, &_fontManager );

        _fontManager.Load( RenderGlobal::sDefaultGameFont );

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
}
