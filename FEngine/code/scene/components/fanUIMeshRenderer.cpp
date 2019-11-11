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
		ImGui::InputTexture( "ui texture", &m_texture );
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