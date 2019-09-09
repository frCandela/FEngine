#include "fanGlobalIncludes.h"

#include "fanEngine.h"
#include "renderer/fanRenderer.h"
#include "renderer/fanRessourceManager.h"
#include "scene/components/fanModel.h"
#include "scene/components/fanTransform.h"
#include "scene/fanEntity.h"
#include "core/math/shapes/fanAABB.h"
#include "renderer/fanMesh.h"
#include "core/files/fanFbxImporter.h"


namespace fan
{
	namespace scene
	{
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
		void Model::Initialize() {
		}

		//================================================================================================================================
		//================================================================================================================================
		void Model::Delete() {
			onUnRegisterModel.Emmit(this);
		}

		//================================================================================================================================
		//================================================================================================================================
		shape::AABB Model::ComputeAABB() const {
			const scene::Transform * transform = GetEntity()->GetComponent<scene::Transform>();
			const glm::mat4 modelMatrix = transform->GetModelMatrix();

			const std::vector<uint32_t> & indices = m_mesh->GetIndices();
			const std::vector<vk::Vertex> &  vertices = m_mesh->GetVertices();

			if (indices.size() > 0) {
				glm::vec3 high(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
				glm::vec3 low(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());

				for (int index = 0; index < indices.size(); index++) {
					const glm::vec4 vertex = modelMatrix * glm::vec4(vertices[index].pos, 1.f);
					if (vertex.x < low.x) { low.x = vertex.x; }
					if (vertex.y < low.y) { low.y = vertex.y; }
					if (vertex.z < low.z) { low.z = vertex.z; }
					if (vertex.x > high.x) { high.x = vertex.x; }
					if (vertex.y > high.y) { high.y = vertex.y; }
					if (vertex.z > high.z) { high.z = vertex.z; }
				}
				return shape::AABB(ToBullet(low), ToBullet(high));

			}
			else {
				return shape::AABB();
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void Model::SetMesh(fan::Mesh * _mesh) {
			m_mesh = _mesh;
			onRegisterModel.Emmit(this);
			GetEntity()->onComponentModified.Emmit(this);
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