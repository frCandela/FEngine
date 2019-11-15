#include "fanGlobalIncludes.h"
#include "scene/components/fanUIMeshRenderer.h"

#include "renderer/fanRenderer.h"
#include "renderer/fanUIMesh.h"
#include "scene/components/fanTransform.h"
#include "scene/fanGameobject.h"
#include "renderer/core/fanTexture.h"
#include "core/input/fanInput.h"

// Editor
#include "editor/fanModals.h"

namespace fan
{
	REGISTER_TYPE_INFO( UIMeshRenderer, TypeInfo::Flags::EDITOR_COMPONENT )

	//================================================================================================================================
	//================================================================================================================================
	void UIMeshRenderer::OnAttach()
	{
		m_uiMesh = new UIMesh();
		std::vector<UIVertex> vertices = {
			 UIVertex( glm::vec2( -1.f, -1.f), glm::vec2( +0.f, +0.f) )			
			,UIVertex( glm::vec2( +1.f, +1.f), glm::vec2( +1.f, +1.f) )
			,UIVertex( glm::vec2( +1.f, -1.f), glm::vec2( +1.f, +0.f) )
			,UIVertex( glm::vec2( -1.f, -1.f), glm::vec2( +0.f, +0.f) )
			,UIVertex( glm::vec2( -1.f, +1.f), glm::vec2( +0.f, +1.f) )
			,UIVertex( glm::vec2( +1.f, +1.f), glm::vec2( +1.f, +1.f) )

		};
		m_uiMesh->LoadFromVertices(vertices);

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
	void UIMeshRenderer::OnGui()
	{
		 Transform * transfom = m_gameobject->GetTransform();
		 const btVector2 windowSize = Input::Get().WindowSize();

		Component::OnGui();
		ImGui::PushItemWidth(0.6f * ImGui::GetWindowWidth() ); {

			// color
			ImGui::ColorEdit4( "color", m_color.Data(), ImGui::fanColorEditFlags );
			if ( ImGui::FanTexture( "ui texture", &m_texture ) )
			{
				btVector2 textureSize = *m_texture != nullptr ? btVector2( (float)m_texture->GetSize().x, (float)m_texture->GetSize().y ) : btVector3::Zero();
				const btVector2 newScale = textureSize / windowSize;
				transfom->SetScale( btVector3( newScale.x(), newScale.y(), transfom->GetScale().z() ) );
			}

		} ImGui::PopItemWidth();

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
		{
			btVector2 textureSize = *m_texture != nullptr ? btVector2( (float)m_texture->GetSize().x, (float)m_texture->GetSize().y ) : btVector3::Zero();
			
			// pixel size reset		
			if ( ImGui::Button( "##reset ui size" ) )
			{
				const btVector2 newScale = textureSize / windowSize;
				transfom->SetScale( btVector3( newScale.x(), newScale.y(), transfom->GetScale().z() ) );
			} ImGui::SameLine();

			// pixel size
			btVector2 pixelSize = transfom->GetScale() * windowSize;
			if ( ImGui::DragFloat2( "pixel size", &pixelSize[0], 1.f, 0.f, 10000.f ) )
			{
				const btVector2 newScale = pixelSize / windowSize;
				transfom->SetScale( btVector3( newScale.x(), newScale.y(), transfom->GetScale().z() ) );
			}

			// pixel pos reset		
			if ( ImGui::Button( "##reset ui pos" ) )
			{
				const btVector2 newScale = textureSize / windowSize;
				transfom->SetPosition( btVector3( 0,0, transfom->GetPosition().z() ) );
			} ImGui::SameLine();

			//pixel pos
			btVector2 pixelPos = 0.5f *( transfom->GetPosition() + btVector3::One() ) * windowSize;
			if ( ImGui::DragFloat2( "pixel position", &pixelPos[0] ) )
			{
				const btVector2 newPosition = 2.f * pixelPos / windowSize - btVector3::One();
				transfom->SetPosition( btVector3( newPosition.x(), newPosition.y(), transfom->GetPosition().z() ) );
			}

		} ImGui::PopItemWidth();


	}

	//================================================================================================================================
	//================================================================================================================================
	bool UIMeshRenderer::Load( Json & _json )
	{
		LoadColor(  _json,	"color",			m_color);
		LoadTexturePtr(_json, "texture_path", m_texture );

		return true;
	}

	//==========================z======================================================================================================
	//================================================================================================================================
	bool UIMeshRenderer::Save( Json & _json ) const
	{
		Component::Save( _json );

		SaveColor( _json, "color", m_color );
		SaveTexturePtr( _json, "texture_path", m_texture );

		return true;
	}
}