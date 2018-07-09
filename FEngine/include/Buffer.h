#pragma once

#include "Device.h"
#include "Vertex.h"



class Buffer
{
public:
	Buffer(Device& device);
	~Buffer();
	const std::string MODEL_PATH = "models/cloud.obj";
	const std::string TEXTURE_PATH = "textures/chalet.jpg";


	Device & m_device;

	//Vertices and indices
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	//Buffers
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	void createIndexBuffer();

	static void createBuffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer( VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void loadModel();
	void createVertexBuffer();
};