#include "fanGlobalIncludes.h"
#include "scene/components/fanUIMeshRenderer.h"

#include "renderer/fanRenderer.h"
#include "renderer/fanUIMesh.h"
#include "scene/components/fanTransform.h"
#include "scene/fanGameobject.h"
#include "renderer/core/fanTexture.h"

// Editor
#include "editor/fanModals.h"

namespace fan
{
	REGISTER_EDITOR_COMPONENT( UIMeshRenderer );
	REGISTER_TYPE_INFO( UIMeshRenderer )

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
		Component::OnGui();
		ImGui::ColorEdit4( "color", m_color.Data(), gui::colorEditFlags );






// 		bool openSetPathPopup = false;
// 		if ( ImGui::Button( "##setPathTex" ) )
// 		{
// 			openSetPathPopup = true;
// 		}
// 		ImGui::SameLine();
// 		const std::string texturePath = GetTexture() != nullptr ? GetTexture()->GetPath() : "not set";
// 		ImGui::Text( "texture : %s", texturePath.c_str() );
// 		// Set path  popup on double click
// 		if ( openSetPathPopup || ImGui::IsItemHovered() && ImGui::IsMouseClicked( 0 ) )
// 		{
// 			if ( GetTexture() != nullptr && GetTexture()->GetPath().empty() == false )
// 			{
// 				m_pathBuffer = std::fs::path( GetTexture()->GetPath() ).parent_path();
// 			}
// 			else
// 			{
// 				m_pathBuffer = "./";
// 			}
// 			ImGui::OpenPopup( "set_path_texture" );
// 			m_pathBuffer = "content/models";
// 		}
// 
// 		if ( gui::LoadFileModal( "set_path_texture", GlobalValues::s_imagesExtensions, m_pathBuffer ) )
// 		{
// 			SetTexturePath( m_pathBuffer.string() );
// 		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool UIMeshRenderer::Load( Json & /*_json*/ )
	{

		return true;
	}

	//==========================z======================================================================================================
	//================================================================================================================================
	bool UIMeshRenderer::Save( Json & _json ) const
	{
		Component::Save( _json );


		return true;
	}
}