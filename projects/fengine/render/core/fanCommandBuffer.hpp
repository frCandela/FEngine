#pragma once

#include "glfw/glfw3.h"
#include "render/core/fanDevice.hpp"
#include "render/core/fanSwapChain.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	struct CommandBuffer
	{
	public:
		bool Create( Device& _device, const int _size, const VkCommandBufferLevel _level );

        static VkCommandBufferInheritanceInfo GetInheritanceInfo( VkRenderPass _renderPass,
                                                                  VkFramebuffer _framebuffer );
		static VkCommandBufferBeginInfo GetBeginInfo( VkCommandBufferInheritanceInfo* _inheritanceInfo );

        VkCommandBuffer mBuffers[SwapChain::sMaxFramesInFlight] = { VK_NULL_HANDLE,
                                                                    VK_NULL_HANDLE,
                                                                    VK_NULL_HANDLE };
	};
}