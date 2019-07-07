#pragma once

#include "scene/components/fanComponent.h"
#include "vulkan/util/vkVertex.h"

namespace scene
{
	class Gameobject;

	class Mesh : public Component
	{
	public:
		Mesh(Gameobject * _gameobject) : Component(_gameobject) {};
		~Mesh();
		
		void SetPath( const std::string _path) { m_path =  _path; }
		std::string GetPath() const { return m_path; }

		std::vector<vk::Vertex> & GetVertices() {return m_vertices; }
		std::vector<uint32_t> & GetIndices() { return m_indices; }

		bool IsUnique() const override { return true; }
		std::string GetName() const override { return "mesh"; }

	private:
		std::string m_path;
		std::vector<vk::Vertex> m_vertices;
		std::vector<uint32_t> m_indices;

	};
}