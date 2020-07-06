#pragma once

#include "glfw/glfw3.h"
#include <vector>

namespace fan
{
	class Device;

	//================================================================================================================================
	// abstraction of a vulkan render pass
	//================================================================================================================================
	class RenderPass
	{
	public:
		bool Create( VkDevice _device, std::vector<VkAttachmentDescription> _attachmentdescriptions, std::vector<VkSubpassDescription> _subpassDescriptions, std::vector<VkSubpassDependency> _dependencies );
		void Destroy( VkDevice _device );

		static VkAttachmentDescription	GetColorAttachment( const VkFormat _format, const VkImageLayout _finalLayout );
		static VkAttachmentDescription	GetDepthAttachment( const VkFormat _format );
		static VkAttachmentReference	GetColorAttachmentReference( const uint32_t _index );
		static VkAttachmentReference	GetDepthAttachmentReference( const uint32_t _index );
		static VkSubpassDependency		GetDependency();
		static VkSubpassDescription		GetSubpassDescription( VkAttachmentReference* _colorReferences, uint32_t _count, VkAttachmentReference* _depthReference );
		static VkRenderPassBeginInfo	GetBeginInfo( VkRenderPass _renderPass, VkFramebuffer _frameBuffer, VkExtent2D _extent, const VkClearValue* _clearValue, uint32_t _clearCount );

		VkRenderPass renderPass = VK_NULL_HANDLE;
	};
}