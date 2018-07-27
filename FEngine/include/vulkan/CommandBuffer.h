#pragma once

#include "Device.h"
#include "CommandPool.h"

namespace vk
{
	class CommandBuffer
	{
	public:
		CommandBuffer(Device& device, CommandPool& commandPool);
		~CommandBuffer();		
		
		std::vector<VkCommandBuffer> commandBuffers;

		void cleanup();		
		void CreateBuffer(size_t size);
		void Begin(size_t index);
		void End(size_t index);

	private:
		Device & m_device;
		CommandPool& m_rCommandPool;
	};
}
