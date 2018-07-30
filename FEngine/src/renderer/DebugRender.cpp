#include "renderer/DebugRender.h"


RenderDebug::RenderDebug(vk::Device& rDevice) :
	m_rDevice(rDevice),
	m_debugBuffer(rDevice)
{
}

void RenderDebug::Draw(VkCommandBuffer commandBuffer)
{
	if (m_verticesDebug.size() > 0)
	{
		VkBuffer vertexBuffers[] = { m_debugBuffer.m_buffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdDraw(commandBuffer, static_cast<uint32_t>(m_verticesDebug.size()), 1, 0, 0);
	}
}

void RenderDebug::Clear()
{
	m_verticesDebug.clear();
}

void RenderDebug::UpdateDebugBuffer(vk::CommandPool& commandPool)
{
	VkDeviceSize bufferSize = sizeof(m_verticesDebug[0]) * m_verticesDebug.size();

	// Create a host visible buffer
	vk::Buffer buf(m_rDevice);
	buf.CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, bufferSize);

	// Fills it with data
	buf.Map(bufferSize);
	memcpy(buf.mappedData, m_verticesDebug.data(), (size_t)bufferSize);
	buf.Unmap();

	// Create a device local buffer	
	m_debugBuffer.Destroy();
	m_debugBuffer.CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferSize);

	// Fills it with the data from the first buffer
	vk::Mesh::copyBuffer(buf.m_buffer, m_debugBuffer.m_buffer, bufferSize, commandPool);
}


void RenderDebug::DebugLine(glm::vec3 start, glm::vec3 end, glm::vec4 color)
{
	m_verticesDebug.push_back({ start , color });
	m_verticesDebug.push_back({ end , color });
}