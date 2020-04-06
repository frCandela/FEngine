#include "scene/components/ui/fanUIRenderer.hpp"

#include "render/fanRenderSerializable.hpp"
#include "core/input/fanInput.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
	REGISTER_COMPONENT( UIRenderer, "ui_renderer" );

	//================================================================================================================================
	//================================================================================================================================
	void UIRenderer::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::UI_MESH_RENDERER16;
		_info.onGui = &UIRenderer::OnGui;
		_info.init = &UIRenderer::Init;
		_info.load = &UIRenderer::Load;
		_info.save = &UIRenderer::Save;
		_info.editorPath = "ui/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIRenderer::Init( EcsWorld& _world, Component& _component )
	{
		UIRenderer& uiRenderer = static_cast<UIRenderer&>( _component );

		if( uiRenderer.uiMesh.GetVertexBuffer() == nullptr )
		{
			std::vector<UIVertex> vertices = { // tmp make a 2D quad
				UIVertex( glm::vec2( +2.f, +0.f ), glm::vec2( +1.f, +0.f ) )
				,UIVertex( glm::vec2( +0.f, +0.f ), glm::vec2( +0.f, +0.f ) )
				,UIVertex( glm::vec2( +2.f, +2.f ), glm::vec2( +1.f, +1.f ) )
				,UIVertex( glm::vec2( +0.f, +0.f ), glm::vec2( +0.f, +0.f ) )
				,UIVertex( glm::vec2( +0.f, +2.f ), glm::vec2( +0.f, +1.f ) )
				,UIVertex( glm::vec2( +2.f, +2.f ), glm::vec2( +1.f, +1.f ) )
			};
			uiRenderer.uiMesh.LoadFromVertices( vertices );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIRenderer::OnGui( EcsWorld& _world, EntityID _entityID, Component& _component )
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

		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIRenderer::Save( const Component& _component, Json& _json )
	{
		const UIRenderer& ui = static_cast<const UIRenderer&>( _component );
		Serializable::SaveColor( _json, "color", ui.color );
		Serializable::SaveTexturePtr( _json, "texture_path", ui.texture );
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIRenderer::Load( Component& _component, const Json& _json )
	{
		UIRenderer& ui = static_cast<UIRenderer&>( _component );
		Serializable::LoadColor( _json, "color", ui.color );
		Serializable::LoadTexturePtr( _json, "texture_path", ui.texture );
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::ivec2	UIRenderer::GetTextureSize() const 
	{ 
		return *texture != nullptr ? texture->GetSize() : glm::ivec2( 0, 0 ); 
	}
}