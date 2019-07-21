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
		Mesh();
		Mesh(Gameobject * _gameobject);
		~Mesh();
		
		void SetPath(const std::string _path);
		std::string GetPath() const { return m_path; }

		std::vector<vk::Vertex> &		GetVertices() {return m_vertices; }
		const std::vector<vk::Vertex> & GetVertices() const { return m_vertices; }
		std::vector<uint32_t> &			GetIndices() { return m_indices; }
		const std::vector<uint32_t> &	GetIndices() const { return m_indices; }
		shape::AABB ComputeAABB() const;

		bool			IsUnique()	const override { return true; }
		const char *	GetName()	const override { return s_name; }
		uint32_t		GetType()	const override { return s_type; }

		// ISerializable
		void Load(std::istream& _in) override;
		void Save(std::ostream& _out) override;

		const static char * s_name;
		static const uint32_t s_type;

	private:
		std::string m_path;
		std::vector<vk::Vertex> m_vertices;
		std::vector<uint32_t> m_indices;

	protected:
		Component *		NewInstance(Gameobject * _gameobject) const override { return new Mesh(_gameobject); }
	};
}