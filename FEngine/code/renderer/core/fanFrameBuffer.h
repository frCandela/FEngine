#pragma once

namespace fan
{
	class Device;
	class Sampler;
	class Image;
	class ImageView;

	//================================================================================================================================
	//================================================================================================================================
	class FrameBuffer {
	public:
		FrameBuffer(Device & _device);
		~FrameBuffer();

		void AddDepthAttachment( const VkExtent2D _extent );
		void AddColorAttachment( const VkExtent2D _extent, const VkFormat _format );
		void SetExternalAttachment( const std::vector< ImageView * > _perFramebufferViews );
		bool Create( const size_t _count, VkRenderPass _renderPass, const VkExtent2D _extent );
		void Resize( const VkExtent2D _extent );

		VkFramebuffer GetFrameBuffer( const size_t _index ) { return m_frameBuffers[_index]; }
		Sampler*	  GetColorAttachmentSampler() { return m_colorSampler; }
		ImageView*   GetColorAttachmentImageView() { return m_colorImageView; }

	private:
		Device &	m_device;

		std::vector<VkFramebuffer> m_frameBuffers;
		VkRenderPass m_renderPass;
		size_t m_count;

		// External attachment
		std::vector< ImageView * > m_externalAttachments;

		// Depth attachment
		Image *		m_depthImage	 = nullptr;
		ImageView *	m_depthImageView = nullptr;

		// Color attachment
		VkFormat	m_colorFormat;
		VkExtent2D	m_colorExtend;
		Sampler *	m_colorSampler	 = nullptr;
		Image *		m_colorImage	 = nullptr;
		ImageView *	m_colorImageView = nullptr;

		void CreateColorRessources( const VkExtent2D _extent, const VkFormat _format );
		bool CreateDepthRessources( const VkExtent2D _extent );
		void DestroyFrameBuffers();
	};
}