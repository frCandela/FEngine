#include "Commands.h"

#include <array>

Commands::Commands(Device& device) :
	m_device(device)
{
	createCommandPool();
}

Commands::~Commands()
{
	vkFreeCommandBuffers(m_device.device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
	vkDestroyCommandPool(m_device.device, commandPool, nullptr);
}

void Commands::cleanup()
{
	vkFreeCommandBuffers(m_device.device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
}

// Create the command pool (for ordering drawing operations, memory transfers etc.)
void Commands::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = m_device.findQueueFamilies(m_device.physicalDevice);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
	poolInfo.flags = 0; // Optional

	if (vkCreateCommandPool(m_device.device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
		throw std::runtime_error("failed to create command pool!");
}



// Allocate a temporary command buffer for memory transfer operations and start recording
VkCommandBuffer Commands::beginSingleTimeCommands()
{
	// Allocate a temporary command buffer for memory transfer operations
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(m_device.device, &allocInfo, &commandBuffer);

	// Start recording the command buffer
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

// Execute a command buffer to complete the transfer and cleans it
void Commands::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	// Execute the command buffer to complete the transfer
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(m_device.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_device.graphicsQueue);

	// Cleaning
	vkFreeCommandBuffers(m_device.device, commandPool, 1, &commandBuffer);
}