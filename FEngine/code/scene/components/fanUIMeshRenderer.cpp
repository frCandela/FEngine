#include "fanGlobalIncludes.h"
#include "scene/components/fanUIMeshRenderer.h"

#include "renderer/fanRenderer.h"
#include "renderer/fanUIMesh.h"
#include "scene/components/fanTransform.h"
#include "scene/fanGameobject.h"

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
			 UIVertex( glm::vec2( -1.f, -1.f ) )			
			,UIVertex( glm::vec2( 1.f, 1.f ) )
			,UIVertex( glm::vec2( 1.f, -1.f  ) )

			,UIVertex( glm::vec2( -1.f, -1.f ) )
			,UIVertex( glm::vec2( -1.f, 1.f ) )
			,UIVertex( glm::vec2( 1.f, 1.f ) )

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