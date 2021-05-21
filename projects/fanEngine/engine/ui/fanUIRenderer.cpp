#include "fanUIRenderer.hpp"
#include "render/fanRenderSerializable.hpp"
#include "render/resources/fanMesh2DManager.hpp"
#include "render/fanRenderGlobal.hpp"
#include "engine/singletons/fanEngineResources.hpp"
#include "engine/fanSceneTags.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UIRenderer::SetInfo( EcsComponentInfo& _info )
    {
        _info.load = &UIRenderer::Load;
        _info.save = &UIRenderer::Save;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UIRenderer::Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
    {
        EngineResources& engineResources = _world.GetSingleton<EngineResources>();

        UIRenderer& uiRenderer = static_cast<UIRenderer&>( _component );
        uiRenderer.mMesh2D = engineResources.mMesh2DManager->Get( RenderGlobal::sMesh2DQuad );
        uiRenderer.mColor  = Color::sWhite;
        uiRenderer.mUvOffset = {0,0};
        uiRenderer.mTiling = {1,1};
        uiRenderer.mDepth  = 0;
        fanAssert( uiRenderer.mMesh2D );

        _world.AddTag<TagUIVisible>( _entity );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UIRenderer::Save( const EcsComponent& _component, Json& _json )
    {
        const UIRenderer& ui = static_cast<const UIRenderer&>( _component );
        Serializable::SaveColor( _json, "color", ui.mColor );
        Serializable::SaveTexturePtr( _json, "texture_path", ui.mTexture );
        Serializable::SaveInt( _json, "depth", ui.mDepth );
        Serializable::SaveIVec2( _json, "uv_offset", ui.mUvOffset );
        Serializable::SaveIVec2( _json, "tiling", ui.mTiling );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UIRenderer::Load( EcsComponent& _component, const Json& _json )
    {
        UIRenderer& ui = static_cast<UIRenderer&>( _component );
        Serializable::LoadColor( _json, "color", ui.mColor );
        Serializable::LoadTexturePtr( _json, "texture_path", ui.mTexture );
        Serializable::LoadInt( _json, "depth", ui.mDepth );
        Serializable::LoadIVec2( _json, "uv_offset", ui.mUvOffset );
        Serializable::LoadIVec2( _json, "tiling", ui.mTiling );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    glm::ivec2 UIRenderer::GetTextureSize() const
    {
        return *mTexture != nullptr ?
                glm::ivec2( mTexture->mExtent.width, mTexture->mExtent.height ) :
                glm::ivec2( 0, 0 );
    }
}