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
		m_gameobject->AddEcsComponent<ecsModel>()->Init();
		m_gameobject->AddEcsComponent<ecsConvexHull>()->Init();
		onRegisterModel.Emmit( this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Model::OnDetach() {
		Component::OnDetach();
		m_gameobject->RemoveEcsComponent<ecsModel>();
		m_gameobject->RemoveEcsComponent<ecsConvexHull>();
		onUnRegisterModel.Emmit(this);

		m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_TRANSFORM );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Model::SetMesh(Mesh * _mesh) {
		GetEcsModel()->mesh = _mesh;
		if( _mesh != nullptr && ! _mesh->GetIndices().empty() ) {				
			_mesh->GenerateConvexHull( GetConvexHull() );
			m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_TRANSFORM );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Model::SetPath( std::string _path ) {
		onModelSetPath.Emmit( this, _path );
	}

	Mesh *			Model::GetMesh() { return GetEcsModel()->mesh; }
	const Mesh *	Model::GetMesh() const { return GetEcsModel()->mesh; }

	int		Model::GetRenderID() const { return GetEcsModel()->renderID; }
	void	Model::SetRenderID( const int _renderID ) { GetEcsModel()->renderID = _renderID; }

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
		ecsModel* model = GetEcsModel();

		SaveString( _json, "path", ( model->mesh != nullptr ? model->mesh->GetPath() : "" ));
		Component::Save( _json );
		
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	ecsModel*		Model::GetEcsModel() const		{ return	m_gameobject->GetEcsComponent<ecsModel>(); }
	ecsConvexHull * Model::GetEcsConvexHull() const { return	m_gameobject->GetEcsComponent<ecsConvexHull>(); }
	ConvexHull&		Model::GetConvexHull() const	{ return	GetEcsConvexHull()->convexHull; }
}