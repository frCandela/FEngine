#include "fanGlobalIncludes.h"

#include "renderer/fanRenderer.h"
#include "renderer/fanMesh.h"
#include "scene/components/fanModel.h"
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
	REGISTER_EDITOR_COMPONENT(Model);
	REGISTER_TYPE_INFO(Model)

	Signal< Model * >				Model::onRegisterModel;
	Signal< Model * >				Model::onUnRegisterModel;
	Signal< Model *, std::string  >	Model::onModelSetPath;

	//================================================================================================================================
	//================================================================================================================================
	void Model::OnAttach() {
		Component::OnAttach();

		ecsMesh ** tmpMesh = &const_cast<ecsMesh*>( m_mesh );
		*tmpMesh = m_gameobject->AddEcsComponent<ecsMesh>();
		m_mesh->Init();

		ecsConvexHull ** tmpHull = &const_cast<ecsConvexHull*>( m_convexHull );
		*tmpHull = m_gameobject->AddEcsComponent<ecsConvexHull>();
		m_convexHull->Init();

		onRegisterModel.Emmit( this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Model::OnDetach() {
		Component::OnDetach();
		m_gameobject->RemoveEcsComponent<ecsMesh>();
		m_gameobject->RemoveEcsComponent<ecsConvexHull>();
		onUnRegisterModel.Emmit(this);

		m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_AABB );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Model::SetMesh( Mesh * _mesh )
	{
		m_mesh->mesh = _mesh;
		if( _mesh != nullptr && ! _mesh->GetIndices().empty() ) {	
			ConvexHull & convexHull = GetConvexHull();
			if( m_autoUpdateHull || convexHull.IsEmpty() ) {
				_mesh->GenerateConvexHull( convexHull );
			}
			m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_AABB );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Model::SetPath( std::string _path ) {		
 		onModelSetPath.Emmit( this, _path );
	}

	Mesh *			Model::GetMesh() { return m_mesh->mesh; }
	const Mesh *	Model::GetMesh() const { return m_mesh->mesh; }

	int		Model::GetRenderID() const { return m_mesh->renderID; }
	void	Model::SetRenderID( const int _renderID ) { m_mesh->renderID = _renderID; }

	//================================================================================================================================
	//================================================================================================================================
	void Model::OnGui() {
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

		if (gui::LoadFileModal("set_path", GlobalValues::s_meshExtensions, m_pathBuffer)) {
			onModelSetPath.Emmit( this, m_pathBuffer.string() );
		}

		// Num triangles
		std::stringstream ss;
		ss << "triangles: ";
		ss << ( GetMesh() != nullptr ?   GetMesh()->GetIndices().size() / 3  :  0 );		
		ImGui::Text(ss.str().c_str());

	}

	//================================================================================================================================
	//================================================================================================================================
	bool Model::Load( Json & _json ) {
		std::string pathBuffer;
		if ( LoadString( _json, "path", pathBuffer ) ) {
			onModelSetPath.Emmit( this, pathBuffer );
		}
		return true;
	}

	//==========================z======================================================================================================
	//================================================================================================================================
	bool Model::Save( Json & _json ) const {
		SaveString( _json, "path", ( m_mesh->mesh != nullptr ? m_mesh->mesh->GetPath() : "" ));
		Component::Save( _json );
		
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	ConvexHull&		Model::GetConvexHull() const	{ return	m_convexHull->convexHull; }
}