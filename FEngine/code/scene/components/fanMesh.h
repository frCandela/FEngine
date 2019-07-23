#pragma once

#include "scene/components/fanComponent.h"
#include "vulkan/util/vkVertex.h"

namespace shape {
	class AABB;
}

namespace scene
{
	class Gameobject;

	class Mesh : public Component
	{
	public:
		~Mesh();
		
		void SetPath(const std::string _path);
		std::string GetPath() const { return m_path; }

		std::vector<vk::Vertex> &		GetVertices() {return m_vertices; }
		const std::vector<vk::Vertex> & GetVertices() const { return m_vertices; }
		std::vector<uint32_t> &			GetIndices() { return m_indices; }
		const std::vector<uint32_t> &	GetIndices() const { return m_indices; }
		shape::AABB ComputeAABB() const;

		bool			IsUnique()	const override { return true; }

		// ISerializable
		void Load(std::istream& _in) override;
		void Save(std::ostream& _out) override;

		DECLARE_TYPE_INFO(Mesh);
	private:
		void Initialize() override;

		std::string m_path;
		std::vector<vk::Vertex> m_vertices;
		std::vector<uint32_t> m_indices;
	};
}