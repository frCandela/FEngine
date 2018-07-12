#pragma once

#include "Device.h"

namespace vk
{

	class Device;

	class Commands
	{
	public:
		Commands(Device& device);
		~Commands();

		Device& m_device;
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;
		void cleanup();

		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);
		void createCommandPool();
	};
}
