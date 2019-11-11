#include "fanGlobalIncludes.h"
#include "scene/components/fanMeshRenderer.h"

#include "renderer/fanRenderer.h"
#include "renderer/fanMesh.h"
#include "scene/components/fanTransform.h"
#include "scene/fanGameobject.h"
#include "core/math/shapes/fanAABB.h"
#include "core/math/shapes/fanConvexHull.h"
#include "core/files/fanFbxImporter.h"
#include "ecs/fanECSConfig.h"
#include "core/time/fanProfiler.h"

// Editor
#include "editor/fanModals.h"

namespace fan
{
	REGISTER_EDITOR_COMPONENT(MeshRenderer);
	REGISTER_TYPE_INFO(MeshRenderer)

	Signal< MeshRenderer * >				MeshRenderer::onRegisterMeshRenderer;
	Signal< MeshRenderer * >				MeshRenderer::onUnRegisterMeshRenderer;
	Signal< MeshRenderer *, std::string  >	MeshRenderer::onMeshRendererSetPath;

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer::OnAttach() {
		Component::OnAttach();

		ecsMesh ** tmpMesh = &const_cast<ecsMesh*>( m_mesh );
		*tmpMesh = m_gameobject->AddEcsComponent<ecsMesh>();
		m_mesh->Init();

		onRegisterMeshRenderer.Emmit( this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer::OnDetach() {
		Component::OnDetach();
		m_gameobject->RemoveEcsComponent<ecsMesh>();
		onUnRegisterMeshRenderer.Emmit(this);

		m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_AABB );
	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer::SetMesh( Mesh * _mesh )
	{
		m_mesh->mesh = _mesh;
		if( _mesh != nullptr && ! _mesh->GetIndices().empty() ) {	
			m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_AABB );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer::SetPath( std::string _path ) {		
 		onMeshRendererSetPath.Emmit( this, _path );
	}

	Mesh *			MeshRenderer::GetMesh() { return m_mesh->mesh; }
	const Mesh *	MeshRenderer::GetMesh() const { return m_mesh->mesh; }

	int		MeshRenderer::GetRenderID() const { return m_mesh->renderID; }
	void	MeshRenderer::SetRenderID( const int _renderID ) { m_mesh->renderID = _renderID; }

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer::OnGui() {
		Component::OnGui();
		// Set path popup
		bool openSetPathPopup = false;
		if (ImGui::Button("##setPath")) {
			openSetPathPopup = true;
		}
		ImGui::SameLine();
		const std::string meshPath = GetMesh() != nullptr ? GetMesh()->GetPath() : "not set";
		ImGui::Text("path: %s", meshPath.c_str());
		// Set path  popup on double click
		if (openSetPathPopup || ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
			if (GetMesh() != nullptr && GetMesh()->GetPath().empty() == false) {
				m_pathBuffer = std::fs::path(GetMesh()->GetPath()).parent_path();
			}
			else {
				m_pathBuffer = "./";
			}
			ImGui::OpenPopup("set_path");
			m_pathBuffer = "content/models";
		}

		if (ImGui::FanLoadFileModal("set_path", GlobalValues::s_meshExtensions, m_pathBuffer)) {
			onMeshRendererSetPath.Emmit( this, m_pathBuffer.string() );
		}

		// Num triangles
		std::stringstream ss;
		ss << "triangles: ";
		ss << ( GetMesh() != nullptr ?   GetMesh()->GetIndices().size() / 3  :  0 );		
		ImGui::Text(ss.str().c_str());

	}

	//================================================================================================================================
	//================================================================================================================================
	bool MeshRenderer::Load( Json & _json ) {
		std::string pathBuffer;
		if ( LoadString( _json, "path", pathBuffer ) ) {
			onMeshRendererSetPath.Emmit( this, pathBuffer );
		}
		return true;
	}

	//==========================z======================================================================================================
	//================================================================================================================================
	bool MeshRenderer::Save( Json & _json ) const {
		SaveString( _json, "path", ( m_mesh->mesh != nullptr ? m_mesh->mesh->GetPath() : "" ));
		Component::Save( _json );
		
		return true;
	}
}