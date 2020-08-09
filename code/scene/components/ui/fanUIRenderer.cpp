#include "scene/components/ui/fanUIRenderer.hpp"

#include "render/fanRenderSerializable.hpp"
#include "render/resources/fanMesh2DManager.hpp"
#include "core/input/fanInput.hpp"
#include "scene/singletons/fanRenderResources.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void UIRenderer::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::UI_MESH_RENDERER16;
		_info.group = EngineGroups::SceneUI;
		_info.onGui = &UIRenderer::OnGui;
		_info.load = &UIRenderer::Load;
		_info.save = &UIRenderer::Save;
		_info.editorPath = "ui/";
		_info.name = "ui renderer";
		_info.mSlots.push_back( new Slot<>( "show", &UIRenderer::Show ));
        _info.mSlots.push_back( new Slot<>( "hide", &UIRenderer::Hide ));
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIRenderer::Init( EcsWorld& _world, EcsEntity /*_entity*/, EcsComponent& _component )
	{
        RenderResources& renderResources = _world.GetSingleton<RenderResources>();

		UIRenderer& uiRenderer = static_cast<UIRenderer&>( _component );
        uiRenderer.mUiMesh = renderResources.mMesh2DManager->Get( RenderGlobal::sMesh2DQuad );
        uiRenderer.mVisible = true;
        uiRenderer.color = Color::White;
        fanAssert( uiRenderer.mUiMesh );
	}

    //================================================================================================================================
    //================================================================================================================================
    void UIRenderer::Save( const EcsComponent& _component, Json& _json )
    {
        const UIRenderer& ui = static_cast<const UIRenderer&>( _component );
        Serializable::SaveColor( _json, "color", ui.color );
        Serializable::SaveTexturePtr( _json, "texture_path", ui.texture );
    }

    //================================================================================================================================
    //================================================================================================================================
    void UIRenderer::Load( EcsComponent& _component, const Json& _json )
    {
        UIRenderer& ui = static_cast<UIRenderer&>( _component );
        Serializable::LoadColor( _json, "color", ui.color );
        Serializable::LoadTexturePtr( _json, "texture_path", ui.texture );
    }

    //================================================================================================================================
    //================================================================================================================================
    glm::ivec2	UIRenderer::GetTextureSize() const
    {
        return *texture != nullptr ? glm::ivec2( texture->mExtent.width, texture->mExtent.height ) : glm::ivec2( 0, 0 );
    }

    //================================================================================================================================
    //================================================================================================================================
    void UIRenderer::Show( EcsComponent& _component )
    {
        UIRenderer& ui = (UIRenderer&)_component;
        ui.mVisible = true;
    }
    //================================================================================================================================
    //================================================================================================================================
    void UIRenderer::Hide( EcsComponent& _component )
    {
        UIRenderer& ui = (UIRenderer&)_component;
        ui.mVisible   = false;
    }


    //================================================================================================================================
	//================================================================================================================================
	void UIRenderer::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		UIRenderer& ui = static_cast<UIRenderer&>( _component );

		const btVector2 windowSize = Input::Get().WindowSizeF();

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			// color
			if( ImGui::Button( "##TransPos" ) )
			{
				ui.color = Color::White;
			} ImGui::SameLine();
			ImGui::ColorEdit4( "color", ui.color.Data(), ImGui::fanColorEditFlags );

			// texture
			ImGui::FanTexturePtr( "ui texture", ui.texture );
			ImGui::Checkbox("visible", &ui.mVisible );

		} ImGui::PopItemWidth();
	}
}