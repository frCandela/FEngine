#include "render/core/fanFrameBuffer.hpp"
#include "render/core/fanDevice.hpp"
#include "render/core/fanSampler.hpp"
#include "render/core/fanImage.hpp"
#include "render/core/fanImageView.hpp" 


namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	FrameBuffer::FrameBuffer( Device & _device, const VkExtent2D _extent )
		: m_device( _device )
		, m_extent( _extent )
	{}

	//================================================================================================================================
	//================================================================================================================================
	FrameBuffer::~FrameBuffer()
	{
		// color attachment
		delete m_colorImageView;
		delete m_colorImage;
		delete m_colorSampler;

		// depth attachment
		delete m_depthImageView;
		delete m_depthImage;

		DestroyFrameBuffers();
	}

	//================================================================================================================================
	//================================================================================================================================
	void FrameBuffer::DestroyFrameBuffers()
	{
		for ( int framebufferIndex = 0; framebufferIndex < m_frameBuffers.size(); framebufferIndex++ )
		{
			vkDestroyFramebuffer( m_device.vkDevice, m_frameBuffers[framebufferIndex], nullptr );
		} m_frameBuffers.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool FrameBuffer::Create( const size_t _count, VkRenderPass _renderPass )
	{
		m_count = _count;
		m_renderPass = _renderPass;

		std::vector<VkImageView> commonAttachments;
		if ( m_colorImageView != nullptr ) { commonAttachments.push_back( m_colorImageView->GetImageView() ); }
		if ( m_depthImageView != nullptr ) { commonAttachments.push_back( m_depthImageView->GetImageView() ); }

		assert( m_frameBuffers.empty() );
		m_frameBuffers.resize( _count );
		for ( int frameIndex = 0; frameIndex < _count; frameIndex++ )
		{
			std::vector<VkImageView> attachments = commonAttachments;
			if ( !m_externalAttachments.empty() )
			{
				assert( m_externalAttachments.size() == _count );
				attachments.push_back( m_externalAttachments[frameIndex]->GetImageView() );
			}

			VkFramebufferCreateInfo framebufferCreateInfo;
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.pNext = nullptr;
			framebufferCreateInfo.flags = 0;
			framebufferCreateInfo.renderPass = _renderPass;
			framebufferCreateInfo.attachmentCount = static_cast<uint32_t>( attachments.size() );
			framebufferCreateInfo.pAttachments = attachments.data();
			framebufferCreateInfo.width = m_extent.width;
			framebufferCreateInfo.height = m_extent.height;
			framebufferCreateInfo.layers = 1;

			if ( vkCreateFramebuffer( m_device.vkDevice, &framebufferCreateInfo, nullptr, &m_frameBuffers[frameIndex] ) != VK_SUCCESS )
			{
				Debug::Error( "Could not create framebuffer" );
				return false;
			} Debug::Get() << Debug::Severity::log << std::hex << "VkFramebuffer         " << m_frameBuffers[frameIndex] << std::dec << Debug::Endl();
		}
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void FrameBuffer::Resize( const VkExtent2D _extent )
	{
		m_extent = _extent;

		// Rebuild color attachment
		if ( m_colorImageView != nullptr )
		{
			m_colorImage->Resize( _extent );
			m_colorImageView->SetImage( m_colorImage->GetImage() );
		}

		// Rebuild depth attachment
		if ( m_depthImageView != nullptr )
		{
			CreateDepthRessources();
		}

		DestroyFrameBuffers();
		Create( m_count, m_renderPass );
	}

	//================================================================================================================================
	// Only one color attachment needed for all swapchain images
	//================================================================================================================================
	void FrameBuffer::SetExternalAttachment( std::vector< ImageView * > _perFramebufferViews )
	{
		m_externalAttachments = _perFramebufferViews;
	}


	//================================================================================================================================
	// Only one depth attachment needed for all swapchain images
	//================================================================================================================================
	void FrameBuffer::AddDepthAttachment()
	{
		assert( m_depthImageView == nullptr );
		CreateDepthRessources();
	}

	//================================================================================================================================
	// Only one color attachment needed for all swapchain images
	//================================================================================================================================
	void FrameBuffer::AddColorAttachment( const VkFormat _format )
	{
		assert( m_colorImageView == nullptr );

		m_colorFormat = _format;

		//Sampler
		m_colorSampler = new Sampler( m_device );
		m_colorSampler->CreateSampler( 0, 1.f, VK_FILTER_LINEAR );

		CreateColorRessources( _format );
	}

	//================================================================================================================================
	//================================================================================================================================
	void FrameBuffer::CreateColorRessources( const VkFormat _format )
	{
		delete m_colorImage;
		m_colorImage = new Image( m_device );
		m_colorImage->Create(
			_format,
			m_extent,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		delete m_colorImageView;
		m_colorImageView = new ImageView( m_device );
		m_colorImageView->Create( m_colorImage->GetImage(), _format, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D );
	}

	//================================================================================================================================
	//================================================================================================================================
	bool FrameBuffer::CreateDepthRessources()
	{
		VkFormat depthFormat = m_device.FindDepthFormat();
		if ( m_depthImage == nullptr )
		{
			m_depthImage = new Image( m_device );
			m_depthImageView = new ImageView( m_device );
			m_depthImage->Create( depthFormat, m_extent, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
			m_depthImageView->Create( m_depthImage->GetImage(), depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D );
		}
		else
		{
			m_depthImage->Resize( m_extent );
			m_depthImageView->SetImage( m_depthImage->GetImage() );
		}

		VkCommandBuffer cmd = m_device.BeginSingleTimeCommands();
		m_depthImage->TransitionImageLayout( cmd, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1 );
		m_device.EndSingleTimeCommands( cmd );

		return true;
	}
}