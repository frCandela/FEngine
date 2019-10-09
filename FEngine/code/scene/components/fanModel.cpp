#include "fanGlobalIncludes.h"

#include "renderer/fanRenderer.h"
#include "renderer/fanMesh.h"
#include "scene/components/fanModel.h"
#include "scene/components/fanTransform.h"
#include "scene/fanGameobject.h"
#include "core/math/shapes/fanAABB.h"
#include "core/math/shapes/fanConvexHull.h"
#include "core/files/fanFbxImporter.h"

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
	Model::Model() :
		m_mesh(nullptr) {
	}

	//================================================================================================================================
	//================================================================================================================================
	void Model::OnDetach() {
		Component::OnDetach();
		onUnRegisterModel.Emmit(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	AABB Model::ComputeAABB() const {
		const glm::mat4 modelMatrix = GetGameobject()->GetComponent<Transform>()->GetModelMatrix();
		const std::vector<Vertex> &  vertices = m_mesh->GetVertices();

		const ConvexHull * hull = m_mesh->GetConvexHull();
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
		m_mesh = _mesh;
		onRegisterModel.Emmit(this);
		MarkModified(true);
	}

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
	bool Model::Load(std::istream& _in) {
		std::string pathBuffer;
		if (!ReadSegmentHeader(_in, "path:")) { return false; }
		if (!ReadString(_in, pathBuffer)) { return false; }

		onModelSetPath.Emmit(this, pathBuffer );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Model::Save(std::ostream& _out, const int _indentLevel) const {
		const std::string indentation = GetIndentation(_indentLevel);
		_out << indentation << "path: " << m_mesh->GetPath() << std::endl;
		return true;
	}
}