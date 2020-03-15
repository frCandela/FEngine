#include "scene/ecs/components/fanUIRenderer.hpp"

#include "render/fanRenderSerializable.hpp"
#include "core/input/fanInput.hpp"

namespace fan
{
	REGISTER_COMPONENT( UIRenderer2, "ui_renderer" );

	//================================================================================================================================
	//================================================================================================================================
	void UIRenderer2::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::UI_MESH_RENDERER16;
		_info.onGui = &UIRenderer2::OnGui;
		_info.init = &UIRenderer2::Init;
		_info.load = &UIRenderer2::Load;
		_info.save = &UIRenderer2::Save;
		_info.editorPath = "/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIRenderer2::Init( ecComponent& _component )
	{
		UIRenderer2& uiRenderer = static_cast<UIRenderer2&>( _component );

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
	void UIRenderer2::OnGui( ecComponent& _component )
	{
		UIRenderer2& ui = static_cast<UIRenderer2&>( _component );

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
	void UIRenderer2::Save( const ecComponent& _component, Json& _json )
	{
		const UIRenderer2& ui = static_cast<const UIRenderer2&>( _component );
		Serializable::SaveColor( _json, "color", ui.color );
		Serializable::SaveTexturePtr( _json, "texture_path", ui.texture );
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIRenderer2::Load( ecComponent& _component, const Json& _json )
	{
		UIRenderer2& ui = static_cast<UIRenderer2&>( _component );
		Serializable::LoadColor( _json, "color", ui.color );
		Serializable::LoadTexturePtr( _json, "texture_path", ui.texture );
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::ivec2	UIRenderer2::GetTextureSize() const 
	{ 
		return *texture != nullptr ? texture->GetSize() : glm::ivec2( 0, 0 ); 
	}
}