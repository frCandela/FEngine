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

	Signal< Model * > Model::onRegisterModel;
	Signal< Model * > Model::onUnRegisterModel;
	Signal< Model *, std::string  >	Model::onModelSetPath;

	//================================================================================================================================
	//================================================================================================================================
	void Model::OnAttach() {
		Component::OnAttach();
		GetGameobject()->AddEcsComponent<ecsModel>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Model::OnDetach() {
		Component::OnDetach();
		GetGameobject()->RemoveEcsComponent<ecsModel>();
		onUnRegisterModel.Emmit(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	AABB Model::ComputeAABB() const {
		const glm::mat4 modelMatrix = GetGameobject()->GetComponent<Transform>()->GetModelMatrix();
		Mesh * mesh = GetEcsModel()->mesh;

		const std::vector<Vertex> &  vertices = mesh->GetVertices();

		const ConvexHull * hull = mesh->GetConvexHull();
		if (hull != nullptr) {
			return AABB(hull->GetVertices(), modelMatrix);
		}
		else {
			std::vector<btVector3> btVertices;
			btVertices.reserve(vertices.size());
			for (int vertIndex = 0; vertIndex < vertices.size(); vertIndex++) {
				btVertices.push_back(ToBullet(vertices[vertIndex].pos));
			}
			return AABB(btVertices, modelMatrix);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Model::SetMesh(Mesh * _mesh) {
		GetEcsModel()->mesh = _mesh;
		onRegisterModel.Emmit(this);
		MarkModified(true);
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
	ecsModel* Model::GetEcsModel() const { return GetGameobject()->GetEcsComponent<ecsModel>(); }
}