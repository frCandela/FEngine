#pragma once

#include "Device.h"

namespace vk
{
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

