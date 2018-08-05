#include "editor/Mesh.h"

#include "vulkan/CommandBuffer.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

void Mesh::LoadModel(std::string path)
{
	// Load a model into the library's data structures
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str()))
		throw std::runtime_error(err);

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

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}
}

void Mesh::RenderGui()
{
	Component::RenderGui();
}

