#include "render/core/fanFrameBuffer.hpp"

#include <cassert>
#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	FrameBuffer::FrameBuffer( Device& _device, const VkExtent2D _extent )
		: m_device( _device )
		, m_extent( _extent )
	{}

	//================================================================================================================================
	//================================================================================================================================
	FrameBuffer::~FrameBuffer()
	{
		// color attachment
		m_colorImageView.Destroy( m_device );
		m_colorImage.Destroy( m_device );
		m_colorSampler.Destroy( m_device );

		// depth attachment
		m_depthImageView.Destroy( m_device );
		m_depthImage.Destroy( m_device );

		DestroyFrameBuffers();
	}

	//================================================================================================================================
	//================================================================================================================================
	void FrameBuffer::DestroyFrameBuffers()
	{
		for ( int framebufferIndex = 0; framebufferIndex < m_frameBuffers.size(); framebufferIndex++ )
		{
			vkDestroyFramebuffer( m_device.device, m_frameBuffers[ framebufferIndex ], nullptr );
		} m_frameBuffers.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool FrameBuffer::Create( const size_t _count, VkRenderPass _renderPass )
	{
		m_count = _count;
		m_renderPass = _renderPass;

		std::vector<VkImageView> commonAttachments;
		if( m_colorImageView.imageView != VK_NULL_HANDLE ) { commonAttachments.push_back( m_colorImageView.imageView ); }
		if( m_depthImageView.imageView != VK_NULL_HANDLE ) { commonAttachments.push_back( m_depthImageView.imageView ); }

		assert( m_frameBuffers.empty() );
		m_frameBuffers.resize( _count );
		for ( int frameIndex = 0; frameIndex < _count; frameIndex++ )
		{
			std::vector<VkImageView> attachments = commonAttachments;
			if ( m_externalAttachments != nullptr )
			{
				attachments.push_back( m_externalAttachments[ frameIndex ].imageView );
			}

			VkFramebufferCreateInfo framebufferCreateInfo;
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.pNext = nullptr;
			framebufferCreateInfo.flags = 0;
			framebufferCreateInfo.renderPass = _renderPass;
			framebufferCreateInfo.attachmentCount = static_cast< uint32_t >( attachments.size() );
			framebufferCreateInfo.pAttachments = attachments.data();
			framebufferCreateInfo.width = m_extent.width;
			framebufferCreateInfo.height = m_extent.height;
			framebufferCreateInfo.layers = 1;

			if ( vkCreateFramebuffer( m_device.device, &framebufferCreateInfo, nullptr, &m_frameBuffers[ frameIndex ] ) != VK_SUCCESS )
			{
				Debug::Error( "Could not create framebuffer" );
				return false;
			} Debug::Get() << Debug::Severity::log << std::hex << "VkFramebuffer         " << m_frameBuffers[ frameIndex ] << std::dec << Debug::Endl();
		}
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void FrameBuffer::Resize( const VkExtent2D _extent )
	{
		m_extent = _extent;

		// Rebuild color attachment
		if ( m_colorImageView.imageView != VK_NULL_HANDLE )
		{
			m_colorImage.Destroy( m_device );
			m_colorImageView.Destroy( m_device );
			CreateColorImageAndView( m_device, m_colorFormat, _extent );
		}

		// Rebuild depth attachment
		if ( m_depthImageView.imageView != VK_NULL_HANDLE )
		{
			CreateDepthResources( m_device );
		}

		DestroyFrameBuffers();
		Create( m_count, m_renderPass );
	}

	//================================================================================================================================
	// Only one depth attachment needed for all swapchain images
	//================================================================================================================================
	void FrameBuffer::AddDepthAttachment()
	{
		assert( m_depthImageView.imageView == VK_NULL_HANDLE );
		depthFormat = m_device.FindDepthFormat();
		CreateDepthResources( m_device );
	}

	//================================================================================================================================
	// Only one color attachment needed for all swapchain images
	//================================================================================================================================
	void FrameBuffer::AddColorAttachment( const VkFormat _format, const VkExtent2D _extent )
	{
		assert( m_colorImageView.imageView == VK_NULL_HANDLE );

		m_colorFormat = _format;

		m_colorSampler.Create( m_device, 0, 1.f, VK_FILTER_LINEAR );

		CreateColorImageAndView( m_device, _format, _extent );
	}

	//================================================================================================================================
	//================================================================================================================================
	void FrameBuffer::CreateColorImageAndView( Device& _device, const VkFormat _format, const VkExtent2D _extent )
	{
		m_colorImage.Create(_device, _format, _extent,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		m_colorImageView.Create( _device, m_colorImage.image, _format, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D );
	}

	//================================================================================================================================
	//================================================================================================================================
	void FrameBuffer::CreateDepthImageAndView( Device& _device, const VkFormat _format, const VkExtent2D _extent )
	{
		m_depthImage.Create( _device, _format, _extent, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		m_depthImageView.Create( _device, m_depthImage.image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D );
	}

	//================================================================================================================================
	//================================================================================================================================
	bool FrameBuffer::CreateDepthResources( Device& _device )
	{
		
		if ( m_depthImage.image == VK_NULL_HANDLE )
		{
			CreateDepthImageAndView( _device, depthFormat, m_extent );			
		}
		else
		{
			m_depthImage.Destroy( _device );
			m_depthImageView.Destroy( _device );
			CreateDepthImageAndView( _device, depthFormat, m_extent );
		}

		VkCommandBuffer cmd = _device.BeginSingleTimeCommands();
		m_depthImage.TransitionImageLayout( cmd, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1 );
		_device.EndSingleTimeCommands( cmd );

		return true;
	}
}