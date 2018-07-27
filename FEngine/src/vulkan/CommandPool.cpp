#include "vulkan/CommandPool.h"

namespace vk
{

	CommandPool::CommandPool(Device& device) : m_rDevice(device)
	{
		QueueFamilyIndices queueFamilyIndices = m_rDevice.findQueueFamilies(m_rDevice.physicalDevice);

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT & VK_COMMAND_POOL_CREATE_TRANSIENT_BIT; // Optional		

		if (vkCreateCommandPool(m_rDevice.device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
			throw std::runtime_error("failed to create command pool!");
	}

	// Allocate a temporary command buffer for memory transfer operations and start recording
	VkCommandBuffer CommandPool::BeginSingleTimeCommands()
	{
		// Allocate a temporary command buffer for memory transfer operations
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(m_rDevice.device, &allocInfo, &commandBuffer);

		// Start recording the command buffer
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	// Execute a command buffer to complete the transfer and cleans it
	void CommandPool::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);

		// Execute the command buffer to complete the transfer
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(m_rDevice.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_rDevice.graphicsQueue);

		// Cleaning
		vkFreeCommandBuffers(m_rDevice.device, commandPool, 1, &commandBuffer);
	}

	CommandPool::~CommandPool()
	{
		vkDestroyCommandPool(m_rDevice.device, commandPool, nullptr);
	}
}