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
		RenderPass( Device& _device );
		~RenderPass();
		bool Create();
		VkRenderPass Get() const { return m_renderPass; }

		VkAttachmentDescription& AddInputAttachment();
		VkAttachmentDescription& AddColorAttachment( const VkFormat _format, const VkImageLayout _finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		VkAttachmentDescription& AddDepthAttachment( const VkFormat _format );
		VkSubpassDependency&	 AddDependency();

		static VkRenderPassBeginInfo GetBeginInfo( VkRenderPass _renderPass, VkFramebuffer _frameBuffer, VkExtent2D _extent, const VkClearValue* _clearValue, uint32_t _clearCount );

		VkRenderPassCreateInfo m_renderPassCreateInfo;

	private:
		Device& m_device;
		VkRenderPass	m_renderPass = VK_NULL_HANDLE;

		std::vector<VkAttachmentDescription> m_attachments;
		std::vector<VkAttachmentReference>	 m_depthAttachmentsRef;
		std::vector<VkAttachmentReference>	 m_colorAttachmentsRef;
		std::vector<VkAttachmentReference>	 m_inputAttachmentsRef;
		std::vector<VkSubpassDependency> m_dependencies;
	};
}