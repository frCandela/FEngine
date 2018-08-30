#pragma once

#include "vulkan/Buffer.hpp"

// Mesh datas
struct MeshData
{
	vk::Buffer* vertexBuffer = nullptr;
	vk::Buffer* indexBuffer = nullptr;
	size_t indexBufferSize;
	size_t index = -1;
	glm::mat4 model = glm::mat4(1.f);	// Model matrix

	void Delete(vk::Device* device)
	{
		vkDeviceWaitIdle(device->device);
		delete(vertexBuffer);
		delete(indexBuffer);
	}
}; 
