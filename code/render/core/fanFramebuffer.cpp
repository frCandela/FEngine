#include "render/core/fanFrameBuffer.hpp"

#include <cassert>
#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"
#include "render/core/fanRenderPass.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void FrameBuffer::Create( Device& _device, const size_t _count, const VkExtent2D _extent, RenderPass& _renderPass, ImageView* _externalAttachments )
	{
		std::vector<VkImageView> commonAttachments;
		if( m_colorImageView.mImageView != VK_NULL_HANDLE ) { commonAttachments.push_back( m_colorImageView.mImageView ); }
		if( m_depthImageView.mImageView != VK_NULL_HANDLE ) { commonAttachments.push_back( m_depthImageView.mImageView ); }

		for( int frameIndex = 0; frameIndex < _count; frameIndex++ )
		{
			assert( mFrameBuffers[frameIndex] == VK_NULL_HANDLE );
			std::vector<VkImageView> attachments = commonAttachments;
			if( _externalAttachments != nullptr )
			{
				attachments.push_back( _externalAttachments[frameIndex].mImageView );
			}

			VkFramebufferCreateInfo framebufferCreateInfo;
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.pNext = nullptr;
			framebufferCreateInfo.flags = 0;
			framebufferCreateInfo.renderPass = _renderPass.mRenderPass;
			framebufferCreateInfo.attachmentCount = static_cast<uint32_t>( attachments.size() );
			framebufferCreateInfo.pAttachments = attachments.data();
			framebufferCreateInfo.width = _extent.width;
			framebufferCreateInfo.height = _extent.height;
			framebufferCreateInfo.layers = 1;

			if( vkCreateFramebuffer( _device.mDevice, &framebufferCreateInfo, nullptr, &mFrameBuffers[frameIndex] ) != VK_SUCCESS )
			{
				Debug::Error( "Could not create framebuffer" );
			} Debug::Get() << Debug::Severity::log << std::hex << "VkFramebuffer         " << mFrameBuffers[frameIndex] << std::dec << Debug::Endl();
		}
	}


	//================================================================================================================================
	//================================================================================================================================
	void FrameBuffer::Destroy( Device& _device )
	{
		// color attachment
		m_colorImageView.Destroy( _device );
		m_colorImage.Destroy( _device );
		m_colorSampler.Destroy( _device );

		// depth attachment
		m_depthImageView.Destroy( _device );
		m_depthImage.Destroy( _device );

		DestroyFrameBuffers( _device );
	}

	//================================================================================================================================
	//================================================================================================================================
	void FrameBuffer::DestroyFrameBuffers( Device& _device )
	{
		for ( int framebufferIndex = 0; framebufferIndex < SwapChain::s_maxFramesInFlight; framebufferIndex++ )
		{
			if( mFrameBuffers[framebufferIndex] != VK_NULL_HANDLE )
			{
				vkDestroyFramebuffer( _device.mDevice, mFrameBuffers[framebufferIndex], nullptr );
				mFrameBuffers[framebufferIndex] = VK_NULL_HANDLE;
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void FrameBuffer::Resize( Device& _device, const size_t _count, const VkExtent2D _extent, RenderPass& _renderPass, ImageView* _externalAttachments )
	{
		// Rebuild color attachment
		if ( m_colorImageView.mImageView != VK_NULL_HANDLE )
		{
			m_colorImage.Destroy( _device );
			m_colorImageView.Destroy( _device );
			CreateColorImageAndView( _device, m_colorFormat, _extent );
		}

		// Rebuild depth attachment
		if ( m_depthImageView.mImageView != VK_NULL_HANDLE )
		{
			CreateDepthResources( _device, _extent );
		}

		DestroyFrameBuffers( _device );
		Create( _device, _count, _extent, _renderPass, _externalAttachments );
	}

	//================================================================================================================================
	// Only one depth attachment needed for all swapchain images
	//================================================================================================================================
	void FrameBuffer::AddDepthAttachment( Device& _device, const VkExtent2D _extent )
	{
		assert( m_depthImageView.mImageView == VK_NULL_HANDLE );		
		CreateDepthResources( _device, _extent );
	}

	//================================================================================================================================
	// Only one color attachment needed for all swapchain images
	//================================================================================================================================
	void FrameBuffer::AddColorAttachment( Device& _device, const VkFormat _format, const VkExtent2D _extent )
	{
		assert( m_colorImageView.mImageView == VK_NULL_HANDLE );

		m_colorFormat = _format;

		m_colorSampler.Create( _device, 0, 1.f, VK_FILTER_LINEAR );

		CreateColorImageAndView( _device, _format, _extent );
	}

	//================================================================================================================================
	//================================================================================================================================
	void FrameBuffer::CreateColorImageAndView( Device& _device, const VkFormat _format, const VkExtent2D _extent )
	{
		m_colorImage.Create(_device, _format, _extent,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		m_colorImageView.Create( _device, m_colorImage.mImage, _format, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D );
	}

	//================================================================================================================================
	//================================================================================================================================
	void FrameBuffer::CreateDepthImageAndView( Device& _device, const VkFormat _format, const VkExtent2D _extent )
	{
		m_depthImage.Create( _device, _format, _extent, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		m_depthImageView.Create( _device, m_depthImage.mImage, _format, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D );
	}

	//================================================================================================================================
	//================================================================================================================================
	bool FrameBuffer::CreateDepthResources( Device& _device, const VkExtent2D _extent )
	{
		VkFormat depthFormat = _device.FindDepthFormat();

		if ( m_depthImage.mImage == VK_NULL_HANDLE )
		{
			CreateDepthImageAndView( _device, depthFormat, _extent );			
		}
		else
		{
			m_depthImage.Destroy( _device );
			m_depthImageView.Destroy( _device );
			CreateDepthImageAndView( _device, depthFormat, _extent );
		}

		VkCommandBuffer cmd = _device.BeginSingleTimeCommands();
		m_depthImage.TransitionImageLayout( cmd, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1 );
		_device.EndSingleTimeCommands( cmd );

		return true;
	}
}