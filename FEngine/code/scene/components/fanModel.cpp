#include "fanIncludes.h"

#include "scene/components/fanModel.h"
#include "scene/components/fanTransform.h"
#include "scene/fanGameobject.h"
#include "fanEngine.h"
#include "core/math/shapes/fanAABB.h"
#include "core/ressources/fanMesh.h"
#include "vulkan/vkRenderer.h"
#include "core/files/fanFbxImporter.h"

#include "core/ressources/fanMesh.h"


namespace scene
{
	REGISTER_TYPE_INFO(Model)
	util::Signal< Model * > Model::onRegisterModel;

	//================================================================================================================================
	//================================================================================================================================
	Model::Model( ) :
		m_mesh( nullptr ) {
	}

	//================================================================================================================================
	//================================================================================================================================
	void Model::Initialize() {

	}

	//================================================================================================================================
	//================================================================================================================================
	Model::~Model() {
		fan::Engine::GetEngine().GetRenderer().RemoveModel(this);		
	} 

	//================================================================================================================================
	//================================================================================================================================
	shape::AABB Model::ComputeAABB() const {
		const scene::Transform * transform = GetGameobject()->GetComponent<scene::Transform>();
		const glm::mat4 modelMatrix = transform->GetModelMatrix();

		const std::vector<uint32_t> & indices = m_mesh->GetIndices();
		const std::vector<vk::Vertex> &  vertices = m_mesh->GetVertices();

		if (indices.size() > 0) {
			glm::vec3 high(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
			glm::vec3 low(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());

			for (int index = 0; index < indices.size(); index++) {
				const glm::vec4 vertex = modelMatrix * glm::vec4(vertices[index].pos, 1.f );
				if (vertex.x < low.x) { low.x = vertex.x; }
				if (vertex.y < low.y) { low.y = vertex.y; }
				if (vertex.z < low.z) { low.z = vertex.z; }
				if (vertex.x > high.x) { high.x = vertex.x; }
				if (vertex.y > high.y) { high.y = vertex.y; }
				if (vertex.z > high.z) { high.z = vertex.z; }
			}
			return shape::AABB(util::ToBullet(low), util::ToBullet(high));

		} else {
			return shape::AABB();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Model::SetMesh(ressource::Mesh * _mesh) {
		m_mesh = _mesh;
		m_mesh->Load();
		onRegisterModel.Emmit(this);
		GetGameobject()->onComponentModified.Emmit(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Model::Load(std::istream& _in) {
		std::string path;
 		_in >> path;
		SetMesh(new ressource::Mesh(path));
	}

	//================================================================================================================================
	//================================================================================================================================
	void Model::Save(std::ostream& _out) {
		(void)_out;
		_out << "\t\t" << m_mesh->GetPath() << std::endl;
	}

}