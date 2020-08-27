#include "scene/components/ui/fanUIRenderer.hpp"
#include "render/fanRenderSerializable.hpp"
#include "render/resources/fanMesh2DManager.hpp"
#include "scene/singletons/fanRenderResources.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void UIRenderer::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::UI_MESH_RENDERER16;
		_info.mGroup      = EngineGroups::SceneUI;
		_info.onGui       = &UIRenderer::OnGui;
		_info.load        = &UIRenderer::Load;
		_info.save        = &UIRenderer::Save;
		_info.mEditorPath = "ui/";
		_info.mName       = "ui renderer";
	}

	//========================================================================================================
	//========================================================================================================
	void UIRenderer::Init( EcsWorld& _world, EcsEntity /*_entity*/, EcsComponent& _component )
	{
        RenderResources& renderResources = _world.GetSingleton<RenderResources>();

		UIRenderer& uiRenderer = static_cast<UIRenderer&>( _component );
        uiRenderer.mMesh2D  = renderResources.mMesh2DManager->Get( RenderGlobal::sMesh2DQuad );
        uiRenderer.mVisible = true;
        uiRenderer.mColor   = Color::sWhite;
        uiRenderer.mDepth   = 0;
        fanAssert( uiRenderer.mMesh2D );
	}

    //========================================================================================================
    //========================================================================================================
    void UIRenderer::Save( const EcsComponent& _component, Json& _json )
    {
        const UIRenderer& ui = static_cast<const UIRenderer&>( _component );
        Serializable::SaveColor( _json, "color", ui.mColor );
        Serializable::SaveTexturePtr( _json, "texture_path", ui.mTexture );
        Serializable::SaveBool( _json, "visible", ui.mVisible );
        Serializable::SaveInt( _json, "depth", ui.mDepth );
    }

    //========================================================================================================
    //========================================================================================================
    void UIRenderer::Load( EcsComponent& _component, const Json& _json )
    {
        UIRenderer& ui = static_cast<UIRenderer&>( _component );
        Serializable::LoadColor( _json, "color", ui.mColor );
        Serializable::LoadTexturePtr( _json, "texture_path", ui.mTexture );
        Serializable::LoadBool( _json, "visible", ui.mVisible );
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

    //========================================================================================================
	//========================================================================================================
	void UIRenderer::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		UIRenderer& ui = static_cast<UIRenderer&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			// color
			if( ImGui::Button( "##TransPos" ) )
			{
				ui.mColor = Color::sWhite;
			} ImGui::SameLine();
			ImGui::ColorEdit4( "color", ui.mColor.Data(), ImGui::fanColorEditFlags );

			// texture
			ImGui::FanTexturePtr( "ui texture", ui.mTexture );
			ImGui::Checkbox("visible", &ui.mVisible );
            if( ImGui::Button( "##depth ui" ) ){	ui.mDepth = 0 ; }
            ImGui::SameLine();
			ImGui::DragInt("depth", &ui.mDepth, 1, 0, 1024 );

		} ImGui::PopItemWidth();
	}
}