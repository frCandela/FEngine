#pragma once

#include "vulkan/Buffer.hpp"
#include "vulkan/CommandPool.h"

#include "renderer/DebugPipeline.h"

// Class used to facilitate the usage of the DebugPipeline
class RenderDebug
{
public:
	RenderDebug(vk::Device& rDevice);

	// Draw the stored debug information on a commandBuffer
	void Draw(VkCommandBuffer commandBuffer);

	// Clears the stored debug information
	void Clear();

	// Updates the debug buffer
	void UpdateDebugBuffer(vk::CommandPool& commandPool);

	// Adds a Debug line for debug  rendering later
	void DebugLine(glm::vec3 start, glm::vec3 end, glm::vec4 color);

private:
	vk::Device& m_rDevice;

	std::vector<DebugPipeline::Vertex> m_verticesDebug;
	vk::Buffer m_debugBuffer;
};