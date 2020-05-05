#pragma once

#include "fanGLFW.hpp"
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
		VkRenderPass GetRenderPass() const { return m_renderPass; }

		VkAttachmentDescription& AddInputAttachment();
		VkAttachmentDescription& AddColorAttachment( const VkFormat _format, const VkImageLayout _finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		VkAttachmentDescription& AddDepthAttachment( const VkFormat _format );
		VkSubpassDependency& AddDependency();

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