#pragma once

#include "glfw/glfw3.h"
#include <vector>
#include "render/core/fanImage.hpp"
#include "render/core/fanImageView.hpp" 
#include "render/core/fanSampler.hpp"
#include "render/core/fanSwapChain.hpp"

namespace fan
{
	struct Device;
	struct RenderPass;

	//================================================================================================================================
	// render target with attachments (color, depth, etc. )
	//================================================================================================================================
	struct FrameBuffer
	{
		void Create( Device& _device, const size_t _count, const VkExtent2D _extent, RenderPass& _renderPass, ImageView* _externalAttachments = nullptr );
		void Resize( Device& _device, const size_t _count, const VkExtent2D _extent, RenderPass& _renderPass, ImageView* _externalAttachments = nullptr );
		void Destroy( Device& _device );

		void AddDepthAttachment( Device& _device, const VkExtent2D _extent );
		void AddColorAttachment( Device& _device, const VkFormat _format, const VkExtent2D _extent );

		VkFramebuffer mFrameBuffers[ SwapChain::s_maxFramesInFlight ] = { VK_NULL_HANDLE,VK_NULL_HANDLE,VK_NULL_HANDLE };

		// Depth attachment
		Image m_depthImage;
		ImageView m_depthImageView;

		// Color attachment
		VkFormat	m_colorFormat;
		Sampler		m_colorSampler;
		Image		m_colorImage;
		ImageView	m_colorImageView;

		void CreateColorImageAndView( Device& _device, const VkFormat _format, const VkExtent2D _extent );
		void CreateDepthImageAndView( Device& _device, const VkFormat _format, const VkExtent2D _extent );
		bool CreateDepthResources( Device& _device, const VkExtent2D _extent );
		void DestroyFrameBuffers( Device& _device );
	};
}