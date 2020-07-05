#include "render/core/fanRenderPass.hpp"

#include <cassert>
#include "render/core/fanDevice.hpp"
#include "core/fanDebug.hpp"


namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	RenderPass::RenderPass( Device& _device ) :
		m_device( _device )
	{}

	//================================================================================================================================
	//================================================================================================================================
	RenderPass::~RenderPass()
	{
		vkDestroyRenderPass( m_device.vkDevice, m_renderPass, nullptr );
	}

	//================================================================================================================================
	//================================================================================================================================
	VkAttachmentDescription& RenderPass::AddInputAttachment()
	{
		VkAttachmentReference inputAttachmentRef;
		inputAttachmentRef.attachment = static_cast< uint32_t >( m_attachments.size() );
		inputAttachmentRef.layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		m_inputAttachmentsRef.push_back( inputAttachmentRef );

		VkAttachmentDescription inputAttachment;

		m_attachments.push_back( inputAttachment );
		return  m_attachments[ m_attachments.size() - 1 ];
	}

	//================================================================================================================================
	//================================================================================================================================
	VkAttachmentDescription& RenderPass::AddColorAttachment( const VkFormat _format, const VkImageLayout _finalLayout )
	{
		VkAttachmentReference colorAttachmentRef;
		colorAttachmentRef.attachment = static_cast< uint32_t >( m_attachments.size() );
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		m_colorAttachmentsRef.push_back( colorAttachmentRef );

		VkAttachmentDescription colorAttachment;
		colorAttachment.flags = 0;
		colorAttachment.format = _format;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = _finalLayout;

		m_attachments.push_back( colorAttachment );
		return  m_attachments[ m_attachments.size() - 1 ];
	}

	//================================================================================================================================
	//================================================================================================================================
	VkAttachmentDescription& RenderPass::AddDepthAttachment( const VkFormat _format )
	{
		VkAttachmentReference depthAttachmentRef;
		depthAttachmentRef.attachment = static_cast< uint32_t >( m_attachments.size() );
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		m_depthAttachmentsRef.push_back( depthAttachmentRef );

		VkAttachmentDescription depthAttachment;
		depthAttachment.flags = 0;
		depthAttachment.format = _format;
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		m_attachments.push_back( depthAttachment );
		return  m_attachments[ m_attachments.size() - 1 ];
	}

	//================================================================================================================================
	//================================================================================================================================
	VkSubpassDependency& RenderPass::AddDependency()
	{
		VkSubpassDependency dependency;
		dependency.srcSubpass = 0;
		dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dependency.dependencyFlags = 0;

		m_dependencies.push_back( dependency );
		return  m_dependencies[ m_dependencies.size() - 1 ];
	}

	//================================================================================================================================
	//================================================================================================================================
	VkRenderPassBeginInfo RenderPass::GetBeginInfo( VkRenderPass _renderPass, VkFramebuffer _frameBuffer, VkExtent2D _extent, const VkClearValue* _clearValue, uint32_t _clearCount )
	{
		VkRenderPassBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		beginInfo.pNext = nullptr;
		beginInfo.renderPass = _renderPass;
		beginInfo.framebuffer = _frameBuffer;
		beginInfo.renderArea.offset = { 0,0 };
		beginInfo.renderArea.extent.width = _extent.width;
		beginInfo.renderArea.extent.height = _extent.height;
		beginInfo.clearValueCount = _clearCount;
		beginInfo.pClearValues = _clearValue;

		return beginInfo;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool RenderPass::Create()
	{
		assert( m_depthAttachmentsRef.size() == 0 || m_depthAttachmentsRef.size() == 1 );

		VkSubpassDescription subpassDescription;
		subpassDescription.flags = 0;
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.inputAttachmentCount = static_cast< uint32_t >( m_inputAttachmentsRef.size() );
		subpassDescription.pInputAttachments = m_inputAttachmentsRef.data();
		subpassDescription.colorAttachmentCount = static_cast< uint32_t >( m_colorAttachmentsRef.size() );
		subpassDescription.pColorAttachments = m_colorAttachmentsRef.data();
		subpassDescription.pResolveAttachments = nullptr;
		subpassDescription.pDepthStencilAttachment = m_depthAttachmentsRef.empty() ? nullptr : &m_depthAttachmentsRef[ 0 ];
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;

		std::vector<VkSubpassDescription> subpassDescriptions = { subpassDescription };

		VkRenderPassCreateInfo renderPassCreateInfo;
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.pNext = nullptr;
		renderPassCreateInfo.flags = 0;
		renderPassCreateInfo.attachmentCount = static_cast< uint32_t >( m_attachments.size() );
		renderPassCreateInfo.pAttachments = m_attachments.data();
		renderPassCreateInfo.subpassCount = static_cast< uint32_t >( subpassDescriptions.size() );;
		renderPassCreateInfo.pSubpasses = subpassDescriptions.data();
		renderPassCreateInfo.dependencyCount = static_cast< uint32_t >( m_dependencies.size() );;
		renderPassCreateInfo.pDependencies = m_dependencies.data();

		if ( vkCreateRenderPass( m_device.vkDevice, &renderPassCreateInfo, nullptr, &m_renderPass ) != VK_SUCCESS )
		{
			Debug::Error( "Could not create render pass" );
			return false;
		}
		Debug::Get() << Debug::Severity::log << std::hex << "VkRenderPass          " << m_renderPass << std::dec << Debug::Endl();

		return true;
	}
}