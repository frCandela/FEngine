#include "Buffer.h"

Buffer::Buffer( Device& device ) :
	m_device(device)
{

}

Buffer::~Buffer()
{	
	// Destroy index buffer
	vkDestroyBuffer(m_device.device, indexBuffer, nullptr);
	vkFreeMemory(m_device.device, indexBufferMemory, nullptr);

	// Destroy vertex buffer
	vkDestroyBuffer(m_device.device, vertexBuffer, nullptr);
	vkFreeMemory(m_device.device, vertexBufferMemory, nullptr);
}



// Create the index buffer
void Buffer::createIndexBuffer(  )
{
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	// Create a host visible buffer
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(m_device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	// Fills it with data
	void* data;
	vkMapMemory(m_device.device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_device.device, stagingBufferMemory);

	// Create a device local buffer
	createBuffer(m_device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);
	copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	// Cleaning
	vkDestroyBuffer(m_device.device, stagingBuffer, nullptr);
	vkFreeMemory(m_device.device, stagingBufferMemory, nullptr);
}

// Helper function for creating buffers
void Buffer::createBuffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	// Buffer info structure
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device.device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		throw std::runtime_error("failed to create buffer!");

	// Query memory requirements
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device.device, buffer, &memRequirements);

	// Allow memory to buffer
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = device.findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device.device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate buffer memory!");

	vkBindBufferMemory(device.device, buffer, bufferMemory, 0);
}

// Copy the contents from one buffer to another
void Buffer::copyBuffer( VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = m_device.beginSingleTimeCommands();

	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	m_device.endSingleTimeCommands(commandBuffer);
}

// Creates a vertex buffer for test rendering
void Buffer::createVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	// Create a host visible buffer
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(m_device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	// Fills it with data
	void* data;
	vkMapMemory(m_device.device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_device.device, stagingBufferMemory);

	// Create a device local buffer
	createBuffer(m_device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	// Cleaning
	vkDestroyBuffer(m_device.device, stagingBuffer, nullptr);
	vkFreeMemory(m_device.device, stagingBufferMemory, nullptr);

}