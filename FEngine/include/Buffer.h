#pragma once

#include "Device.h"
#include "Vertex.h"

class Buffer
{
public:
	Buffer(Device& device);
	~Buffer();
	Device & m_device;

	//Vertices and indices of the loaded model
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	//Buffers
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;



	static void createBuffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer( VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	
	/// Loads a model from an OBJ file
	void LoadModel( std::string path );

private:
	/// Creates a vertex buffer from the loaded model data
	void CreateVertexBuffer();

	/// Create the index buffer from the loaded model data
	void CreateIndexBuffer();
};