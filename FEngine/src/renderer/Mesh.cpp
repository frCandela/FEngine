#include "renderer/Mesh.h"

#include "vulkan/CommandBuffer.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace vk
{
	Mesh::Mesh(Device& device) :
		m_device(device)
		,vertexBuffer(device)
		,indexBuffer(device)
	{

	}

	Mesh::~Mesh()
	{
	}

	// Copy the contents from one buffer to another
	void Mesh::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, CommandPool& rCommandPool)
	{
		VkCommandBuffer commandBuffer = rCommandPool.BeginSingleTimeCommands();

		VkBufferCopy copyRegion = {};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		rCommandPool.EndSingleTimeCommands(commandBuffer);
	}

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

	void Mesh::CreateBuffers(CommandPool& rCommandPool)
	{
		CreateVertexBuffer(rCommandPool);
		CreateIndexBuffer(rCommandPool);
	}

	void Mesh::CreateVertexBuffer(CommandPool& rCommandPool)
	{
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		// Create a host visible buffer
		vk::Buffer buf(m_device);
		buf.CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, bufferSize);

		// Fills it with data
		buf.Map(bufferSize);
		memcpy(buf.mappedData, vertices.data(), (size_t)bufferSize);
		buf.Unmap();

		// Create a device local buffer
		vertexBuffer.CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferSize);

		copyBuffer(buf.m_buffer, vertexBuffer.m_buffer, bufferSize, rCommandPool);
	}

	void Mesh::CreateIndexBuffer(CommandPool& rCommandPool)
	{
		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		// Create a host visible buffer
		vk::Buffer buf(m_device);
		buf.CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, bufferSize);

		// Fills it with data
		buf.Map(bufferSize);
		memcpy(buf.mappedData, indices.data(), (size_t)bufferSize);
		buf.Unmap();

		// Create a device local buffer		
		indexBuffer.CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferSize);
		
		copyBuffer(buf.m_buffer, indexBuffer.m_buffer, bufferSize, rCommandPool);
	}
}