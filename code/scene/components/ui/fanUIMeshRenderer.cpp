#include "scene/components/ui/fanUIMeshRenderer.hpp"
#include "scene/components/fanTransform.hpp"
#include "core/input/fanInput.hpp"
#include "render/core/fanTexture.hpp"
#include "render/fanUIMesh.hpp"
#include "core/imgui/fanModals.hpp"

namespace fan
{
	REGISTER_TYPE_INFO( UIMeshRenderer, TypeInfo::Flags::EDITOR_COMPONENT, "ui/" )

		//================================================================================================================================
		//================================================================================================================================
		void UIMeshRenderer::OnAttach()
	{
		m_uiMesh = new UIMesh();
		std::vector<UIVertex> vertices = {
			UIVertex( glm::vec2( +2.f, +0.f ), glm::vec2( +1.f, +0.f ) )
			,UIVertex( glm::vec2( +0.f, +0.f ), glm::vec2( +0.f, +0.f ) )
			,UIVertex( glm::vec2( +2.f, +2.f ), glm::vec2( +1.f, +1.f ) )
			,UIVertex( glm::vec2( +0.f, +0.f ), glm::vec2( +0.f, +0.f ) )
			,UIVertex( glm::vec2( +0.f, +2.f ), glm::vec2( +0.f, +1.f ) )
			,UIVertex( glm::vec2( +2.f, +2.f ), glm::vec2( +1.f, +1.f ) )

		};
		m_uiMesh->LoadFromVertices( vertices );

		Component::OnAttach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIMeshRenderer::OnDetach()
	{
		Component::OnDetach();

		delete m_uiMesh;
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::ivec2	UIMeshRenderer::GetTextureSize() const
	{
		return *m_texture != nullptr ? m_texture->GetSize() : glm::ivec2( 0, 0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIMeshRenderer::OnGui()
	{
		const btVector2 windowSize = Input::Get().WindowSizeF();

		Component::OnGui();
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{

			// visible
			ImGui::Checkbox( "visible", &m_visible );

			// color
			ImGui::ColorEdit4( "color", m_color.Data(), ImGui::fanColorEditFlags );
			if ( ImGui::FanTexturePtr( "ui texture", m_texture ) )
			{
				btVector2 textureSize = *m_texture != nullptr ? btVector2( ( float ) m_texture->GetSize().x, ( float ) m_texture->GetSize().y ) : btVector3::Zero();
				const btVector2 newScale = textureSize / windowSize;
				SetPixelSize( GetTextureSize() );
				SetPixelPosition( Input::Get().WindowSize() / 2 );
			}

		} ImGui::PopItemWidth();

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
		{
			// pixel size reset		
			if ( ImGui::Button( "Reset size##reset ui size" ) ) { SetPixelSize( GetTextureSize() ); }
		} ImGui::PopItemWidth();


	}

	//================================================================================================================================
	//================================================================================================================================
	void UIMeshRenderer::SetPixelPosition( const glm::ivec2 _position )
	{
		Transform& transform = m_gameobject->GetTransform();
		const btVector2 newPosition = 2.f * btVector2( ( float ) _position.x, ( float ) _position.y ) / Input::Get().WindowSizeF() - btVector3::One();
		transform.SetPosition( btVector3( newPosition.x(), newPosition.y(), transform.GetPosition().z() ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIMeshRenderer::SetPixelSize( const glm::ivec2 _size )
	{
		Transform& transform = m_gameobject->GetTransform();
		btVector2 screenSize = btVector2( ( float ) _size.x, ( float ) _size.y ) / Input::Get().WindowSizeF();
		transform.SetScale( btVector3( screenSize.x(), screenSize.y(), transform.GetScale().z() ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	bool UIMeshRenderer::Load( const Json& _json )
	{
		Serializable::LoadColor( _json, "color", m_color );
		Serializable::LoadTexturePtr( _json, "texture_path", m_texture );
		Serializable::LoadBool( _json, "visible", m_visible );

		return true;
	}

	//==========================z======================================================================================================
	//================================================================================================================================
	bool UIMeshRenderer::Save( Json& _json ) const
	{
		Component::Save( _json );

		Serializable::SaveColor( _json, "color", m_color );
		Serializable::SaveTexturePtr( _json, "texture_path", m_texture );
		Serializable::SaveBool( _json, "visible", m_visible );

		return true;
	}
}