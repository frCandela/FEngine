#pragma once

class Device;
class SwapChain;
class Buffer;

#include "SwapChain.h"
#include "Buffer.h"

class Commands
{
public:
	Commands( Device& device );
	~Commands();

	Device& m_device;
	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
	void cleanup();

	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	void createCommandPool();
	void createCommandBuffers(
		SwapChain& swapChain,
		VkRenderPass& renderPass,
		VkPipeline& pipeline,
		VkPipelineLayout& pipelineLayout,
		Buffer& buffer,
		VkDescriptorSet& descriptor);
};
