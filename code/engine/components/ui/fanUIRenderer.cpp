#include "engine/components/ui/fanUIRenderer.hpp"
#include "render/fanRenderSerializable.hpp"
#include "render/resources/fanMesh2DManager.hpp"
#include "engine/singletons/fanRenderResources.hpp"
#include "engine/fanSceneTags.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void UIRenderer::SetInfo( EcsComponentInfo& _info )
	{
		_info.load        = &UIRenderer::Load;
		_info.save        = &UIRenderer::Save;
	}

	//========================================================================================================
	//========================================================================================================
	void UIRenderer::Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
	{
        RenderResources& renderResources = _world.GetSingleton<RenderResources>();

		UIRenderer& uiRenderer = static_cast<UIRenderer&>( _component );
        uiRenderer.mMesh2D  = renderResources.mMesh2DManager->Get( RenderGlobal::sMesh2DQuad );
        uiRenderer.mColor   = Color::sWhite;
        uiRenderer.mDepth   = 0;
        fanAssert( uiRenderer.mMesh2D );

        _world.AddTag< TagUIVisible >( _entity );
	}

    //========================================================================================================
    //========================================================================================================
    void UIRenderer::Save( const EcsComponent& _component, Json& _json )
    {
        const UIRenderer& ui = static_cast<const UIRenderer&>( _component );
        Serializable::SaveColor( _json, "color", ui.mColor );
        Serializable::SaveTexturePtr( _json, "texture_path", ui.mTexture );
        Serializable::SaveInt( _json, "depth", ui.mDepth );
    }

    //========================================================================================================
    //========================================================================================================
    void UIRenderer::Load( EcsComponent& _component, const Json& _json )
    {
        UIRenderer& ui = static_cast<UIRenderer&>( _component );
        Serializable::LoadColor( _json, "color", ui.mColor );
        Serializable::LoadTexturePtr( _json, "texture_path", ui.mTexture );
        Serializable::LoadInt( _json, "depth", ui.mDepth );
    }

    //========================================================================================================
    //========================================================================================================
    glm::ivec2	UIRenderer::GetTextureSize() const
    {
        return *mTexture != nullptr ?
                            glm::ivec2( mTexture->mExtent.width, mTexture->mExtent.height ) :
                            glm::ivec2( 0, 0 );
    }
}