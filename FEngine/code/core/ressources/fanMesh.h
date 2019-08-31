#pragma once

#include "core/ressources/fanRessource.h"
#include "renderer/util/vkVertex.h"

namespace ressource {
	//================================================================================================================================
	//================================================================================================================================
	class Mesh : public Ressource {
	public:

		Mesh(const std::string& _path = "");

		void SetPath(const std::string& _path) { m_path = _path; }
		std::string GetPath() const { return m_path; }

		std::vector<vk::Vertex> &		GetVertices() { return m_vertices; }
		const std::vector<vk::Vertex> & GetVertices() const { return m_vertices; }
		std::vector<uint32_t> &			GetIndices() { return m_indices; }
		const std::vector<uint32_t> &	GetIndices() const { return m_indices; }

		void Load() override;

		static util::Signal< Mesh * > onMeshLoad;

		static const char * defaultMeshPath;

		DECLARE_TYPE_INFO(Mesh)
	private:
		std::string m_path;
		std::vector<vk::Vertex> m_vertices;
		std::vector<uint32_t> m_indices;
	};
}