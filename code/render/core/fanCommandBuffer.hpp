#pragma once

#include "glfw/glfw3.h"
#include "render/core/fanDevice.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	struct CommandBuffer
	{
	public:
		bool Create( Device& _device, const int _size, const VkCommandBufferLevel _level );

		static VkCommandBufferInheritanceInfo GetInheritanceInfo( VkRenderPass _renderPass, VkFramebuffer _framebuffer );
		static VkCommandBufferBeginInfo GetBeginInfo( VkCommandBufferInheritanceInfo* _inheritanceInfo );

		VkCommandBuffer buffers[3] = { VK_NULL_HANDLE, VK_NULL_HANDLE,VK_NULL_HANDLE };
		int size;
	};
}