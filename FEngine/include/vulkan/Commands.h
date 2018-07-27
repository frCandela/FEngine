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
		void CreateBuffer(size_t size);
		void Begin(size_t index);
		void End(size_t index);
	};
}
