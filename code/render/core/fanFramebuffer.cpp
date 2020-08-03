#include "render/core/fanFrameBuffer.hpp"

#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"
#include "render/core/fanRenderPass.hpp"

namespace fan
{
	//================================================================================================================================
	// Creates _framesCount framebuffers with one different swapchain image view each
	//================================================================================================================================
	void FrameBuffer::CreateForSwapchain( Device& _device, const size_t _framesCount, const VkExtent2D _extent, RenderPass& _renderPass, ImageView* _swapchainImageViews )
	{		
		for( int i = 0; i < (int)_framesCount; i++ )
		{
			assert( mFrameBuffers[i] == VK_NULL_HANDLE );
		
			VkFramebufferCreateInfo framebufferCreateInfo;
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.pNext = nullptr;
			framebufferCreateInfo.flags = 0;
			framebufferCreateInfo.renderPass = _renderPass.mRenderPass;
			framebufferCreateInfo.attachmentCount = 1;
			framebufferCreateInfo.pAttachments = &_swapchainImageViews[i].mImageView;
			framebufferCreateInfo.width = _extent.width;
			framebufferCreateInfo.height = _extent.height;
			framebufferCreateInfo.layers = 1;
		
			if( vkCreateFramebuffer( _device.mDevice, &framebufferCreateInfo, nullptr, &mFrameBuffers[i] ) != VK_SUCCESS )
			{
				Debug::Error( "Could not create swapchain framebuffer" );
			} 
			Debug::Get() << Debug::Severity::log << std::hex << "VkFramebuffer         " << mFrameBuffers[i]  << " swapchain" << std::dec << Debug::Endl();
		
			_device.AddDebugName( (uint64_t)mFrameBuffers[i], "Framebuffer" );
		}
	}
	
	//================================================================================================================================
	// Create _framesCount framebuffers with the same attachments
	//================================================================================================================================
	void FrameBuffer::Create( Device& _device, const size_t _framesCount, const VkExtent2D _extent, RenderPass& _renderPass, const VkImageView* _attachments, uint32_t _attCount )
	{
		for( int i = 0; i < (int)_framesCount; i++ )
		{
			assert( mFrameBuffers[i] == VK_NULL_HANDLE );

			VkFramebufferCreateInfo framebufferCreateInfo;
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.pNext = nullptr;
			framebufferCreateInfo.flags = 0;
			framebufferCreateInfo.renderPass = _renderPass.mRenderPass;
			framebufferCreateInfo.attachmentCount = _attCount;
			framebufferCreateInfo.pAttachments = _attachments;
			framebufferCreateInfo.width = _extent.width;
			framebufferCreateInfo.height = _extent.height;
			framebufferCreateInfo.layers = 1;

			if( vkCreateFramebuffer( _device.mDevice, &framebufferCreateInfo, nullptr, &mFrameBuffers[i] ) != VK_SUCCESS )
			{
				Debug::Error( "Could not create framebuffer" );
			} Debug::Get() << Debug::Severity::log << std::hex << "VkFramebuffer         " << mFrameBuffers[i] << std::dec << Debug::Endl();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void FrameBuffer::Destroy( Device& _device )
	{
		for( int framebufferIndex = 0; framebufferIndex < SwapChain::s_maxFramesInFlight; framebufferIndex++ )
		{
			if( mFrameBuffers[framebufferIndex] != VK_NULL_HANDLE )
			{
				vkDestroyFramebuffer( _device.mDevice, mFrameBuffers[framebufferIndex], nullptr );
				_device.RemoveDebugName( (uint64_t)mFrameBuffers[framebufferIndex] );
				mFrameBuffers[framebufferIndex] = VK_NULL_HANDLE;
			}
		}
	}
}