#pragma once

#include "renderer/core/fanPipeline.h"

namespace fan
{
	class Sampler;
	class Image;
	class ImageView;
	class Buffer;
	class Descriptor;

	//================================================================================================================================
	//================================================================================================================================
	class PostprocessPipeline : public Pipeline {
	public:
		//================================================================
		//================================================================
		struct Uniforms {
			glm::vec4 color;
		} uniforms;

		PostprocessPipeline( Device& _device, const VkFormat _format, const VkExtent2D _extent );
		~PostprocessPipeline() override;

		void Resize( const VkExtent2D _extent ) override;
		void Bind( VkCommandBuffer _commandBuffer ) override;

		void			UpdateUniformBuffers();
		VkImageView		GetImageView();

	protected:
		void ConfigurePipeline() override;

	private:
		bool CreateDescriptors();
		void CreateImagesAndViews(VkExtent2D _extent);

		VkFormat m_format;

		Descriptor *	m_descriptor = nullptr;
		Sampler *		m_sampler;
		Image *			m_image;
		ImageView *		m_imageView;
	};
}