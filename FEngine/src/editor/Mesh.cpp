#include "editor/Mesh.h"
#include "editor/Transform.h"

#include "vulkan/CommandBuffer.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

Mesh::Mesh()
{

}

bool Mesh::LoadModel(std::string path)
{
	// Load a model into the library's data structures
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str()))
	{
		std::cerr << err << std::endl;
		return false;
	}

	m_path = path;

	vertices.clear();
	indices.clear();

	std::unordered_map<ForwardPipeline::Vertex, uint32_t> uniqueVertices = {};

	//combine all of the faces in the file into a single model
	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			ForwardPipeline::Vertex vertex = {};

			vertex.pos =
			{
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			if (index.texcoord_index > 0)
			{
				vertex.texCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};
			}

			vertex.color = { 1.0f, 1.0f, 1.0f };

			vertex.normal = 
			{ 
				 attrib.normals[3 * index.normal_index + 0]
				,attrib.normals[3 * index.normal_index + 1]
				,attrib.normals[3 * index.normal_index + 2]
			};

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}

	return true;
}

void Mesh::RenderGui()
{
	Component::RenderGui();		

	// Set path popup
	if (ImGui::BeginPopup("set_path"))
	{
		ImGui::InputText("path: ", m_pathBuffer.data(), m_pathBuffer.size());
		if (ImGui::Button("Update"))
		{
			if (LoadModel(m_pathBuffer.data()))
			{
				m_wasModified = true;
				GetGameobject()->GetComponent<Transform>()->SetScale({ 1.f,1.f,1.f });
			}
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel"))		
			ImGui::CloseCurrentPopup();
		
		ImGui::EndPopup();
	}

	ImGui::Text("path: %s", m_path.c_str());
	ImGui::SameLine();
	// Set path button (open popup)
	if (ImGui::Button("Set##set_path_button"))
	{
		std::size_t len = m_path.copy(m_pathBuffer.data(), m_pathBuffer.size());
		m_pathBuffer[len] = '\0';

		ImGui::OpenPopup("set_path");
	}
}

