#include "render/core/fanCommandBuffer.hpp"
#include "core/fanDebug.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	bool CommandBuffer::Create( Device& _device, const int _size, const VkCommandBufferLevel _level )
	{
		assert( buffers[0] == VK_NULL_HANDLE && buffers[1] == VK_NULL_HANDLE&& buffers[2] == VK_NULL_HANDLE );

		size = _size;

		VkCommandBufferAllocateInfo commandBufferAllocateInfo;
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.pNext = nullptr;
		commandBufferAllocateInfo.commandPool = _device.GetCommandPool();
		commandBufferAllocateInfo.level = _level;
		commandBufferAllocateInfo.commandBufferCount = _size;

		if( vkAllocateCommandBuffers( _device.vkDevice, &commandBufferAllocateInfo, buffers ) != VK_SUCCESS )
		{
			Debug::Error( "Could not allocate command buffers." );
			return false;
		}
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	VkCommandBufferInheritanceInfo CommandBuffer::GetInheritanceInfo( VkRenderPass _renderPass, VkFramebuffer _framebuffer )
	{
		VkCommandBufferInheritanceInfo inheritanceInfo = {};
		inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		inheritanceInfo.pNext = nullptr;
		inheritanceInfo.renderPass = _renderPass;
		inheritanceInfo.subpass = 0;
		inheritanceInfo.framebuffer = _framebuffer;
		inheritanceInfo.occlusionQueryEnable = VK_FALSE;
		//commandBufferInheritanceInfo.queryFlags				=;
		//commandBufferInheritanceInfo.pipelineStatistics		=;
		return inheritanceInfo;
	}

	//================================================================================================================================
	//================================================================================================================================
	VkCommandBufferBeginInfo CommandBuffer::GetBeginInfo( VkCommandBufferInheritanceInfo* _inheritanceInfo )
	{
		VkCommandBufferBeginInfo beginInfo;
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.pNext = nullptr;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		beginInfo.pInheritanceInfo = _inheritanceInfo;

		return beginInfo;
	}
}