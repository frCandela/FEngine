#pragma once

#include "Device.h"
#include "SwapChain.h"

class RenderPass
{
public:
	RenderPass(Device& device, SwapChain& swapchain);
	~RenderPass()
	{
		vkDestroyRenderPass(m_device.device, renderPass, nullptr);
	}

	VkRenderPass renderPass;

	Device& m_device;
};