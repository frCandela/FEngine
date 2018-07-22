#include "vulkan/Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace vk
{

	Mesh::Mesh(Device& device) :
		m_device(device)
	{

	}

	Mesh::~Mesh()
	{
		// Destroy index buffer
		vkDestroyBuffer(m_device.device, indexBuffer, nullptr);
		vkFreeMemory(m_device.device, indexBufferMemory, nullptr);

		// Destroy vertex buffer
		vkDestroyBuffer(m_device.device, vertexBuffer, nullptr);
		vkFreeMemory(m_device.device, vertexBufferMemory, nullptr);
	}
	


	// Copy the contents from one buffer to another
	void Mesh::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		VkCommandBuffer commandBuffer = m_device.commands->beginSingleTimeCommands();

		VkBufferCopy copyRegion = {};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		m_device.commands->endSingleTimeCommands(commandBuffer);
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

		std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

		//combine all of the faces in the file into a single model
		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex = {};

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

		CreateVertexBuffer();
		CreateIndexBuffer();
	}

	void Mesh::CreateVertexBuffer()
	{
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		// Create a host visible buffer
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		vks::Buffer::createBuffer(m_device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		// Fills it with data
		void* data;
		vkMapMemory(m_device.device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(m_device.device, stagingBufferMemory);

		// Create a device local buffer
		vks::Buffer::createBuffer(m_device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
		copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

		// Cleaning
		vkDestroyBuffer(m_device.device, stagingBuffer, nullptr);
		vkFreeMemory(m_device.device, stagingBufferMemory, nullptr);

	}

	void Mesh::CreateIndexBuffer()
	{
		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		// Create a host visible buffer
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		vks::Buffer::createBuffer(m_device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		// Fills it with data
		void* data;
		vkMapMemory(m_device.device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), (size_t)bufferSize);
		vkUnmapMemory(m_device.device, stagingBufferMemory);

		// Create a device local buffer
		vks::Buffer::createBuffer(m_device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);
		copyBuffer(stagingBuffer, indexBuffer, bufferSize);

		// Cleaning
		vkDestroyBuffer(m_device.device, stagingBuffer, nullptr);
		vkFreeMemory(m_device.device, stagingBufferMemory, nullptr);
	}
}
