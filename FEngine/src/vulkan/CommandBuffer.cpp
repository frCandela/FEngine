#include "vulkan/CommandBuffer.h"

#include <array>

namespace vk
{

	CommandBuffer::CommandBuffer(Device& device, CommandPool& commandPool) :
		m_device(device),
		m_rCommandPool(commandPool)
	{
	}

	CommandBuffer::~CommandBuffer()
	{
		cleanup();
	}

	void CommandBuffer::cleanup()
	{
		if (commandBuffers.size() > 0)
		{
			vkFreeCommandBuffers(m_device.device, m_rCommandPool.commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
			commandBuffers.clear();
		}			
	}

	void CommandBuffer::CreateBuffer( size_t size)
	{
		assert(size > 0);
		assert(commandBuffers.size() == 0);

		commandBuffers.resize(size);

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_rCommandPool.commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

		if (vkAllocateCommandBuffers(m_device.device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
			throw std::runtime_error("failed to allocate command buffers!");
	}

	void CommandBuffer::Begin(size_t index )
	{
		// Specify the usage of the command buffer
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		if (vkBeginCommandBuffer(commandBuffers[index], &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("failed to begin recording command buffer!");
	}

	void CommandBuffer::End(size_t index)
	{
		if (vkEndCommandBuffer(commandBuffers[index]) != VK_SUCCESS)
			throw std::runtime_error("failed to record command buffer!");
	}
}