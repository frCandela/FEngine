#pragma once

#include "Device.h"

namespace vk
{
	/*
	Command pools are objects from which command buffers acquire their memory. 
	Memory itself is allocated implicitly and dynamically, but without it command buffers wouldn'thave any storage space to hold the recorded commands.
	*/
	class CommandPool
	{
	public:
		/// Create the command pool (for ordering drawing operations, memory transfers etc.)
		CommandPool(Device& device);
		~CommandPool();

		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

		VkCommandPool commandPool;

	private:
		Device & m_rDevice;
		
	};
}

