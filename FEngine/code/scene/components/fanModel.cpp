#include "fanGlobalIncludes.h"

#include "fanEngine.h"
#include "renderer/fanRenderer.h"
#include "renderer/fanMesh.h"
#include "renderer/fanRessourceManager.h"
#include "scene/components/fanModel.h"
#include "scene/components/fanTransform.h"
#include "scene/fanEntity.h"
#include "core/math/shapes/fanAABB.h"
#include "core/math/shapes/fanConvexHull.h"
#include "core/files/fanFbxImporter.h"

// Editor
#include "editor/fanModals.h"

namespace fan
{
	namespace scene
	{
		REGISTER_EDITOR_COMPONENT(Model);
		REGISTER_TYPE_INFO(Model)

		fan::Signal< Model * > Model::onRegisterModel;
		fan::Signal< Model * > Model::onUnRegisterModel;

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
		shape::AABB Model::ComputeAABB() const {
			const glm::mat4 modelMatrix = GetEntity()->GetComponent<scene::Transform>()->GetModelMatrix();
			const std::vector<vk::Vertex> &  vertices = m_mesh->GetVertices();

			const shape::ConvexHull * hull = m_mesh->GetConvexHull();
			if (hull != nullptr) {
				return shape::AABB(hull->GetVertices(), modelMatrix);
			} else {
				std::vector<btVector3> btVertices;
				btVertices.reserve(vertices.size());
				for (int vertIndex = 0; vertIndex < vertices.size(); vertIndex++) {
					btVertices.push_back(ToBullet(vertices[vertIndex].pos));
				}
				return shape::AABB(btVertices, modelMatrix);
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void Model::SetMesh(fan::Mesh * _mesh) {
			m_mesh = _mesh;
			onRegisterModel.Emmit(this);
			SetModified(true);
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

				vk::RessourceManager * ressourceManager = Renderer::Get().GetRessourceManager();
				fan::Mesh * mesh = ressourceManager->FindMesh(m_pathBuffer.string().c_str());
				if (mesh == nullptr) {
					mesh = ressourceManager->LoadMesh(m_pathBuffer.string());

				}
				SetMesh(mesh);
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		bool Model::Load(std::istream& _in) {
			std::string pathBuffer;
			if (!ReadSegmentHeader(_in, "path:")) { return false; }
			if (!ReadString(_in, pathBuffer)) { return false; }

			vk::RessourceManager * ressourceManager = Renderer::Get().GetRessourceManager();
			fan::Mesh * mesh = ressourceManager->FindMesh(pathBuffer);
			if (mesh == nullptr) {
				mesh = ressourceManager->LoadMesh(pathBuffer);
			}
			SetMesh(mesh);
			return true;
		}

		//================================================================================================================================
		//================================================================================================================================
		bool Model::Save(std::ostream& _out, const int _indentLevel) const {
			const std::string indentation = GetIndentation(_indentLevel);
			_out << indentation << "path: " <<  m_mesh->GetPath() << std::endl;
			return true;
		}
	}
}