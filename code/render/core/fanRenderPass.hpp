#pragma once

#include "glfw/glfw3.h"
#include <vector>

namespace fan
{
	struct Device;

	//================================================================================================================================
	// abstraction of a vulkan render pass
	//================================================================================================================================
	struct RenderPass
	{
		bool Create( Device& _device, 	VkAttachmentDescription* _attachmentdescriptions,	uint32_t _countAttachments,  
										VkSubpassDescription*	 _subpassDescriptions,		uint32_t _countSubpasses,
										VkSubpassDependency*	 _dependencies,				uint32_t _countDependencies );
		void Destroy( Device& _device );

		static VkAttachmentDescription	GetColorAttachment( const VkFormat _format, const VkImageLayout _finalLayout );
		static VkAttachmentDescription	GetDepthAttachment( const VkFormat _format );
		static VkAttachmentReference	GetColorAttachmentReference( const uint32_t _index );
		static VkAttachmentReference	GetDepthAttachmentReference( const uint32_t _index );
		static VkSubpassDependency		GetDependency();
		static VkSubpassDescription		GetSubpassDescription( VkAttachmentReference* _colorReferences, uint32_t _count, VkAttachmentReference* _depthReference );
		static VkRenderPassBeginInfo	GetBeginInfo( VkRenderPass _renderPass, VkFramebuffer _frameBuffer, VkExtent2D _extent, const VkClearValue* _clearValue, uint32_t _clearCount );

		VkRenderPass mRenderPass = VK_NULL_HANDLE;
	};
}