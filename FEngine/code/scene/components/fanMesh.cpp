#include "fanIncludes.h"

#include "scene/components/fanMesh.h"
#include "scene/components/fanTransform.h"
#include "scene/fanGameobject.h"
#include "fanEngine.h"
#include "util/shapes/fanAABB.h"
#include "vulkan/vkRenderer.h"

namespace scene
{
	//================================================================================================================================
	//================================================================================================================================
	Mesh::~Mesh() {
		fan::Engine::GetEngine().GetRenderer().RemoveMesh(this);		
	}
	
	//================================================================================================================================
	//================================================================================================================================
	shape::AABB Mesh::ComputeAABB() const {
		const scene::Transform * transform = GetGameobject()->GetComponent<scene::Transform>();
		const glm::mat4 modelMatrix = transform->GetModelMatrix();

		if (m_indices.size() > 0) {
			glm::vec3 high(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
			glm::vec3 low(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());

			for (int index = 0; index < m_indices.size(); index++) {
				const glm::vec4 vertex = modelMatrix * glm::vec4( m_vertices[index].pos, 1.f );
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
}