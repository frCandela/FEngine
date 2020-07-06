#pragma once

#include "glfw/glfw3.h"
#include <vector>
#include "render/core/fanImage.hpp"
#include "render/core/fanImageView.hpp" 

namespace fan
{
	class Device;
	class Sampler;
	class Image;
	class ImageView;

	//================================================================================================================================
	// render target with attachments (color, depth, etc. )
	//================================================================================================================================
	class FrameBuffer
	{
	public:
		FrameBuffer( Device& _device, const VkExtent2D _extent );
		~FrameBuffer();

		void AddDepthAttachment();
		void AddColorAttachment( const VkFormat _format, const VkExtent2D _extent );
		void SetExternalAttachment( std::vector< ImageView > * _perFramebufferViews );
		bool Create( const size_t _count, VkRenderPass _renderPass );
		void Resize( const VkExtent2D _extent );

		VkFramebuffer	Get( const size_t _index ) { return m_frameBuffers[ _index ]; }
		Sampler*		GetColorAttachmentSampler() { return m_colorSampler; }
		ImageView*		GetColorAttachmentImageView() { return &m_colorImageView; }
		VkExtent2D		GetExtent() const { return m_extent; }
	private:
		Device& m_device;

		std::vector<VkFramebuffer> m_frameBuffers;
		VkRenderPass m_renderPass;
		size_t m_count;

		// External attachment
		std::vector< ImageView > * m_externalAttachments = nullptr;

		// Depth attachment
		VkFormat depthFormat;
		Image m_depthImage;
		ImageView m_depthImageView;

		// Color attachment
		VkFormat	m_colorFormat;
		VkExtent2D	m_extent;
		Sampler* m_colorSampler = nullptr;
		Image m_colorImage;
		ImageView m_colorImageView;

		void CreateColorImageAndView( Device& _device, const VkFormat _format, const VkExtent2D _extent );
		void CreateDepthImageAndView( Device& _device, const VkFormat _format, const VkExtent2D _extent );
		bool CreateDepthResources( Device& _device );
		void DestroyFrameBuffers();
	};
}